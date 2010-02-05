/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCORE_SPYLOGGERMANAGER_HPP_
#define _FWCORE_SPYLOGGERMANAGER_HPP_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "fwCore/BaseObject.hpp"
#include "fwCore/config.hpp"
#include "fwCore/SpyLogger.hpp"


namespace spyLog
{

/**
 * @brief   Implements the SpyLoggerManager.
 * 
 * @author IRCAD (Research and Development Team).
 */
class SpyLoggerManager : public ::fwCore::BaseObject
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (SpyLoggerManager)(BaseObject::Baseclass), (()), new SpyLoggerManager) ;

    FWCORE_API static SpyLoggerManager::sptr getSpyLoggerManager();

    FWCORE_API static void kill();

    // FWCORE_API static void initMainSpyLogger(const SpyLogger & logger);

    FWCORE_API SpyLogger & getMainSpyLogger();

    FWCORE_API SpyLogger & getSpyLogger(const std::string & loggerName);

    ~SpyLoggerManager();

private :

    SpyLoggerManager();

    static SpyLoggerManager::sptr m_manager;

    std::map< std::string, SpyLogger > m_loggerNameToLogger;

    SpyLogger m_mainSpyLogger;


}; // SpyLoggerManager


} // namespace spyLog

#endif // _FWCORE_SPYLOGGERMANAGER_HPP_

