/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#ifdef _MSC_VER
#pragma warning(disable : 4996) // warning for sprintf() in Boost.log
#endif // _MSC_VER

#include "fwCore/log/SpyLogger.hpp"

#include "fwCore/macros.hpp"

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

namespace fwCore
{
namespace log
{

SpyLogger SpyLogger::s_spyLogger;

BOOST_LOG_GLOBAL_LOGGER(lg, ::boost::log::sources::severity_logger_mt< ::boost::log::trivial::severity_level >);
BOOST_LOG_GLOBAL_LOGGER_DEFAULT(lg, ::boost::log::sources::severity_logger_mt< ::boost::log::trivial::severity_level >);

//-----------------------------------------------------------------------------

SpyLogger::SpyLogger()
{
    ::boost::log::add_common_attributes();
    ::boost::log::core::get()
    ->add_global_attribute("Uptime", ::boost::log::attributes::timer());
}

//-----------------------------------------------------------------------------

void SpyLogger::createBasicConfiguration()
{
#ifdef _WIN32
    this->addFileAppender();
#else
    this->addStreamAppender();
#endif
}

//-----------------------------------------------------------------------------

void SpyLogger::addStreamAppender( std::ostream& os, LevelType level )
{
    namespace expr     = ::boost::log::expressions;
    namespace keywords = ::boost::log::keywords;

    typedef ::boost::posix_time::ptime::time_duration_type DurationType;

    ::boost::log::add_console_log(
        os,
        keywords::format = (
            expr::stream << "["
                         << expr::attr<unsigned int>("LineID")
                         << "][" <<  expr::format_date_time< DurationType >("Uptime", "%H:%M:%S.%f")
                         << "][" << expr::attr< ::boost::log::trivial::severity_level >("Severity")
                         << "] " << expr::smessage
            ),
        keywords::filter = expr::attr< ::boost::log::trivial::severity_level >("Severity") >=
                           static_cast < ::boost::log::trivial::severity_level > (level),
        // auto-flush feature of the backend
        keywords::auto_flush = true
        );
}

//-----------------------------------------------------------------------------

// void SpyLogger::addSyslogAppender(const std::string & hostName, const std::string & facilityName)
// {
// }

//-----------------------------------------------------------------------------

void SpyLogger::addFileAppender(const std::string& logFile, LevelType level)
{
    namespace expr     = ::boost::log::expressions;
    namespace keywords = ::boost::log::keywords;

    typedef ::boost::posix_time::ptime::time_duration_type DurationType;

    ::boost::log::add_file_log(
        // file name pattern
        keywords::file_name = logFile,
        // rotate files every 10 MiB...
        keywords::rotation_size = 10 * 1024 * 1024,
        // ...or at midnight
        keywords::time_based_rotation = ::boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        // log record format
        keywords::format = (
            expr::stream
                << "[" << expr::format_date_time< ::boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
                << "][" << expr::format_date_time< DurationType >("Uptime", "%H:%M:%S.%f")
                << "][" << expr::attr< ::boost::log::attributes::current_process_id::value_type >("ProcessID")
                << "][" << expr::attr< ::boost::log::attributes::current_thread_id::value_type >("ThreadID")
                << "][" << expr::attr< ::boost::log::trivial::severity_level >("Severity")
                << "] " << expr::smessage
            ),
        keywords::filter = expr::attr< ::boost::log::trivial::severity_level >("Severity") >=
                           static_cast < ::boost::log::trivial::severity_level > (level),
        // auto-flush feature of the backend
        keywords::auto_flush = true
        );
}

//-----------------------------------------------------------------------------

void SpyLogger::setLevel(LevelType level)
{
    ::boost::log::core::get()->set_filter
    (
        ::boost::log::expressions::attr< ::boost::log::trivial::severity_level >("Severity")
        >= static_cast < ::boost::log::trivial::severity_level > (level)
    );
}

//-----------------------------------------------------------------------------

void SpyLogger::trace(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::trace) << file << ":" << line << ": "<< mes;
}

//-----------------------------------------------------------------------------

void SpyLogger::debug(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::debug) << file << ":" << line << ": "<< mes;
}

//-----------------------------------------------------------------------------

void SpyLogger::info(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::info) << file << ":" << line << ": "<< mes;
}

//-----------------------------------------------------------------------------

void SpyLogger::warn(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::warning) << file << ":" << line << ": "<< mes;
}

//-----------------------------------------------------------------------------

void SpyLogger::error(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::error) << file << ":" << line << ": "<< mes;
}

//-----------------------------------------------------------------------------

void SpyLogger::fatal(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::fatal) << file << ":" << line << ": "<< mes;
}

//-----------------------------------------------------------------------------

void SpyLogger::log(const std::string& mes, const char* file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::error) << file << ":" << line << ": "<< mes;
}

//-----------------------------------------------------------------------------

} // namespace log
} // namespace fwCore
