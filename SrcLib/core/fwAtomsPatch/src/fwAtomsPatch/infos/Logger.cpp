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

#include "fwAtomsPatch/infos/Logger.hpp"

#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/Os.hpp>

#include <boost/filesystem.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/keywords/channel.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/parameter/keyword.hpp>

namespace fwAtomsPatch
{
namespace infos
{

BOOST_LOG_GLOBAL_LOGGER(lg_channel,
                        ::boost::log::sources::channel_logger<std::string>);

BOOST_LOG_GLOBAL_LOGGER_CTOR_ARGS(lg_channel,
                                  ::boost::log::sources::channel_logger_mt<std::string>,
                                  (std::string("patch")));

Logger::StreamPtrType Logger::s_stream = ::boost::make_shared< Logger::StreamType >();

Logger Logger::s_logger;

Logger::Logger()
{
    namespace expr     = ::boost::log::expressions;
    namespace keywords = ::boost::log::keywords;

    namespace bfile = ::boost::filesystem;

    //Create PATCH.log in a user data dir nammed sight/appName/
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    //default name of application with no profile.xml
    std::string appName = "default";

    if(profile)
    {
        appName = profile->getName();
    }

    const bfile::path appPrefDir = ::fwTools::os::getUserDataDir("sight", appName, true);

    FW_RAISE_IF("Unable to define User's data directory", appPrefDir.empty());

    if (!bfile::exists(appPrefDir))
    {
        bfile::create_directories(appPrefDir);
    }

    ::boost::log::add_file_log(
        // file name pattern
        keywords::file_name = appPrefDir.string() + "/PATCH.log",
        // rotate files every 10 MiB...
        keywords::rotation_size = 10 * 1024 * 1024,
        // ...or at midnight
        keywords::time_based_rotation = ::boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        // log record format
        keywords::format = (
            expr::stream
                << "[" << expr::format_date_time< ::boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
                << "][" << expr::format_date_time< ::boost::posix_time::ptime >("Uptime", "%H:%M:%S.%f")
                << "][" << expr::attr< std::string >("Channel")
                << "] " << expr::smessage
            ),
        // auto-flush feature of the backend
        keywords::auto_flush = true,
        keywords::filter     = (expr::attr< std::string >("Channel") == "patch")
        );

    // Construct the sink
    typedef ::boost::log::sinks::synchronous_sink< ::boost::log::sinks::text_ostream_backend > text_sink;
    ::boost::shared_ptr< text_sink > pSink = ::boost::make_shared< text_sink >();

    // Add a stream to write log to
    pSink->locked_backend()->add_stream(s_stream);

    // Register the sink in the logging core
    ::boost::log::core::get()->add_sink(pSink);
}

// ----------------------------------------------------------------------------

Logger::~Logger()
{
}

// ----------------------------------------------------------------------------

Logger::StreamPtrType Logger::getStream()
{
    return s_stream;
}

// ----------------------------------------------------------------------------

void Logger::error(const std::string& message)
{
    BOOST_LOG_STREAM(lg_channel::get()) << "PATCH_ERROR" << ": " << message;
}

// ----------------------------------------------------------------------------

void Logger::badCast(const std::string& message)
{
    BOOST_LOG_STREAM(lg_channel::get()) << "BADCAST" << ": " << message;
}

// ----------------------------------------------------------------------------

void Logger::outOfRange(const std::string& message)
{
    BOOST_LOG_STREAM(lg_channel::get()) << "OUTOFRANGE" << ": " << message;
}

// ----------------------------------------------------------------------------

void Logger::info(const std::string& message)
{
    BOOST_LOG_STREAM(lg_channel::get()) << "INFO" << ": " << message;
}

// ----------------------------------------------------------------------------

void Logger::addAttribute(const std::string& message)
{
    BOOST_LOG_STREAM(lg_channel::get()) << "ADD_ATTR" << ": " << message;
}

// ----------------------------------------------------------------------------

void Logger::eraseAttribute(const std::string& message)
{
    BOOST_LOG_STREAM(lg_channel::get()) << "ERASE_ATTR" << ": " << message;
}

// ----------------------------------------------------------------------------

void Logger::replaceAttribute(const std::string& message)
{
    BOOST_LOG_STREAM(lg_channel::get()) << "REPLACE_ATTR" << ": " << message;
}

// ----------------------------------------------------------------------------

} //infos
} //fwAtomHelper
