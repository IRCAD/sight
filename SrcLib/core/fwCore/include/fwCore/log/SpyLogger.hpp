/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWCORE_LOG_SPYLOGGER_HPP__
#define __FWCORE_LOG_SPYLOGGER_HPP__

#include <iostream>
#include <string>


#include "fwCore/BaseObject.hpp"
#include "fwCore/config.hpp"

namespace fwCore
{
namespace log
{

/**
 * @brief   Implements the SpyLogger.
 *
 *
 */
class SpyLogger : public ::fwCore::BaseObject
{

public:

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

    FWCORE_API void addStreamAppender(std::ostream &os = std::clog, LevelType level = SL_TRACE);

    FWCORE_API void addFileAppender(const std::string & logFile = "SLM.log", LevelType level = SL_TRACE);

    // FWCORE_API void addSyslogAppender(const std::string & hostName, const std::string & facilityName);

    FWCORE_API void setLevel(LevelType level);

    FWCORE_API void trace(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void debug(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void info(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void warn(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void error(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void fatal(const std::string & mes, const char * file = 0, int line = -1);

    FWCORE_API void log(const std::string & mes, const char * file = 0, int line = -1);

    static SpyLogger &getSpyLogger()
    {
        return s_spyLogger;
    }

protected:


    FWCORE_API SpyLogger ();

    FWCORE_API static SpyLogger s_spyLogger;

}; // SpyLogger

} // namespace log
} // namespace fwCore

#endif // __FWCORE_LOG_SPYLOGGER_HPP__
