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

#include <fwComEd/PatientDBMsg.hpp>

#include "ctrlSelection/updater/ImageFromPDBSelectorUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::ImageFromPDBSelectorUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

ImageFromPDBSelectorUpdaterSrv::ImageFromPDBSelectorUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

ImageFromPDBSelectorUpdaterSrv::~ImageFromPDBSelectorUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void ImageFromPDBSelectorUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
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
                ::fwData::Image::sptr img;
                 if( it->get<3>() != REMOVE )
                 {
                     img = this->getImage(patientDB,_msg);
                 }
                // Udapte the composite object referenced by the composite key ( it->get<2>() )
                this->updateComposite(composite, img, it->get<2>(), it->get<3>() );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr ImageFromPDBSelectorUpdaterSrv::getImage( ::fwData::PatientDB::sptr patientDB, ::fwServices::ObjectMsg::csptr _msg )
{
    OSLM_FATAL_IF("Sorry the message classname ( " << _msg->getClassname() << " ) is not supported by AcqFromPDBSelectorUpdaterSrv. This service only support PatientDBMsg.",::fwComEd::PatientDBMsg::dynamicConstCast(_msg) == 0 );
    SLM_FATAL_IF("Sorry AcqFromPDBSelectorUpdaterSrv only support the ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED event", ! _msg->hasEvent( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED ) );
    ::fwComEd::PatientDBMsg::csptr pPatientDBMsg = ::fwComEd::PatientDBMsg::dynamicConstCast(_msg);

    // Get Selection
    ::fwData::Object::csptr pDataInfo = pPatientDBMsg->getDataInfo( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED );

    ::fwData::Integer::sptr myIntPat = ::fwData::Integer::dynamicCast( pDataInfo->children().at(0) );
    ::fwData::Integer::sptr myIntStu = ::fwData::Integer::dynamicCast( pDataInfo->children().at(1) );
    ::fwData::Integer::sptr myIntAcq = ::fwData::Integer::dynamicCast( pDataInfo->children().at(2) );

    // Patient selection
    ::fwData::PatientDB::PatientIterator patientIter = patientDB->getPatients().first;
    patientIter += myIntPat->value();

    // Study selection
    ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
    studyIter += myIntStu->value();

    // Acquisition selection
    ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
    acquisitionIter += myIntAcq->value();

    // Image selection
    ::fwData::Image::sptr imageSelectedInDB =  (*acquisitionIter)->getImage();

    return imageSelectedInDB;
}

//-----------------------------------------------------------------------------

void ImageFromPDBSelectorUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ImageFromPDBSelectorUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageFromPDBSelectorUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void ImageFromPDBSelectorUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageFromPDBSelectorUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageFromPDBSelectorUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
