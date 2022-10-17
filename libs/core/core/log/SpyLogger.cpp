/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

// cspell:ignore NOLINT

#ifdef _MSC_VER
#pragma warning(disable : 4996) // warning for sprintf() in Boost.log
#endif // _MSC_VER

#include "core/log/SpyLogger.hpp"

#include "core/crypto/Base64.hpp"
#include "core/crypto/PasswordKeeper.hpp"
#include "core/macros.hpp"
#include "core/spyLog.hpp"

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

namespace sight::core::log
{

namespace trivial     = boost::log::trivial;
namespace keywords    = boost::log::keywords;
namespace expressions = boost::log::expressions;
namespace attributes  = boost::log::attributes;

using boost::posix_time::ptime;

BOOST_LOG_GLOBAL_LOGGER(lg, boost::log::sources::severity_logger_mt<trivial::severity_level>);
BOOST_LOG_GLOBAL_LOGGER_DEFAULT(lg, boost::log::sources::severity_logger_mt<trivial::severity_level>);

//-----------------------------------------------------------------------------

inline static auto consoleStreamFormat()
{
    return expressions::stream << "["
           << expressions::attr<unsigned int>("LineID") << "]["
           << expressions::format_date_time<ptime::time_duration_type>("Uptime", "%H:%M:%S.%f") << "]["
           << expressions::attr<trivial::severity_level>("Severity") << "] "
           << expressions::smessage;
}

//------------------------------------------------------------------------------

inline static auto fileStreamFormat()
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

class SpyLogger::SpyLoggerImpl
{
public:

    // Delete default constructors and assignment operators
    SpyLoggerImpl(const SpyLoggerImpl&)            = delete;
    SpyLoggerImpl(SpyLoggerImpl&&)                 = delete;
    SpyLoggerImpl& operator=(const SpyLoggerImpl&) = delete;
    SpyLoggerImpl& operator=(SpyLoggerImpl&&)      = delete;

    // Constructor
    inline explicit SpyLoggerImpl(SpyLogger* const logger) noexcept :
        m_logger(logger)
    {
    }

    // Destructor
    inline ~SpyLoggerImpl()
    {
        removeSink();
    }

    // Returns the next log file name
    inline std::filesystem::path nextLogArchive(const std::filesystem::path& log_archive)
    {
        // Get the original log archive name and the current index
        if(m_original_log_archive.empty() && log_archive.empty())
        {
            m_original_log_archive = ENCRYPTED_LOG_FILE;
            m_log_archive_index    = 0;
        }
        else if(!log_archive.empty() && (m_original_log_archive.empty() || log_archive != m_original_log_archive))
        {
            m_original_log_archive = log_archive;
            m_log_archive_index    = 0;
        }
        else
        {
            m_log_archive_index++;
        }

        // Build the next log archive name
        m_current_log_archive = m_original_log_archive;

        m_current_log_archive.replace_filename(
            m_original_log_archive.stem().string()
            + "."
            + std::to_string(m_log_archive_index)
            + m_original_log_archive.extension().string()
        );

        std::cerr << "Log archive: " << m_current_log_archive.string() << std::endl;

        return m_current_log_archive;
    }

    //------------------------------------------------------------------------------

    inline void removeSink()
    {
        if(m_remote_sink)
        {
            boost::log::core::get()->flush();
            boost::log::core::get()->remove_sink(m_remote_sink);
            m_remote_sink.reset();

            if(m_remote_in)
            {
                m_remote_in->close();
                m_remote_in.reset();
            }

            if(m_remote_out)
            {
                m_remote_out->close();
                m_remote_out.reset();
            }

            m_remote_pid = 0;
        }
    }

    // Launch the remote logger, either in raw mode or in encrypted mode
    inline void startLogger(
        const std::filesystem::path& log_archive,
        LevelType level,
        const core::crypto::secure_string& password = core::crypto::secure_string(),
        bool ask_password                           = false
)
    {
        // If the logger is started, stop it by closing the input stream
        removeSink();

        // Find the logger binary
        static const auto logger_path =
            []
            {
                decltype(boost::this_process::path()) bin_paths;
                bin_paths.push_back(boost::dll::program_location().remove_filename());
                return boost::process::search_path("sightlog", bin_paths);
            }();

        // Build the log archive name
        const auto& new_log_archive = nextLogArchive(log_archive);

        // To avoid the logger to restart after changing the global password
        m_raw = (password.empty() && !ask_password);

        // Store the level, so we can launch the logger later when changing the password
        m_level = level;

        // Create the input and output streams
        m_remote_in  = std::make_unique<boost::process::opstream>();
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
            if(!password.empty())
            {
                args.emplace_back("-p");
                args.emplace_back(core::crypto::to_base64(password).c_str());
            }

            if(ask_password)
            {
                args.emplace_back("-a");
            }
        }

