/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

// Disable clang-analyzer-optin.cplusplus.VirtualCall since boost have bugs in boost::process::ipstream
// NOLINTBEGIN(clang-analyzer-optin.cplusplus.VirtualCall)
// cspell:ignore NOLINT

#ifdef _MSC_VER
#pragma warning(disable : 4996) // warning for sprintf() in Boost.log
#endif // _MSC_VER

#include "core/log/spy_logger.hpp"

#include "core/crypto/base64.hpp"
#include "core/crypto/password_keeper.hpp"
#include "core/exceptionmacros.hpp"
#include "core/macros.hpp"
#include "core/runtime/path.hpp"
#include "core/spy_log.hpp"
#include "core/tools/system.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/dll.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/current_process_id.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sinks/sink.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/process.hpp>

#include <csignal>
#include <filesystem>
#include <mutex>

namespace sight::core::log
{

// Protect static functions
static std::recursive_mutex s_mutex;

namespace trivial     = boost::log::trivial;
namespace keywords    = boost::log::keywords;
namespace expressions = boost::log::expressions;
namespace attributes  = boost::log::attributes;

using boost::posix_time::ptime;

BOOST_LOG_GLOBAL_LOGGER(lg, boost::log::sources::severity_logger_mt<trivial::severity_level>);
BOOST_LOG_GLOBAL_LOGGER_DEFAULT(lg, boost::log::sources::severity_logger_mt<trivial::severity_level>);

//-----------------------------------------------------------------------------

inline static auto console_stream_format()
{
    return expressions::stream << "["
           << expressions::attr<unsigned int>("LineID") << "]["
           << expressions::format_date_time<ptime::time_duration_type>("Uptime", "%H:%M:%S.%f") << "]["
           << expressions::attr<trivial::severity_level>("Severity") << "] "
           << expressions::smessage;
}

//------------------------------------------------------------------------------

inline static auto file_stream_format()
{
    return expressions::stream << "["
           << expressions::format_date_time<ptime>("TimeStamp", "%d.%m.%Y %H:%M:%S.%f") << "]["
           << expressions::format_date_time<ptime::time_duration_type>("Uptime", "%H:%M:%S.%f") << "]["
           << expressions::attr<attributes::current_process_id::value_type>("ProcessID") << "]["
           << expressions::attr<attributes::current_thread_id::value_type>("ThreadID") << "]["
           << expressions::attr<trivial::severity_level>("Severity") << "] "
           << expressions::smessage;
}

//-----------------------------------------------------------------------------

class spy_logger::spy_logger_impl final
{
public:

    // Delete default constructors and assignment operators
    spy_logger_impl(const spy_logger_impl&)            = delete;
    spy_logger_impl(spy_logger_impl&&)                 = delete;
    spy_logger_impl& operator=(const spy_logger_impl&) = delete;
    spy_logger_impl& operator=(spy_logger_impl&&)      = delete;

    // Constructor
    inline explicit spy_logger_impl(spy_logger* const _logger) noexcept :
        m_logger(_logger)
    {
    }

    // Destructor
    inline ~spy_logger_impl()
    {
        remove_sink();
    }

    // Returns the next log file name
    inline std::filesystem::path next_log_archive(const std::filesystem::path& _log_archive)
    {
        // Get the original log archive name and the current index
        if(m_original_log_archive.empty() && _log_archive.empty())
        {
            m_original_log_archive = ENCRYPTED_LOG_FILE;
            m_log_archive_index    = 0;
        }
        else if(!_log_archive.empty() && (m_original_log_archive.empty() || _log_archive != m_original_log_archive))
        {
            m_original_log_archive = _log_archive;
            m_log_archive_index    = 0;
        }
        else
        {
            m_log_archive_index++;
        }

        // Build the next log archive name
        std::filesystem::path log_archive_path(m_original_log_archive);

        auto original_log_archive = m_original_log_archive.filename().string();
        auto pos                  = original_log_archive.find_first_of('.');

        if(pos == std::string::npos)
        {
            pos = std::max(original_log_archive.size() - 1, std::size_t(0));
        }

        // Insert the log counter
        original_log_archive.insert(pos, "." + std::to_string(m_log_archive_index));

        log_archive_path.replace_filename(original_log_archive);

        return log_archive_path;
    }

