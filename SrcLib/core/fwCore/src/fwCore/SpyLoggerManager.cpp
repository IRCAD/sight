/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include "fwCore/SpyLoggerManager.hpp"

namespace spyLog
{

//==============================================================================

SpyLogger & SpyLoggerManager::getMainSpyLogger()
{
        return m_mainSpyLogger;
}

//==============================================================================

SpyLogger & SpyLoggerManager::getSpyLogger(const std::string & loggerName)
{
        if( m_loggerNameToLogger.find(loggerName) == m_loggerNameToLogger.end() )
        {
                SpyLogger log (loggerName);
                log.createBasicConfiguration();
                m_loggerNameToLogger[loggerName] = log;
        }

        return m_loggerNameToLogger[loggerName];
}

//==============================================================================

SpyLoggerManager::sptr SpyLoggerManager::m_manager = SpyLoggerManager::sptr();

//==============================================================================

SpyLoggerManager::sptr SpyLoggerManager::getSpyLoggerManager()
{
        if (m_manager == NULL)
        {
                m_manager = SpyLoggerManager::New();
        }

        return m_manager;
}

//==============================================================================

void SpyLoggerManager::kill()
{
        // if (NULL != m_manager)
        // {
                // delete m_manager;
                // m_manager = NULL;
        // }
}

//==============================================================================

SpyLoggerManager::SpyLoggerManager() :
m_mainSpyLogger("SLM")
{
#ifdef _DEBUG
        std::cout << "SpyLoggerManager::SpyLoggerManager()" << std::endl;
#endif //_DEBUG
        m_mainSpyLogger.createBasicConfiguration();
}

//==============================================================================

SpyLoggerManager::~SpyLoggerManager()
{
#ifdef _DEBUG
        std::cout << "SpyLoggerManager::~SpyLoggerManager()" << std::endl;
#endif //_DEBUG
}

//==============================================================================

} // namespace spyLog
