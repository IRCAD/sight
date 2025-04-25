/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "core/log/spy_logger.hpp"

#include "core/log/detail/spy_logger_impl.hxx"
#include "core/spy_log.hpp"

#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace sight::core::log
{

namespace keywords = boost::log::keywords;
namespace trivial  = boost::log::trivial;

// Static instance of the logger
spy_logger spy_logger::s_instance   = spy_logger();
SIGHT_CORE_API spy_logger& g_logger = spy_logger::s_instance;

spy_logger::spy_logger() :
    m_pimpl(std::make_unique<detail::spy_logger_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
spy_logger::~spy_logger() = default;

//------------------------------------------------------------------------------

std::shared_ptr<spy_logger> spy_logger::make(
    const std::filesystem::path& _log_path,
    level_t _level,
    const optional_password_t& _password,
    bool _asynchronous
)
{
    SIGHT_THROW_IF("The log file path cannot be empty.", _log_path.empty());

    struct private_spy_logger final : public spy_logger
    {
        explicit private_spy_logger(
            const std::filesystem::path& _log_path,
            level_t _level,
            const optional_password_t& _password,
            bool _asynchronous
        ) :
            m_boost_logger(keywords::channel = _log_path.string())
        {
            // Replace the global logger by our own
            m_pimpl->m_boost_logger = m_boost_logger;

            // Start the logger. It will use _log_path as channel, separating from global logs
            m_pimpl->start(_log_path, _level, _password, _asynchronous, _log_path.string());
        }

        ~private_spy_logger() override
        {
            m_pimpl->stop();
        }

        // The boost logger, with our channel that will be used by global sinks
        detail::boost_logger_t m_boost_logger;
    };

    return std::make_shared<private_spy_logger>(_log_path, _level, _password, _asynchronous);
}

//------------------------------------------------------------------------------

std::ostream& spy_logger::extract(
    std::istream& _input,
    std::ostream& _output,
    const optional_password_t& _password
)
{
    return detail::spy_logger_impl::extract(_input, _output, _password);
}

//-----------------------------------------------------------------------------

boost::shared_ptr<boost::log::sinks::sink> spy_logger::add_global_console_log(std::ostream& _os, level_t _level)
{
    return boost::log::add_console_log(
        _os,
        keywords::format     = (detail::console_stream_format()),
        keywords::filter     = detail::severity >= static_cast<trivial::severity_level>(_level),
        keywords::auto_flush = true
    );
}

//-----------------------------------------------------------------------------

boost::shared_ptr<boost::log::sinks::sink> spy_logger::add_global_file_log(
    const std::filesystem::path& _log_file,
    level_t _level
)
{
    return boost::log::add_file_log(
        // file name pattern
        keywords::file_name = _log_file.string(),
        // rotate files every 10 MiB...
        keywords::rotation_size = 10 * 1024 * 1024,
        // ...or at midnight
        keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        // log record format
        keywords::format     = (detail::file_stream_format()),
        keywords::filter     = detail::severity >= static_cast<trivial::severity_level>(_level),
        keywords::auto_flush = true
    );
}

//-----------------------------------------------------------------------------

void spy_logger::start(
    const std::filesystem::path& _log_path,
    level_t _level,
    const optional_password_t& _password,
    bool _asynchronous
)
{
    std::unique_lock lock(m_pimpl->m_mutex);
    m_pimpl->start(_log_path, _level, _password, _asynchronous);
}

//------------------------------------------------------------------------------

std::filesystem::path spy_logger::get_current_log_path() const
{
    std::unique_lock lock(m_pimpl->m_mutex);
    return m_pimpl->m_log_paths.empty() ? std::filesystem::path() : m_pimpl->m_log_paths.back();
}

//------------------------------------------------------------------------------

void spy_logger::stop()
{
    std::unique_lock lock(m_pimpl->m_mutex);
    m_pimpl->stop();
}

//------------------------------------------------------------------------------

bool spy_logger::is_log_encrypted() const
{
    std::unique_lock lock(m_pimpl->m_mutex);
    return m_pimpl->m_encrypted;
}

//------------------------------------------------------------------------------

std::filesystem::path spy_logger::change_password(
    const core::crypto::secure_string& _password,
    const optional_password_t& _old_password
)
{
    return relocate(m_pimpl->m_original_log_path, _password, _old_password);
}

//------------------------------------------------------------------------------

std::filesystem::path spy_logger::relocate(
    const std::filesystem::path& _new_path,
    const optional_password_t& _password,
    const optional_password_t& _old_password
)
{
    std::unique_lock lock(m_pimpl->m_mutex);

    return m_pimpl->relocate(_new_path, _password, _old_password);
}

//-----------------------------------------------------------------------------

#define FILE_LINE(msg) /* NOLINT(cppcoreguidelines-macro-usage) */ \
        "[" << (file != nullptr ? file : __FILE__) \
        << ":" << (line >= 0 ? line : __LINE__) << "] " \
        << (msg)

//------------------------------------------------------------------------------

void spy_logger::trace(const std::string_view& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(m_pimpl->m_boost_logger, trivial::trace) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::debug(const std::string_view& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(m_pimpl->m_boost_logger, trivial::debug) << "[" << file << ":" << line << "] " << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::info(const std::string_view& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(m_pimpl->m_boost_logger, trivial::info) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::warn(const std::string_view& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(m_pimpl->m_boost_logger, trivial::warning) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::error(const std::string_view& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(m_pimpl->m_boost_logger, trivial::error) << FILE_LINE(_mes);
}

//-----------------------------------------------------------------------------

void spy_logger::fatal(const std::string_view& _mes, const char* file, int line)
{
    BOOST_LOG_SEV(m_pimpl->m_boost_logger, trivial::fatal) << FILE_LINE(_mes);
}

} // namespace sight::core::log