    //------------------------------------------------------------------------------

    inline void remove_sink()
    {
        if(m_remote_sink)
        {
            // Remove the sink
            const auto log_ptr = boost::log::core::get();
            log_ptr->flush();
            log_ptr->remove_sink(m_remote_sink);
            m_remote_sink->flush();
            m_remote_sink.reset();
        }

        if(m_remote_in != nullptr)
        {
            m_remote_in->flush();
            m_remote_in->close();
            m_remote_in.reset();
        }

        if(m_remote_out != nullptr)
        {
            m_remote_out->sync();
            m_remote_out->close();
            m_remote_out.reset();
        }

        if(m_remote_process != nullptr)
        {
            m_remote_process->wait_for(std::chrono::seconds(3));
            m_remote_process->terminate();
            m_remote_process.reset();
        }

        m_remote_pid = 0;
    }

    // Launch the remote logger, either in raw mode or in encrypted mode
    inline void start_logger(
        const std::filesystem::path& _log_archive,
        level_t _level,
        const core::crypto::secure_string& _password = core::crypto::secure_string(),
        bool _ask_password                           = false
)
    {
        // If the logger is started, stop it by closing the input stream
        remove_sink();

        // Relax the pressure to be on the safe side on windows
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Find the logger binary
        const auto logger_path = get_logger_path();

        // Build the log archive name
        const auto& new_log_archive = next_log_archive(_log_archive);

        // Store the log archive name
        m_log_archives.emplace_back(new_log_archive.string());
        std::cerr << "\nLog archive: " << new_log_archive.string() << std::endl;

        // To avoid the logger to restart after changing the global password
        m_raw = (_password.empty() && !_ask_password);

        // Store the level, so we can launch the logger later when changing the password
        m_level = _level;

        // Create the input and output streams
        SIGHT_ASSERT("The input stream is not null", !m_remote_in);
        m_remote_in = std::make_unique<boost::process::opstream>();

        SIGHT_ASSERT("The input stream is not null", !m_remote_out);
        m_remote_out = std::make_unique<boost::process::ipstream>();

        // Build argument list
        std::vector<std::string> args = {
            "-b",
            "-o",
            core::crypto::to_base64(new_log_archive.string())
        };

        // Use raw mode if no password is provided or asked
        if(m_raw)
        {
            args.emplace_back("-r");
        }
        else
        {
            if(!_password.empty())
            {
                args.emplace_back("-p");
                args.emplace_back(core::crypto::to_base64(_password).c_str());
            }

            if(_ask_password)
            {
                args.emplace_back("-a");
            }
        }

        m_remote_process = std::make_unique<boost::process::child>(
            logger_path.string(),
            boost::process::args = args,
            boost::process::std_err > boost::process::null,
            boost::process::std_out > *m_remote_out,
            boost::process::std_in < *m_remote_in
        );

        // Wait for the logger to start
        std::size_t tries = 0;
        while(tries++ < 100 && !m_remote_out->good())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if(tries >= 100)
        {
            throw std::runtime_error("Unable to open the logger standard output stream");
        }

        // Read the PID of the logger
        m_remote_out->read(
            reinterpret_cast<char*>(&m_remote_pid),
            sizeof(m_remote_pid)
        );

        if(m_remote_out->gcount() != sizeof(m_remote_pid))
        {
            throw std::runtime_error("Unable to read the logger PID");
        }

        // And read the entered password, if any
        std::streamsize password_size = 0;
        m_remote_out->read(
            reinterpret_cast<char*>(&password_size),
            sizeof(password_size)
        );

        if(m_remote_out->gcount() != sizeof(password_size) && password_size < 0)
        {
            throw std::runtime_error("Unable to read the logger password size");
        }

        // Finally, read the password
        if(password_size > 0)
        {
            core::crypto::secure_string password_buffer;
            password_buffer.resize(static_cast<std::size_t>(password_size));

            m_remote_out->read(
                password_buffer.data(),
                password_size
            );

            if(m_remote_out->gcount() != password_size)
            {
                throw std::runtime_error("Unable to read the logger password");
            }

            // If we asked the password, store it for later use
            if(_ask_password)
            {
                core::crypto::password_keeper::set_global_password(password_buffer, false);
            }
        }

        // Setup boost log sink
        m_remote_sink = boost::log::add_console_log(
            *m_remote_in,
            keywords::format = (file_stream_format()),
            keywords::filter = expressions::attr<trivial::severity_level>("Severity")
                               >= static_cast<trivial::severity_level>(_level),
            keywords::auto_flush = true
        );

        // Relax the pressure to be on the safe side on windows
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //------------------------------------------------------------------------------

    inline void stop_logger()
    {
        // Stop the logger
        remove_sink();

        // Reset everything to pristine state
        m_remote_pid        = 0;
        m_log_archive_index = 0;
        m_original_log_archive.clear();
        m_log_archives.clear();
        m_level = sl_trace;
        m_raw   = true;
    }

    // Pointer to the public interface
    spy_logger* const m_logger;

    int m_remote_pid {0};

    std::unique_ptr<boost::process::child> m_remote_process;

    // Boost sink for the remote logger
    decltype(boost::log::add_console_log()) m_remote_sink;

    // Remote logger output stream, we use pointer so we can delete them at will, which is required (on Linux...)
    // for the child process to finish
    std::unique_ptr<boost::process::opstream> m_remote_in;

    // Remote logger input stream, we use pointer so we can delete them at will, which is required (on Linux...)
    // for the child process to finish
    std::unique_ptr<boost::process::ipstream> m_remote_out;

    // Log file index counter, used to generate the log file name in an encrypted log archive
    int m_log_archive_index {0};

    // The original asked log archive path
    std::filesystem::path m_original_log_archive;

    // The real log archive path
    std::vector<std::string> m_log_archives;

    // The asked level of logging
    level_t m_level {sl_trace};

    // False if encrypted
    bool m_raw {true};

    // Mutex to protect the logger
    std::recursive_mutex m_mutex;
};

//------------------------------------------------------------------------------

spy_logger& spy_logger::get()
{
    std::unique_lock lock(s_mutex);
    static spy_logger s_logger;
    return s_logger;
}

//------------------------------------------------------------------------------

std::filesystem::path spy_logger::get_logger_path()
{
    std::unique_lock lock(s_mutex);

    // Find the logger binary
    static const std::filesystem::path s_LOGGER_PATH =
        []
        {
            decltype(boost::this_process::path()) bin_paths;
            bin_paths.push_back(boost::dll::program_location().remove_filename());

            // Also append all known location from runtime
            const auto& paths = core::runtime::get_binaries_paths();

            std::transform(
                paths.cbegin(),
                paths.cend(),
                std::back_inserter(bin_paths),
                [](const auto& _path){return _path.string();});

            std::filesystem::path result = boost::process::search_path("sightlog", bin_paths).string();

            // Only do something if the file logger is started
            SIGHT_THROW_IF(
                "Cannot find the logger.",
                result.empty() || !std::filesystem::exists(result)
                || !std::filesystem::is_regular_file(result)
            );

            return result;
        }();

    return s_LOGGER_PATH;
}

spy_logger::spy_logger() :
    m_pimpl(std::make_unique<spy_logger_impl>(this))
{
    // Just in case..
    std::unique_lock lock(s_mutex);

    boost::log::add_common_attributes();
    boost::log::core::get()->add_global_attribute("Uptime", attributes::timer());
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
spy_logger::~spy_logger() = default;

//-----------------------------------------------------------------------------

void spy_logger::add_console_log(std::ostream& _os, level_t _level)
{
    // Just in case..
    std::unique_lock lock(s_mutex);

    boost::log::add_console_log(
        _os,
        keywords::format = (console_stream_format()),
        keywords::filter = expressions::attr<trivial::severity_level>("Severity")
                           >= static_cast<trivial::severity_level>(_level),
        keywords::auto_flush = true
    );
}

//-----------------------------------------------------------------------------

void spy_logger::add_file_log(const std::filesystem::path& _log_file, level_t _level)
{
    // Just in case..
    std::unique_lock lock(s_mutex);

    boost::log::add_file_log(
        // file name pattern
        keywords::file_name = _log_file.string(),
        // rotate files every 10 MiB...
        keywords::rotation_size = 10 * 1024 * 1024,
        // ...or at midnight
        keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        // log record format
        keywords::format = (file_stream_format()),
        keywords::filter = expressions::attr<trivial::severity_level>("Severity")
                           >= static_cast<trivial::severity_level>(_level),
        keywords::auto_flush = true
    );
}

//-----------------------------------------------------------------------------

void spy_logger::start_encrypted_logger(
    const std::filesystem::path& _log_archive,
    level_t _level,
    const core::crypto::secure_string& _password,
    bool _ask_password
)
{
    std::unique_lock lock(m_pimpl->m_mutex);
    m_pimpl->start_logger(_log_archive, _level, _password, _ask_password);
}

//-----------------------------------------------------------------------------

void spy_logger::start_logger(
    const std::filesystem::path& _log_archive,
    level_t _level
)
{
    std::unique_lock lock(m_pimpl->m_mutex);
    m_pimpl->start_logger(_log_archive, _level);
}

//------------------------------------------------------------------------------

std::filesystem::path spy_logger::get_current_log_path() const
{
    std::unique_lock lock(m_pimpl->m_mutex);
    return m_pimpl->m_log_archives.empty() ? std::string() : m_pimpl->m_log_archives.back();
}

//------------------------------------------------------------------------------

void spy_logger::stop_logger()
{
    std::unique_lock lock(m_pimpl->m_mutex);
    m_pimpl->stop_logger();
}

//------------------------------------------------------------------------------

bool spy_logger::is_log_encrypted() const
{
    std::unique_lock lock(m_pimpl->m_mutex);
    return m_pimpl->m_remote_sink != nullptr && !m_pimpl->m_raw;
}

//------------------------------------------------------------------------------

void spy_logger::change_log_password(
    const core::crypto::secure_string& _password,
    const core::crypto::secure_string& _old_password
)
{
    relocate_log(m_pimpl->m_original_log_archive, _password, true, _old_password);
}

//------------------------------------------------------------------------------

void spy_logger::relocate_log(
    const std::filesystem::path& _new_path,
    const core::crypto::secure_string& _password,
    bool _relocate_previous_logs,
    const core::crypto::secure_string& _old_password
)
{
    std::unique_lock lock(m_pimpl->m_mutex);

    // Create the new directory if needed
    const auto& new_dir = _new_path.has_parent_path()
                          ? _new_path.parent_path()
                          : _new_path.has_root_path()
                          ? _new_path.root_path()
                          : std::filesystem::current_path();

    std::filesystem::create_directories(new_dir);

    // Reserve a path for the merge of previous log
    const auto& merged_logs_path =
        [&]
        {
            if(_relocate_previous_logs && !m_pimpl->m_log_archives.empty())
            {
                return m_pimpl->next_log_archive(_new_path);
            }

            return std::filesystem::path();
        }();

    // Restart the logger. If it was encrypted , it will be encrypted again
    m_pimpl->start_logger(
        _new_path,
        m_pimpl->m_level,
        _password,
        false
    );

    // Save last element and remove it from the list so we won't merge it
    const auto current_log = m_pimpl->m_log_archives.back();
    m_pimpl->m_log_archives.pop_back();

    m_pimpl->m_log_archives.erase(
        std::remove_if(
            m_pimpl->m_log_archives.begin(),
            m_pimpl->m_log_archives.end(),
            [&](const auto& _path)
        {
            const auto& canonical_path = std::filesystem::weakly_canonical(_path);
            return canonical_path == std::filesystem::weakly_canonical(merged_logs_path)
            || canonical_path == std::filesystem::weakly_canonical(current_log);
        }),
        m_pimpl->m_log_archives.end()
    );

    // Merge the previous logs and relocate them
    if(!merged_logs_path.empty() && !m_pimpl->m_log_archives.empty())
    {
        // Find the logger binary
        const auto& logger_path = core::log::spy_logger::get_logger_path();

        // Wait for the current log to be created to avoid some kind of race conditions
        // Wait for max 10 seconds
        std::size_t tries = 0;
        while(tries++ < 100 && !std::filesystem::exists(current_log))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if(tries >= 100)
        {
            SIGHT_THROW("Unable to find the current log file '" << current_log << "' after 10 seconds.");
        }

        boost::process::ipstream remote_err;

        const auto& args = _password.empty()
                           ? boost::process::args(
            {
                "-b",
                "-i",
                core::crypto::to_base64(boost::join(m_pimpl->m_log_archives, ";")),
                "-o",
                core::crypto::to_base64(merged_logs_path.string())
            })
                           : _old_password.empty()
                           ? boost::process::args(
            {
                "-b",
                "-i",
                core::crypto::to_base64(boost::join(m_pimpl->m_log_archives, ";")),
                "-p",
                core::crypto::to_base64(_password).c_str(), // NOLINT(readability-redundant-string-cstr)
                "-o",
                core::crypto::to_base64(merged_logs_path.string())
            })
                           : boost::process::args(
            {
                "-b",
                "-i",
                core::crypto::to_base64(boost::join(m_pimpl->m_log_archives, ";")),
                "-p",
                core::crypto::to_base64(_password).c_str(), // NOLINT(readability-redundant-string-cstr)
                "-P",
                core::crypto::to_base64(_old_password).c_str(), // NOLINT(readability-redundant-string-cstr)
                "-o",
                core::crypto::to_base64(merged_logs_path.string())
            });

        // Start the logger in "convert" mode
        auto child = boost::process::child(
            logger_path.string(),
            args,
            boost::process::std_out > boost::process::null,
            boost::process::std_err > remote_err,
            boost::process::std_in < boost::process::null
        );

        // Wait for the logger to finish
        child.join();

        // Check the exit code
        if(child.exit_code() == 0)
        {
            // If the logs has been successfully merged, delete it
            for(const auto& old_archive : m_pimpl->m_log_archives)
            {
                std::filesystem::remove(old_archive);
            }
        }
        else
        {
            SIGHT_ERROR("Sightlog error [" << child.exit_code() << "]: " << remote_err.rdbuf());
        }

        // Keep the merged and current log
        m_pimpl->m_log_archives = {merged_logs_path.string(), current_log};
    }
    else
    {
        // Only keep the current log
        m_pimpl->m_log_archives = {current_log};
    }
}

//-----------------------------------------------------------------------------

#define FILE_LINE(msg) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    "[" << (file != nullptr ? file : __FILE__) \
    << ":" << (line >= 0 ? line : __LINE__) << "] " \
    << (msg)

//------------------------------------------------------------------------------

void spy_logger::trace(const std::string& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::trace) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::debug(const std::string& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::debug)
    << "[" << file << ":" << line << "] " << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::info(const std::string& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::info) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::warn(const std::string& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::warning) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::error(const std::string& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::error) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::fatal(const std::string& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::fatal) << FILE_LINE(_mes);
}

} // namespace sight::core::log

// NOLINTEND(clang-analyzer-optin.cplusplus.VirtualCall)
