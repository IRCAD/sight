/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include "core/BaseObject.hpp"
#include "core/config.hpp"

#include <iostream>
#include <string>

namespace sight::core
{

namespace log
{

/**
 * @brief   Implements the SpyLogger.
 *
 *
 */
class SpyLogger : public core::BaseObject
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

    CORE_API void createBasicConfiguration();

    CORE_API void addStreamAppender(std::ostream& os = std::clog, LevelType level = SL_TRACE);

    CORE_API void addFileAppender(const std::string& logFile = "SLM.log", LevelType level = SL_TRACE);

    // CORE_API void addSyslogAppender(const std::string & hostName, const std::string & facilityName);

    CORE_API void setLevel(LevelType level);

    CORE_API void trace(const std::string& mes, const char* file = 0, int line = -1);

    CORE_API void debug(const std::string& mes, const char* file = 0, int line = -1);

    CORE_API void info(const std::string& mes, const char* file = 0, int line = -1);

    CORE_API void warn(const std::string& mes, const char* file = 0, int line = -1);

    CORE_API void error(const std::string& mes, const char* file = 0, int line = -1);

    CORE_API void fatal(const std::string& mes, const char* file = 0, int line = -1);

    CORE_API void log(const std::string& mes, const char* file = 0, int line = -1);

    //------------------------------------------------------------------------------

    static SpyLogger& getSpyLogger()
    {
        return s_spyLogger;
    }

protected:

    CORE_API SpyLogger();

    CORE_API static SpyLogger s_spyLogger;
}; // SpyLogger

} // namespace log

} // namespace sight::core
