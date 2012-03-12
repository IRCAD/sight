/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/updater/AcqFromPDBUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::AcqFromPDBUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

AcqFromPDBUpdaterSrv::AcqFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

AcqFromPDBUpdaterSrv::~AcqFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
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
                ::fwData::Acquisition::sptr acq;
                if( it->get<3>() != REMOVE )
                {
                    acq = this->getAcquisition(patientDB);
                }
                // Udapte the composite object referenced by the composite key ( it->get<2>() )
                this->updateComposite(composite, acq, it->get<2>(), it->get<3>() );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwData::Acquisition::sptr AcqFromPDBUpdaterSrv::getAcquisition(::fwData::PatientDB::sptr patientDB)
{
    ::fwData::Patient::sptr patient = patientDB->getPatients()[ 0 ];
    ::fwData::Study::sptr study = patient->getStudies()[ 0 ];
    ::fwData::Acquisition::sptr acquisition = study->getAcquisitions()[ 0 ];

    return acquisition;
}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
