/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifdef _MSC_VER
#pragma warning(disable : 4996) // warning for sprintf() in Boost.log
#endif // _MSC_VER

#include <boost/log/attributes.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>


#include "fwCore/macros.hpp"
#include "fwCore/log/SpyLogger.hpp"

namespace fwCore
{
namespace log
{

SpyLogger SpyLogger::s_spyLogger;

BOOST_LOG_DECLARE_GLOBAL_LOGGER(lg, ::boost::log::sources::severity_logger_mt< ::boost::log::trivial::severity_level>);


typedef boost::log::attributes::current_thread_id::held_type ThreadIdType;
typedef boost::log::attributes::current_process_id::held_type ProcessIdType;


//-----------------------------------------------------------------------------

SpyLogger::SpyLogger()
{
    ::boost::log::add_common_attributes();
    ::boost::log::core::get()
        ->add_global_attribute("Uptime", ::boost::make_shared< ::boost::log::attributes::timer>());
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

void SpyLogger::addStreamAppender( std::ostream &os )
{
    namespace fmt = ::boost::log::formatters;
    namespace keywords = ::boost::log::keywords;

    ::boost::log::core::get()->add_sink (
        ::boost::log::init_log_to_console (
                    os,
                    keywords::format = (
                            fmt::stream
                            << fmt::attr("LineID")
                            << "][" << fmt::time_duration("Uptime", std::nothrow)
                            << "][" << fmt::attr< ::boost::log::trivial::severity_level >("Severity", std::nothrow)
                            << "] " << fmt::message()
                        ),
                    // auto-flush feature of the backend
                    keywords::auto_flush = true
        )
    );
}

//-----------------------------------------------------------------------------

// void SpyLogger::addSyslogAppender(const std::string & hostName, const std::string & facilityName)
// {
// }

//-----------------------------------------------------------------------------

void SpyLogger::addFileAppender(const std::string & logFile)
{
    namespace fmt = ::boost::log::formatters;
    namespace keywords = ::boost::log::keywords;

    ::boost::log::core::get()->add_sink(
        ::boost::log::init_log_to_file (
            // file name pattern
            keywords::file_name = logFile,
            // rotate files every 10 MiB...
            keywords::rotation_size = 10 * 1024 * 1024,
            // ...or at midnight
            keywords::time_based_rotation = ::boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
            // log record format
            keywords::format = (
                    fmt::stream
                    << "[" << fmt::date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
                    << "][" << fmt::time_duration("Uptime", std::nothrow)
                    << "][" << fmt::attr<ProcessIdType>("ProcessID", std::nothrow)
                    << "][" << fmt::attr<ThreadIdType> ("ThreadID", std::nothrow)
                    << "][" << fmt::attr< ::boost::log::trivial::severity_level >("Severity", std::nothrow)
                    << "] " << fmt::message()
                ),
            // auto-flush feature of the backend
            keywords::auto_flush = true
        )
    );
}

//-----------------------------------------------------------------------------

void SpyLogger::setLevel(LevelType level)
{
    ::boost::log::core::get()->set_filter
    (
        ::boost::log::filters::attr< ::boost::log::trivial::severity_level >("Severity")
                                            >= static_cast < ::boost::log::trivial::severity_level > (level)
    );
}

//-----------------------------------------------------------------------------

void SpyLogger::trace(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::trace) << file << ":" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::debug(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::debug) << file << ":" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::info(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::info) << file << ":" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::warn(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::warning) << file << ":" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::error(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::error) << file << ":" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::fatal(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::fatal) << file << ":" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

void SpyLogger::log(const std::string & mes, const char * file, int line)
{
    BOOST_LOG_SEV(lg::get(), ::boost::log::trivial::error) << file << ":" << line << ": "<< mes ;
}

//-----------------------------------------------------------------------------

} // namespace log
} // namespace fwCore

