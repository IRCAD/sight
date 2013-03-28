/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include "ctrlMemory/manager/LockDumpManagerSrv.hpp"

namespace ctrlMemory
{

namespace manager
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IManagerSrv, ::ctrlMemory::manager::LockDumpManagerSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

LockDumpManagerSrv::LockDumpManagerSrv() throw()
{
//    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_KEYS );
//    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS );
//    addNewHandledEvent( ::fwComEd::CompositeMsg::CHANGED_KEYS );
}

//-----------------------------------------------------------------------------

LockDumpManagerSrv::~LockDumpManagerSrv() throw()
{}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    if(compositeMsg)
    {
        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getAddedKeys();
            this->setDumpLockOnImages(m_lockedObjects, fields,true);
        }

        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::CHANGED_KEYS ) )
        {
            ::fwData::Composite::sptr fields;
            LockMapType newLocks = m_lockedObjects;

            fields = compositeMsg->getOldChangedKeys();
            this->setDumpLockOnImages(newLocks, fields,false);

            fields = compositeMsg->getNewChangedKeys();
            this->setDumpLockOnImages(newLocks, fields,true);

            m_lockedObjects = newLocks;
        }

        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getRemovedKeys();
            this->setDumpLockOnImages(m_lockedObjects, fields,false);
        }
    }
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::setDumpLockOnImages( LockMapType &lockMap, ::fwData::Composite::sptr _composite, bool _isLocked )
{
    BOOST_FOREACH( ::fwData::Composite::value_type objectId, *_composite )
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
    this->setDumpLockOnImages(m_lockedObjects, composite, true);
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
        SLM_FATAL_IF( "Sorry, attribute \"key\" is missing", !(*item)->hasAttribute("key") );
        std::string key =  (*item)->getExistingAttributeValue("key");
        m_managedKeys.push_back(key);
    }
}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void LockDumpManagerSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // manager
} // ctrlMemory
