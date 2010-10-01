/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>
#include <fwComEd/ResectionMsg.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/updater/ReconstructionFromResecUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::ReconstructionFromResecUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

ReconstructionFromResecUpdaterSrv::ReconstructionFromResecUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

ReconstructionFromResecUpdaterSrv::~ReconstructionFromResecUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void ReconstructionFromResecUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
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
            if( it->get<1>() == "*" || obj->getID() == it->get<1>() )
            {
                ::fwData::Reconstruction::sptr reconst;
                ActionType action = it->get<3>();
                if( action != REMOVE && action != REMOVE_IF_PRESENT && action != DO_NOTHING)
                {
                    reconst = this->getReconstruction(_msg);
                }
                // Udapte the composite object referenced by the composite key ( it->get<2>() )
                this->updateComposite(composite, reconst, it->get<2>(), it->get<3>() );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwData::Reconstruction::sptr ReconstructionFromResecUpdaterSrv::getReconstruction(::fwServices::ObjectMsg::csptr msg)
{
    ::fwData::Reconstruction::sptr reconst;
    ::fwComEd::ResectionMsg::csptr pResectionMsg = ::fwComEd::ResectionMsg::dynamicConstCast( msg );
    SLM_FATAL_IF("Sorry, it's not an ResectionMsg", !pResectionMsg);

    SLM_ASSERT("Sorry, there's not NEW_RECONSTRUCTION_SELECTED in ResectionMsg", pResectionMsg->hasEvent( ::fwComEd::ResectionMsg::NEW_RECONSTRUCTION_SELECTED ));
    ::fwData::Object::csptr dataInfo = pResectionMsg->getDataInfo(::fwComEd::ResectionMsg::NEW_RECONSTRUCTION_SELECTED);
    SLM_ASSERT("Sorry, there is no dataInfo", dataInfo);
    ::fwData::String::csptr reconstructionSelectedUID = ::fwData::String::dynamicConstCast( dataInfo ) ;
    SLM_ASSERT("Sorry,  there is no reconstruction selected", reconstructionSelectedUID);
    reconst =  ::fwData::Reconstruction::dynamicCast(  ::fwTools::fwID::getObject( reconstructionSelectedUID->value()) );

    return reconst;
}

//-----------------------------------------------------------------------------

void ReconstructionFromResecUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ReconstructionFromResecUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ReconstructionFromResecUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void ReconstructionFromResecUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ReconstructionFromResecUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ReconstructionFromResecUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
