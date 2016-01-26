/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

# include <fwCore/log/SpyLogger.hpp>

#include <boost/filesystem.hpp>

#include <ostream>
#include <cstdio>

namespace fwTest
{


struct LogInitialization
{
    LogInitialization ()
    {
        fwCore::log::SpyLogger &logger = fwCore::log::SpyLogger::getSpyLogger();

        std::string logFile = "fwTest.log";

        FILE * pFile = fopen(logFile.c_str(), "w");
        if (pFile==NULL)
        {
            ::boost::system::error_code err;
            ::boost::filesystem::path sysTmp = ::boost::filesystem::temp_directory_path(err);
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

}  //namespace fwTest

