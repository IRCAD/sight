/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// Service associated data
#include <fwData/PatientDB.hpp>

// Services tools
#include <fwServices/Base.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include "uiPatientDBQt/SPatientDBInserter.hpp"

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::editor::IDialogEditor, ::uiPatientDBQt::SPatientDBInserter, ::fwData::PatientDB );

//-----------------------------------------------------------------------------

namespace uiPatientDBQt
{

//-----------------------------------------------------------------------------

SPatientDBInserter::SPatientDBInserter()
{
    this->handlingEventOff();
}

//-----------------------------------------------------------------------------

SPatientDBInserter::~SPatientDBInserter() throw()
{}

//-----------------------------------------------------------------------------

void SPatientDBInserter::configuring() throw ( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElement::sptr imgConfig = m_configuration->findConfigurationElement("image");
    SLM_ASSERT("Sorry you must have an 'image' element to configure this service", imgConfig );
    SLM_ASSERT("Sorry you must have a 'selectionUid' attribute for 'image' element to configure this service", imgConfig->hasAttribute("selectionUid") );
    SLM_ASSERT("Sorry you must have a 'key' attribute for 'image' element to configure this service", imgConfig->hasAttribute("key") );
    m_selectionUid = imgConfig->getAttributeValue("selectionUid");
    m_selectionKey = imgConfig->getAttributeValue("key");
}

//-----------------------------------------------------------------------------

void SPatientDBInserter::starting() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SPatientDBInserter::stopping() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SPatientDBInserter::updating() throw ( ::fwTools::Failed )
{
    ::fwData::PatientDB::sptr pDocumentPDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pDocumentPDB not instanced", pDocumentPDB);

    // Load a new patient DB
    ::fwData::PatientDB::sptr pPDB;
    pPDB = this->createPDB();

    // Merge document
    if ( pPDB )
    {
        ::fwComEd::fieldHelper::MedicalImageHelpers::mergePatientDBInfo( pPDB, pDocumentPDB, this->getSptr() );
    }
}

//-----------------------------------------------------------------------------

void SPatientDBInserter::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SPatientDBInserter::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

::fwData::PatientDB::sptr SPatientDBInserter::createPDB() const
{
    ::fwData::PatientDB::sptr patientDB;
    ::fwData::Image::sptr img = this->getImageFromSelection();
    if ( img )
    {
        patientDB = ::fwData::PatientDB::New();
        ::fwData::Patient::NewSptr patient;
        ::fwData::Study::NewSptr study;
        ::fwData::Acquisition::NewSptr acquisition;

        patientDB->addPatient( patient );
        patient->addStudy( study );
        patient->setName("anonymous");
        patient->setFirstname("anonymous");
        study->addAcquisition( acquisition );
        study->setHospital("Ircad");
        acquisition->setImage( img );
    }
    return patientDB;
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr SPatientDBInserter::getImageFromSelection() const
{
    ::fwData::Image::sptr img;
    ::fwData::Composite::sptr composite =  ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject( m_selectionUid ) );
    OSLM_ASSERT( "Sorry, selection object (fwId="<< m_selectionUid<<") does not exist in the system or it is not a composite object", composite );

    ::fwData::Composite::ContainerType::iterator it = composite->find( m_selectionKey );
    if ( it != composite->end() )
    {
        ::fwData::Image::sptr imgTmp = ::fwData::Image::dynamicCast( it->second );
        if( ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( imgTmp ) )
        {
            img = imgTmp;
        }
    }
    return img;
}

//-----------------------------------------------------------------------------

} // namespace uiPatientDBQt


