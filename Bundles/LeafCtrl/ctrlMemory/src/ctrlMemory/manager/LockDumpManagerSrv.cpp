/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlMemory/manager/LockDumpManagerSrv.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/ObjectLock.hpp>

#include <fwServices/macros.hpp>


namespace ctrlMemory
{

namespace manager
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IManagerSrv, ::ctrlMemory::manager::LockDumpManagerSrv, ::fwData::Composite );

//-----------------------------------------------------------------------------

LockDumpManagerSrv::LockDumpManagerSrv() throw()
{
    newSlot(s_ADD_OBJECTS_SLOT, &LockDumpManagerSrv::addObjects, this);
    newSlot(s_CHANGE_OBJECTS_SLOT, &LockDumpManagerSrv::changeObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &LockDumpManagerSrv::removeObjects, this);
}

//-----------------------------------------------------------------------------

LockDumpManagerSrv::~LockDumpManagerSrv() throw()
{
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::setDumpLockOnImages( LockMapType &lockMap, ::fwData::Composite::ContainerType objects,
                                              bool _isLocked )
{
    for( ::fwData::Composite::value_type objectId :  objects )
    {
        if ( m_managedKeys.empty()
             || std::find(m_managedKeys.begin(), m_managedKeys.end(), objectId.first) != m_managedKeys.end() )
        {
            if (_isLocked)
            {
                lockMap.insert( LockMapType::value_type( objectId.first, ::fwData::ObjectLock( objectId.second ) ) );
                OSLM_TRACE("Locking Key : " << objectId.first);
            }
            else
            {
                SLM_ASSERT( "ERROR image not locked !!!", lockMap.find(objectId.first) != lockMap.end() );
                lockMap.erase( objectId.first );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::starting()  throw ( ::fwTools::Failed )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    this->setDumpLockOnImages(m_lockedObjects, composite->getContainer(), true);
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::stopping()  throw ( ::fwTools::Failed )
{
    m_lockedObjects.clear();
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::configuring()  throw ( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElementContainer updaters = m_configuration->findAllConfigurationElement("manageLockOn");

    m_managedKeys.clear();

    for(    ::fwRuntime::ConfigurationElementContainer::Iterator item = updaters.begin();
            item != updaters.end();
            ++item )
    {
        SLM_FATAL_IF( "The attribute \"key\" is missing", !(*item)->hasAttribute("key") );
        std::string key = (*item)->getExistingAttributeValue("key");
        m_managedKeys.push_back(key);
    }
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::reconfiguring()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::updating() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void LockDumpManagerSrv::addObjects(::fwData::Composite::ContainerType objects)
{
    this->setDumpLockOnImages(m_lockedObjects, objects, true);
}

//------------------------------------------------------------------------------

void LockDumpManagerSrv::changeObjects(::fwData::Composite::ContainerType newObjects,
                                       ::fwData::Composite::ContainerType oldObjects)
{
    LockMapType newLocks = m_lockedObjects;

    this->setDumpLockOnImages(newLocks, oldObjects, false);
    this->setDumpLockOnImages(newLocks, newObjects, true);

    m_lockedObjects = newLocks;
}

//------------------------------------------------------------------------------

void LockDumpManagerSrv::removeObjects(::fwData::Composite::ContainerType objects)
{
    this->setDumpLockOnImages(m_lockedObjects, objects, false);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType LockDumpManagerSrv::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

} // manager

} // ctrlMemory