        // Start the logger process
        boost::process::spawn(
            logger_path,
            boost::process::args = args,
            boost::process::std_err > boost::process::null,
            boost::process::std_out > *m_remote_out,
            boost::process::std_in < *m_remote_in
        );

        // Wait for the logger to start
        if(!m_remote_out->good())
        {
            throw std::runtime_error("Unable to open the logger standard output stream");
        }

        // Read the PID of the logger
        m_remote_out->read(
            reinterpret_cast<char*>(&m_remote_pid),
            sizeof(m_pimpl->m_remote_pid)
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
            if(ask_password)
            {
                core::crypto::PasswordKeeper::set_global_password(password_buffer, false);
            }
        }

        // Setup boost log sink
        m_remote_sink = boost::log::add_console_log(
            *m_remote_in,
            keywords::format = (fileStreamFormat()),
            keywords::filter = expressions::attr<trivial::severity_level>("Severity")
                               >= static_cast<trivial::severity_level>(level),
            keywords::auto_flush = true
        );
    }

    // Pointer to the public interface
    SpyLogger* const m_logger;

    int m_remote_pid {0};

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
    std::filesystem::path m_current_log_archive;

    // The asked level of logging
    LevelType m_level {SL_TRACE};

    // True if encrypted
    bool m_raw {true};
};

SpyLogger::SpyLogger() :
    m_pimpl(std::make_unique<SpyLoggerImpl>(this))
{
    boost::log::add_common_attributes();
    boost::log::core::get()->add_global_attribute("Uptime", attributes::timer());
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
SpyLogger::~SpyLogger() = default;

//-----------------------------------------------------------------------------

void SpyLogger::add_console_log(std::ostream& os, LevelType level)
{
    boost::log::add_console_log(
        os,
        keywords::format = (consoleStreamFormat()),
        keywords::filter = expressions::attr<trivial::severity_level>("Severity")
                           >= static_cast<trivial::severity_level>(level),
        keywords::auto_flush = true
    );
}

//-----------------------------------------------------------------------------

void SpyLogger::add_file_log(const std::filesystem::path& log_file, LevelType level)
{
    boost::log::add_file_log(
        // file name pattern
        keywords::file_name = log_file.string(),
        // rotate files every 10 MiB...
        keywords::rotation_size = 10 * 1024 * 1024,
        // ...or at midnight
        keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        // log record format
        keywords::format = (fileStreamFormat()),
        keywords::filter = expressions::attr<trivial::severity_level>("Severity")
                           >= static_cast<trivial::severity_level>(level),
        keywords::auto_flush = true
    );
}

//-----------------------------------------------------------------------------

void SpyLogger::start_encrypted_logger(
    const std::filesystem::path& log_archive,
    LevelType level,
    const core::crypto::secure_string& password,
    bool ask_password
)
{
    m_pimpl->startLogger(log_archive, level, password, ask_password);
}

//-----------------------------------------------------------------------------

void SpyLogger::start_logger(
    const std::filesystem::path& log_archive,
    LevelType level
)
{
    m_pimpl->startLogger(log_archive, level); // NOLINT(clang-analyzer-optin.cplusplus.VirtualCall)
}

//------------------------------------------------------------------------------

std::filesystem::path SpyLogger::get_current_log_path() const
{
    return m_pimpl->m_current_log_archive;
}

//------------------------------------------------------------------------------

void SpyLogger::stop_logger()
{
    m_pimpl = std::make_unique<SpyLoggerImpl>(this);
}

//------------------------------------------------------------------------------

bool SpyLogger::is_log_encrypted() const
{
    return m_pimpl->m_remote_sink != nullptr && !m_pimpl->m_raw;
}

//------------------------------------------------------------------------------

void SpyLogger::change_log_password(const core::crypto::secure_string& password)
{
    if(is_log_encrypted())
    {
        start_encrypted_logger(
            m_pimpl->m_original_log_archive,
            m_pimpl->m_level,
            password
        );
    }
}

//-----------------------------------------------------------------------------

#define FILE_LINE(msg) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    "[" << (file != nullptr ? file : __FILE__) \
    << ":" << (line >= 0 ? line : __LINE__) << "] " \
    << (msg)

//------------------------------------------------------------------------------

void SpyLogger::trace(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::trace) << FILE_LINE(mes);
}

//-----------------------------------------------------------------------------

void SpyLogger::debug(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::debug)
    << "[" << file << ":" << line << "] " << FILE_LINE(mes);
}

//-----------------------------------------------------------------------------

void SpyLogger::info(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::info) << FILE_LINE(mes);
}

//-----------------------------------------------------------------------------

void SpyLogger::warn(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::warning) << FILE_LINE(mes);
}

//-----------------------------------------------------------------------------

void SpyLogger::error(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::error) << FILE_LINE(mes);
}

//-----------------------------------------------------------------------------

void SpyLogger::fatal(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), trivial::fatal) << FILE_LINE(mes);
}

} // namespace sight::core::log
