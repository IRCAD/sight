/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/log/keywords/channel.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>

#include "fwAtomsPatch/infos/Logger.hpp"

namespace fwAtomsPatch
{
namespace infos
{

Logger::StreamPtrType Logger::s_stream = ::boost::make_shared< Logger::StreamType >();
Logger Logger::s_logger;

BOOST_LOG_GLOBAL_LOGGER(lg_channel,
                        ::boost::log::sources::channel_logger<std::string>);

BOOST_LOG_GLOBAL_LOGGER_CTOR_ARGS(lg_channel,
                                  ::boost::log::sources::channel_logger_mt<std::string>,
                                   (std::string("patch")));


Logger::Logger()
{
    namespace expr = ::boost::log::expressions;
    namespace keywords = ::boost::log::keywords;

    ::boost::log::add_file_log (
                // file name pattern
                keywords::file_name = "PATCH.log",
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
                keywords::filter = (expr::attr< std::string >("Channel") == "patch")
    );

    // Construct the sink
    typedef ::boost::log::sinks::synchronous_sink< ::boost::log::sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > pSink = boost::make_shared< text_sink >();

    // Add a stream to write log to
    pSink->locked_backend()->add_stream(s_stream);

    // Register the sink in the logging core
    ::boost::log::core::get()->add_sink(pSink);
}

// ----------------------------------------------------------------------------

Logger::~Logger()
{}

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

