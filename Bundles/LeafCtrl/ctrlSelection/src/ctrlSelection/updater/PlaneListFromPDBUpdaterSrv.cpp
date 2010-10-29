/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>


#include <fwComEd/ResectionDBMsg.hpp>

#include "ctrlSelection/updater/PlaneListFromPDBUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::PlaneListFromPDBUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

PlaneListFromPDBUpdaterSrv::PlaneListFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

PlaneListFromPDBUpdaterSrv::~PlaneListFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void PlaneListFromPDBUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
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
            if( obj->getID() == it->get<1>() )
            {
                ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::dynamicCast(obj);
                SLM_ASSERT("Sorry, the subject of message is not a ::fwData::PatientDB", patientDB);
                ::fwData::PlaneList::sptr planeList;
                if( it->get<3>() != REMOVE && it->get<3>() != REMOVE_IF_PRESENT )
                {
                    planeList = this->getPlaneList( patientDB, _msg );
                }
                // Udapte the composite object referenced by the composite key ( it->get<2>() )
                this->updateComposite(composite, planeList, it->get<2>(), it->get<3>() );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwData::PlaneList::sptr PlaneListFromPDBUpdaterSrv::getPlaneList( ::fwData::PatientDB::sptr patientDB, ::fwServices::ObjectMsg::csptr _msg )
{
    OSLM_FATAL_IF("Sorry the message classname ( " << _msg->getClassname() << " ) is not supported by PlaneListFromPDBUpdaterSrv. This service only support ResectionDBMsg.",
            ::fwComEd::ResectionDBMsg::dynamicConstCast(_msg) == 0 );
    SLM_FATAL_IF("Sorry PlaneListFromPDBUpdaterSrv only support "
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
    ::fwData::PlaneList::sptr planeList = resec->getPlaneList();
    SLM_ASSERT("Sorry, no PlaneList selected found", planeList);

    return planeList;
}

//-----------------------------------------------------------------------------

void PlaneListFromPDBUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void PlaneListFromPDBUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void PlaneListFromPDBUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void PlaneListFromPDBUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void PlaneListFromPDBUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void PlaneListFromPDBUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
