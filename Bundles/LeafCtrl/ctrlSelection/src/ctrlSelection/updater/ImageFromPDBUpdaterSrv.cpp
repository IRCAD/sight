/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/updater/ImageFromPDBUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::ImageFromPDBUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

ImageFromPDBUpdaterSrv::ImageFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

ImageFromPDBUpdaterSrv::~ImageFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void ImageFromPDBUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
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
            SLM_ASSERT("Sorry, the subject of message is not a ::fwData::Object", obj);

            // Test if we manage this event from this object message uid ( it->get<1>() )
            if( obj->getID() == it->get<1>() )
            {
                ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::dynamicCast(obj);
                SLM_ASSERT("Sorry, the subject of message is not a ::fwData::PatientDB", patientDB);
                ::fwData::Image::sptr img;
                 if( it->get<3>() != REMOVE )
                 {
                     img = this->getImage(patientDB);
                 }
                // Udapte the composite object referenced by the composite key ( it->get<2>() )
                this->updateComposite(composite, img, it->get<2>(), it->get<3>() );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr ImageFromPDBUpdaterSrv::getImage(::fwData::PatientDB::sptr patientDB)
{
    SLM_ASSERT("No patient in DB", patientDB->getNumberOfPatients());
    ::fwData::Patient::sptr patient = patientDB->getPatients().front();
    SLM_ASSERT("No study in patient", patient->getNumberOfStudies());
    ::fwData::Study::sptr study     = patient->getStudies().front();
    SLM_ASSERT("No acquisition in study", study->getNumberOfAcquisitions());
    ::fwData::Acquisition::sptr acq = study->getAcquisitions().front();
    return acq->getImage();
}

//-----------------------------------------------------------------------------

void ImageFromPDBUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ImageFromPDBUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageFromPDBUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void ImageFromPDBUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageFromPDBUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageFromPDBUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
