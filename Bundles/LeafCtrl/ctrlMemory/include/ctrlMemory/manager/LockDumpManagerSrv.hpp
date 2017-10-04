/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLMEMORY_MANAGER_LOCKDUMPMANAGERSRV_HPP__
#define __CTRLMEMORY_MANAGER_LOCKDUMPMANAGERSRV_HPP__

#include "ctrlMemory/config.hpp"

#include <fwData/Composite.hpp>

#include <ctrlSelection/IManagerSrv.hpp>

#include <map>
#include <string>
#include <vector>

namespace fwData
{
class ObjectLock;
}

namespace ctrlMemory
{

namespace manager
{

/**
 * @brief This service allows to lock an object for dump when it is contained in the composite.
 */
class CTRLMEMORY_CLASS_API LockDumpManagerSrv : public ::ctrlSelection::IManagerSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (LockDumpManagerSrv)(::ctrlSelection::IManagerSrv) );

    /// Constructor.  Does nothing.
    CTRLMEMORY_API LockDumpManagerSrv() noexcept;

    /// Destructor. Does nothing.
    CTRLMEMORY_API virtual ~LockDumpManagerSrv() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_ADD_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_CHANGE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_REMOVE_OBJECTS_SLOT
     */
    CTRLMEMORY_API virtual KeyConnectionsType getObjSrvConnections() const override;

protected:

    /// Dump lock composite objects
    CTRLMEMORY_API virtual void starting() override;

    /// Dump unlock composite objects
    CTRLMEMORY_API virtual void stopping() override;

    /// Does nothing
    CTRLMEMORY_API virtual void reconfiguring() override;

    /// Does nothing
    CTRLMEMORY_API virtual void updating() override;

    /// Does nothing
    CTRLMEMORY_API virtual void info( std::ostream& _sstream ) override;

    /**
     * @brief Implements configuring method.
     * configuration \<manageLockOn key="CompositeObjectKey" /\>
     * if configuration is empty, service locks all composite objects
     */
    CTRLMEMORY_API virtual void configuring() override;

private:

    /// Slot: add objects
    void addObjects(::fwData::Composite::ContainerType objects);

    /// Slot: change objects
    void changeObjects(::fwData::Composite::ContainerType newObjects, ::fwData::Composite::ContainerType oldObjects);

    /// Slot: remove objects
    void removeObjects(::fwData::Composite::ContainerType objects);

    typedef std::string CompositeKeyType;

    typedef std::map< CompositeKeyType, ::fwData::ObjectLock > LockMapType;

    /// Add or remove lock ( _isLocked parameter ) on composite objects
    void setDumpLockOnImages(LockMapType& lockMap, ::fwData::Composite::ContainerType objects, bool _isLocked );

    /// Composite keys managed by this service
    std::vector< std::string > m_managedKeys;

    /// Container to manages dump lock on composite sub object
    LockMapType m_lockedObjects;
};

} // manager
} // ctrlMemory

#endif // __CTRLMEMORY_MANAGER_LOCKDUMPMANAGERSRV_HPP__

