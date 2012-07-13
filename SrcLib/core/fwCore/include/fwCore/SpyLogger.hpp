/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCORE_SPYLOGGER_HPP_
#define _FWCORE_SPYLOGGER_HPP_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>

#include "fwCore/BaseObject.hpp"
#include "fwCore/config.hpp"

namespace spyLog
{

/**
 * @brief   Implements the SpyLogger.
 *
 * @author IRCAD (Research and Development Team).
 */
class SpyLogger : public ::fwCore::BaseObject
{

public :
    // fwCoreClassDefinitionsWithFactoryMacro( (SpyLogger)(BaseObject), (()), new SpyLogger) ;

    enum LevelType
    {
        SL_TRACE,
        SL_DEBUG,
        SL_INFO,
        SL_WARN,
        SL_ERROR,
        SL_FATAL
    };

    FWCORE_API void createBasicConfiguration();

    FWCORE_API void addConsoleAppender();

    FWCORE_API void addFileAppender(const std::string & logFile = "SLM.log");

    // FWCORE_API void addSyslogAppender(const std::string & hostName, const std::string & facilityName);

    FWCORE_API void setLevel(LevelType level);

    FWCORE_API void trace(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void debug(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void info(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void warn(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void error(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void fatal(const std::string & mes, const char * file = 0, int line = -1);

    static SpyLogger &getSpyLogger() {return s_spyLogger;}

protected :


    FWCORE_API SpyLogger ();

    FWCORE_API static SpyLogger s_spyLogger;

}; // SpyLogger

} // namespace spyLog

#endif // _FWCORE_SPYLOGGER_HPP_
