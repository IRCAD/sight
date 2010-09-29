/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>

#include <fwComEd/ResectionDBMsg.hpp>

#include "ctrlSelection/updater/ResecFromPDBUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::ResecFromPDBUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

ResecFromPDBUpdaterSrv::ResecFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

ResecFromPDBUpdaterSrv::~ResecFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void ResecFromPDBUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    for (   ManagedEvents::iterator it = m_managedEvents.begin();
            it != m_managedEvents.end();
            ++it )
    {
        // First element of tuple ( it->get<0>() ) is the event, test if message correspond to a defined event
        if( _msg->hasEvent( it->get<0>() ) )
        {
            ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast( _msg->getSubject().lock() );
            SLM_ASSERT(obj,"Sorry, the subject of message is not a ::fwData::Object");

            // Test if we manage this event from this object message uid ( it->get<1>() )
            if( obj->getUUID() == it->get<1>() )
            {
                ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::dynamicCast(obj);
                SLM_ASSERT("Sorry, the subject of message is not a ::fwData::PatientDB", patientDB);
                ::fwData::Resection::sptr resec;
                if( it->get<3>() != REMOVE && it->get<3>() != REMOVE_IF_PRESENT )
                {
                    resec = this->getResection( patientDB, _msg );
                }
                // Udapte the composite object referenced by the composite key ( it->get<2>() )
                this->updateComposite(composite, resec, it->get<2>(), it->get<3>() );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwData::Resection::sptr ResecFromPDBUpdaterSrv::getResection( ::fwData::PatientDB::sptr patientDB, ::fwServices::ObjectMsg::csptr _msg )
{
    OSLM_FATAL_IF("Sorry the message classname ( " << _msg->getClassname() << " ) is not supported by ResecFromPDBUpdaterSrv. This service only support ResectionDBMsg.",
            ::fwComEd::ResectionDBMsg::dynamicConstCast(_msg) == 0 );
    SLM_FATAL_IF("Sorry ResecFromPDBUpdaterSrv only support "
            " ::fwComEd::ResectionDBMsg::NEW_RESECTION_SELECTED or "
            " ::fwComEd::ResectionDBMsg::NEW_SAFE_PART_SELECTED events",
            ! _msg->hasEvent( ::fwComEd::ResectionDBMsg::NEW_RESECTION_SELECTED ) &&
            ! _msg->hasEvent( ::fwComEd::ResectionDBMsg::NEW_SAFE_PART_SELECTED ));

    ::fwComEd::ResectionDBMsg::csptr pResectionDBMsg = ::fwComEd::ResectionDBMsg::dynamicConstCast(_msg);

    ::fwData::Resection::sptr resec;
    if(pResectionDBMsg->hasEvent( ::fwComEd::ResectionDBMsg::NEW_SAFE_PART_SELECTED ))
    {
        resec = pResectionDBMsg->getSafePartSelected();
    }
    else if(pResectionDBMsg->hasEvent( ::fwComEd::ResectionDBMsg::NEW_RESECTION_SELECTED ))
    {
        resec = pResectionDBMsg->getResectionSelected();
    }
    SLM_ASSERT("Sorry, no Resection selected found", resec);
    return resec;
}

//-----------------------------------------------------------------------------

void ResecFromPDBUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ResecFromPDBUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ResecFromPDBUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void ResecFromPDBUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ResecFromPDBUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ResecFromPDBUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
