/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/ModelSeriesMsg.hpp>

#include <fwData/Reconstruction.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/updater/SReconstructionFromModelSeriesUpdater.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::SReconstructionFromModelSeriesUpdater, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SReconstructionFromModelSeriesUpdater::SReconstructionFromModelSeriesUpdater() throw()
{}

//-----------------------------------------------------------------------------

SReconstructionFromModelSeriesUpdater::~SReconstructionFromModelSeriesUpdater() throw()
{}

//-----------------------------------------------------------------------------

void SReconstructionFromModelSeriesUpdater::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
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

::fwData::Reconstruction::sptr SReconstructionFromModelSeriesUpdater::getReconstruction(::fwServices::ObjectMsg::csptr msg)
{
    ::fwData::Reconstruction::sptr reconst;
    ::fwComEd::ModelSeriesMsg::csptr pModelSeriesMsg = ::fwComEd::ModelSeriesMsg::dynamicConstCast( msg );
    SLM_FATAL_IF("Received message isn't a ModelSeriesMsg", !pModelSeriesMsg);

    SLM_ASSERT("Sorry, there's not NEW_RECONSTRUCTION_SELECTED in ModelSeriesMsg", pModelSeriesMsg->hasEvent( ::fwComEd::ModelSeriesMsg::NEW_RECONSTRUCTION_SELECTED ));
    ::fwData::Object::csptr dataInfo = pModelSeriesMsg->getDataInfo(::fwComEd::ModelSeriesMsg::NEW_RECONSTRUCTION_SELECTED);
    SLM_ASSERT("Sorry, there is no dataInfo", dataInfo);
    ::fwData::String::csptr reconstructionSelectedUID = ::fwData::String::dynamicConstCast( dataInfo ) ;
    SLM_ASSERT("Sorry,  there is no reconstruction selected", reconstructionSelectedUID);
    reconst =  ::fwData::Reconstruction::dynamicCast( ::fwTools::fwID::getObject(reconstructionSelectedUID->value() ));

    return reconst;
}

//-----------------------------------------------------------------------------

void SReconstructionFromModelSeriesUpdater::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SReconstructionFromModelSeriesUpdater::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SReconstructionFromModelSeriesUpdater::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void SReconstructionFromModelSeriesUpdater::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SReconstructionFromModelSeriesUpdater::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SReconstructionFromModelSeriesUpdater::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // namespace updater
} // namespace ctrlSelection

