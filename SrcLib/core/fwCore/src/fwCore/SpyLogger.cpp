/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCore/macros.hpp"
#include "fwCore/SpyLogger.hpp"

#include <boost/log/trivial.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/utility/init/formatter_parser.hpp>

namespace spyLog
{

//-----------------------------------------------------------------------------

SpyLogger::SpyLogger()
{
    m_logCore = ::boost::log::core::get();
    m_logCore->set_filter
    (
            ::boost::log::filters::attr< ::boost::log::trivial::severity_level >("Severity") >= ::boost::log::trivial::trace
    );
}

//-----------------------------------------------------------------------------

SpyLogger::SpyLogger(const std::string & name)
{
    m_loggerName = name;
    m_logCore = ::boost::log::core::get();
    m_logCore->set_filter
    (
            ::boost::log::filters::attr< ::boost::log::trivial::severity_level >("Severity") >= ::boost::log::trivial::trace
    );
}

//-----------------------------------------------------------------------------

SpyLogger::SpyLogger (const SpyLogger & logger)
{
    m_loggerName = logger.m_loggerName;
}

//-----------------------------------------------------------------------------

SpyLogger::~SpyLogger()
{
}

//-----------------------------------------------------------------------------

void SpyLogger::createBasicConfiguration()
{
#ifdef _WIN32
    this->addFileAppender(m_loggerName + ".log");
#elif __MACOSX__
    this->addConsoleAppender();
#else // linux
    this->addConsoleAppender();
#endif
}

//-----------------------------------------------------------------------------

void SpyLogger::addConsoleAppender()
{
    ::boost::log::add_common_attributes();
    ::boost::log::init_log_to_console
     (
        std::clog,
        ::boost::log::keywords::format = "[%ProcessID%][%ThreadID%][%TimeStamp%][%Channel%][%Severity%]: %_%",
        // auto-flush feature of the backend
        ::boost::log::keywords::auto_flush = true
     );
}

//-----------------------------------------------------------------------------

void SpyLogger::addSyslogAppender(const std::string & hostName, const std::string & facilityName)
{
}

//-----------------------------------------------------------------------------

void SpyLogger::addFileAppender(const std::string & logFile)
{
    ::boost::log::add_common_attributes();
    ::boost::log::init_log_to_file
    (
         // file name pattern
        ::boost::log::keywords::file_name = logFile,
         // rotate files every 10 MiB...
        ::boost::log::keywords::rotation_size = 10 * 1024 * 1024,
        // ...or at midnight
        ::boost::log::keywords::time_based_rotation = ::boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        // log record format
        ::boost::log::keywords::format = "[%LineID%][%ProcessID%][%ThreadID%][%TimeStamp%][%Scope%][%Severity%]: %_%",
         // auto-flush feature of the backend
         ::boost::log::keywords::auto_flush = true
    );
}

//-----------------------------------------------------------------------------

void SpyLogger::setLevel(LevelType level)
{
    m_logCore->set_filter
    (
        ::boost::log::filters::attr< ::boost::log::trivial::severity_level >("Severity") >= level
    );
}

//-----------------------------------------------------------------------------

void SpyLogger::trace(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_TRIVIAL(trace) << "TRACE "<< file << " l" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::debug(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_TRIVIAL(debug) << "DEBUG "<< file << " l" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::info(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_TRIVIAL(info) << "INFO "<< file << " l" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::warn(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_TRIVIAL(warning) << "WARNING "<< file << " l" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::error(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_TRIVIAL(error) << "ERROR "<< file << " l" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::fatal(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_TRIVIAL(fatal) << "FATAL "<< file << " l" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

} // namespace spyLog

