/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCore/SpyLogger.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef USE_LOG4CXX

#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/spi/location/locationinfo.h>
#include <log4cxx/helpers/transcoder.h>

#include <log4cxx/net/syslogappender.h>

#endif

namespace spyLog
{

//==============================================================================

SpyLogger::SpyLogger()
{
#ifdef _DEBUG
	std::cout << "SpyLogger::SpyLogger()" << std::endl;
#endif
	
#ifdef USE_LOG4CXX
	
	m_logger = log4cxx::Logger::getLogger("SL");
	m_logger->setLevel(log4cxx::Level::getTrace());
	
#endif
	
}

//==============================================================================

SpyLogger::SpyLogger(const std::string & name)
{
#ifdef _DEBUG
	std::cout << "SpyLogger::SpyLogger(name=" << name << ")" << std::endl;
#endif
	m_loggerName = name;
	
#ifdef USE_LOG4CXX
	
	m_logger = log4cxx::Logger::getLogger(name);
	m_logger->setLevel(log4cxx::Level::getTrace());

#endif
	
}

//==============================================================================

SpyLogger::SpyLogger (const SpyLogger & logger)
{

	m_loggerName = logger.m_loggerName;
	
#ifdef USE_LOG4CXX
		
	m_logger = logger.m_logger;

#endif
	
}

//==============================================================================

SpyLogger::~SpyLogger()
{
#ifdef _DEBUG
	std::cout << "SpyLogger::~SpyLogger()" << std::endl;
#endif
}

//==============================================================================

void SpyLogger::createBasicConfiguration()
{
	
#ifdef _WIN32

	addFileAppender(m_loggerName + ".log");

#elif __MACOSX__

	addConsoleAppender();

#else // linux

	addConsoleAppender();

#endif
	
}

//==============================================================================

void SpyLogger::addConsoleAppender()
{
	
#ifdef USE_LOG4CXX
	
	// Default conversion pattern
	log4cxx::LogString defaultConversionPattern (LOG4CXX_STR("[%c] %-5p (%F:%L) - %m%n"));

	// Create Appender
	log4cxx::PatternLayoutPtr layout = new log4cxx::PatternLayout();
	layout->setConversionPattern (defaultConversionPattern);
	log4cxx::ConsoleAppenderPtr appender = new log4cxx::ConsoleAppender(layout);

	// Add to the main SpyLogger
	m_logger->addAppender(appender);
	
#endif
	
}

//==============================================================================


void SpyLogger::addSyslogAppender(const std::string & hostName, const std::string & facilityName)
{

#ifdef USE_LOG4CXX
	
	// Default conversion pattern
	log4cxx::LogString defaultConversionPattern (LOG4CXX_STR("[%c] %-5p (%F:%L) - %m%n"));

	// Create layout
	log4cxx::PatternLayoutPtr layout = new log4cxx::PatternLayout();
	layout->setConversionPattern (defaultConversionPattern);

	// Create appender
	log4cxx::LogString host = log4cxx::helpers::Transcoder::decode(hostName.c_str());
	log4cxx::LogString facility = log4cxx::helpers::Transcoder::decode(facilityName.c_str());
 	log4cxx::net::SyslogAppenderPtr appender = new log4cxx::net::SyslogAppender(layout, host, log4cxx::net::SyslogAppender::getFacility(facility));

	// Add to the main SpyLogger
	m_logger->addAppender(appender);

#endif
	
}


//==============================================================================

void SpyLogger::addFileAppender(const std::string & logFile)
{

#ifdef USE_LOG4CXX
	
	// Default conversion pattern
	log4cxx::LogString defaultConversionPattern (LOG4CXX_STR("[%c] %-5p (%F:%L) - %m%n"));

	// Create Appender
	log4cxx::PatternLayoutPtr layout = new log4cxx::PatternLayout();
	layout->setConversionPattern (defaultConversionPattern);

	//const wchar_t * file = logFile.c_str();
	log4cxx::LogString logStringFile = log4cxx::helpers::Transcoder::decode(logFile.c_str());
	log4cxx::FileAppenderPtr appender = new log4cxx::FileAppender(layout,logStringFile,false);

	// Add to the main SpyLogger
	m_logger->addAppender(appender);

#endif
	
}

//==============================================================================

void SpyLogger::setLevel(LevelType level)
{

#ifdef USE_LOG4CXX
	
	switch (level)
	{
		case SpyLogger::SL_TRACE : m_logger->setLevel(log4cxx::Level::getTrace()); break;
		case SpyLogger::SL_DEBUG : m_logger->setLevel(log4cxx::Level::getDebug()); break;
		case SpyLogger::SL_INFO  : m_logger->setLevel(log4cxx::Level::getInfo() ); break;
		case SpyLogger::SL_WARN  : m_logger->setLevel(log4cxx::Level::getWarn() ); break;
		case SpyLogger::SL_ERROR : m_logger->setLevel(log4cxx::Level::getError()); break;
		case SpyLogger::SL_FATAL : m_logger->setLevel(log4cxx::Level::getFatal()); break;
		default : break;
	}

#endif
	
}



//==============================================================================

void SpyLogger::trace(const std::string & mes, const char * file, int line)
{
		
#ifdef USE_LOG4CXX
	
	if (m_logger->isTraceEnabled()) {
		::std::stringstream oss;
		oss << mes;
		m_logger->forcedLog(::log4cxx::Level::getTrace(), oss.str(), log4cxx::spi::LocationInfo(file, file, line));
	}
#else
	std::cout << "[TRACE] (" << file << ":" << line << ") : " << mes << std::endl;
#endif
	
}

//==============================================================================

void SpyLogger::debug(const std::string & mes, const char * file, int line)
{

#ifdef USE_LOG4CXX
	
	if (m_logger->isDebugEnabled()) {
		::std::stringstream oss;
		oss << mes;
		m_logger->forcedLog(::log4cxx::Level::getDebug(), oss.str(), log4cxx::spi::LocationInfo(file, file, line));
	}

#else
	std::cout << "[DEBUG] (" << file << ":" << line << ") : " << mes << std::endl;
#endif
	
}

//==============================================================================

void SpyLogger::info(const std::string & mes, const char * file, int line)
{

#ifdef USE_LOG4CXX

	if (m_logger->isInfoEnabled()) {
		::std::stringstream oss;
		oss << mes;
		m_logger->forcedLog(::log4cxx::Level::getInfo(), oss.str(), log4cxx::spi::LocationInfo(file, file, line));
	}

#else
	std::cout << "[INFO]  (" << file << ":" << line << ") : " << mes << std::endl;
#endif
	
}

//==============================================================================

void SpyLogger::warn(const std::string & mes, const char * file, int line)
{

#ifdef USE_LOG4CXX

	if (m_logger->isWarnEnabled()) {
		::std::stringstream oss;
		oss << mes;
		m_logger->forcedLog(::log4cxx::Level::getWarn(), oss.str(), log4cxx::spi::LocationInfo(file, file, line));
	}

#else
	std::cout << "[WARN]  (" << file << ":" << line << ") : " << mes << std::endl;
#endif
	
}

//==============================================================================

void SpyLogger::error(const std::string & mes, const char * file, int line)
{

#ifdef USE_LOG4CXX

	if (m_logger->isErrorEnabled()) {
		::std::stringstream oss;
		oss << mes;
		m_logger->forcedLog(::log4cxx::Level::getError(), oss.str(),log4cxx::spi::LocationInfo(file, file, line));
	}

#else
	std::cout << "[ERROR] (" << file << ":" << line << ") : " << mes << std::endl;
#endif
	
}

//==============================================================================

void SpyLogger::fatal(const std::string & mes, const char * file, int line)
{

#ifdef USE_LOG4CXX

	if (m_logger->isFatalEnabled()) {
		::std::stringstream oss;
		oss << mes;
		m_logger->forcedLog(::log4cxx::Level::getFatal(), oss.str(), log4cxx::spi::LocationInfo(file, file, line));
	}

#else
	std::cout << "[FATAL] (" << file << ":" << line << ") : " << mes << std::endl;
#endif
	
}

//==============================================================================

} // namespace spyLog

