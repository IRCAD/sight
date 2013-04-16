/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLMEMORY_MANAGER_LOCKDUMPMANAGERSRV_HPP__
#define __CTRLMEMORY_MANAGER_LOCKDUMPMANAGERSRV_HPP__

#include <map>
#include <vector>
#include <string>

#include <ctrlSelection/IManagerSrv.hpp>

#include "ctrlMemory/config.hpp"

namespace fwData
{
    class ObjectLock;
    class Composite;
}

namespace fwServices
{
    class ObjectMsg;
}

namespace ctrlMemory
{

namespace manager
{

/**
 * @class  LockDumpManagerSrv.
 * @date   2012-2013.
 */
class CTRLMEMORY_CLASS_API LockDumpManagerSrv : public ::ctrlSelection::IManagerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (LockDumpManagerSrv)(::ctrlSelection::IManagerSrv) ) ;

    /// Constructor.  Does nothing.
    CTRLMEMORY_API LockDumpManagerSrv() throw() ;

    /// Destructor. Does nothing.
    CTRLMEMORY_API virtual ~LockDumpManagerSrv() throw() ;

protected :

    /// Dump lock composite objects
    CTRLMEMORY_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Dump unlock composite objects
    CTRLMEMORY_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing
    CTRLMEMORY_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Does nothing
    CTRLMEMORY_API virtual void updating() throw ( ::fwTools::Failed );

    /// Does nothing
    CTRLMEMORY_API virtual void info( std::ostream &_sstream );

    /**
     * @brief Implements configuring method.
     * configuration <manageLockOn key="CompositeObjectKey" />
     * if configuration is empty, service locks all composite objects
     */
    CTRLMEMORY_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Updates dump locks when object are pushed removed or swaped in composite
    CTRLMEMORY_API virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw ( ::fwTools::Failed );

private :

    typedef std::string CompositeKeyType;

    typedef std::map< CompositeKeyType, ::fwData::ObjectLock > LockMapType;

    /// Add or remove lock ( _isLocked parameter ) on composite objects
    void setDumpLockOnImages(LockMapType &lockMap, SPTR(::fwData::Composite) _composite, bool _isLocked );

    /// Composite keys managed by this service
    std::vector< std::string > m_managedKeys;

    /// Container to manages dump lock on composite sub object
    LockMapType m_lockedObjects;
};

} // manager
} // ctrlMemory

#endif // __CTRLMEMORY_MANAGER_LOCKDUMPMANAGERSRV_HPP__

