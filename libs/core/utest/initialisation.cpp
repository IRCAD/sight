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

# include <core/log/SpyLogger.hpp>

#include <cstdio>
#include <filesystem>
#include <ostream>

namespace sight::utest
{

struct LogInitialization
{
    LogInitialization ()
    {
        core::log::SpyLogger& logger = core::log::SpyLogger::getSpyLogger();

        std::string logFile = "fwTest.log";

        FILE* pFile = fopen(logFile.c_str(), "w");
        if (pFile == NULL)
        {
            std::error_code err;
            std::filesystem::path sysTmp = std::filesystem::temp_directory_path(err);
            if(err.value() != 0)
            {
                // replace log file appender by stream appender: default dir and temp dir unreachable
                logger.addStreamAppender();
            }
            else
            {
                // creates SLM.log in temp directory: default dir unreachable
                sysTmp  = sysTmp / logFile;
                logFile = sysTmp.string();
                logger.addFileAppender(logFile);
            }
        }
        else
        {
            // creates SLM.log in default logFile directory
            fclose(pFile);
            logger.addFileAppender(logFile);
        }
    }

};

static LogInitialization init;

} // namespace sight::utest
