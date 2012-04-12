/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// Services tools
#include <fwServices/Base.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "uiPatientDBQt/SPatientDBInserter.hpp"

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::io::IWriter, ::uiPatientDBQt::SPatientDBInserter, ::fwTools::Object );

//-----------------------------------------------------------------------------

namespace uiPatientDBQt
{

//-----------------------------------------------------------------------------

SPatientDBInserter::SPatientDBInserter() : m_patientDBfwID("")
{
    this->handlingEventOff();
}

//-----------------------------------------------------------------------------

SPatientDBInserter::~SPatientDBInserter() throw()
{}

//-----------------------------------------------------------------------------

void SPatientDBInserter::configuring() throw ( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElement::sptr patientDBConfig = m_configuration->findConfigurationElement("patientDB");
    SLM_ASSERT("Sorry you must have an 'patientDB' element to configure this service", patientDBConfig );
    SLM_ASSERT("Sorry you must have a 'fwID' attribute for 'patientDB' element to configure this service", patientDBConfig->hasAttribute("fwID") );
    m_patientDBfwID = patientDBConfig->getAttributeValue("fwID");
}

//-----------------------------------------------------------------------------

void SPatientDBInserter::configureWithIHM()
{

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
    if(m_patientDBfwID.empty())
    {
    ::fwGui::dialog::MessageDialog::showMessageDialog(
            "PatientDB Inserter",
            "SPatientDBInserter is not properly configured.\nPatienDB is missing.",
            ::fwGui::dialog::MessageDialog::WARNING);
    }
    else
    {
        ::fwData::Object::sptr object = this->getObject< ::fwData::Object >();

        ::fwData::PatientDB::sptr pdb   = ::fwData::PatientDB::dynamicCast(object);
        ::fwData::Patient::sptr patient = ::fwData::Patient::dynamicCast(object);
        ::fwData::Study::sptr study     = ::fwData::Study::dynamicCast(object);
        ::fwData::Acquisition::sptr acq = ::fwData::Acquisition::dynamicCast(object);
        ::fwData::Image::sptr img       = ::fwData::Image::dynamicCast(object);

        // Load a new patient DB
        ::fwData::PatientDB::sptr pPDB;
        if(pdb)
        {
            pPDB = pdb;
        }
        else if(patient)
        {
            pPDB = this->createPDB(patient);
        }
        else if(study)
        {
            pPDB = this->createPDB(study);
        }
        else if(acq)
        {
            pPDB = this->createPDB(acq);
        }
        else if(img)
        {
            pPDB = this->createPDB(img);
        }
        else
        {
            std::stringstream stream;
            stream << " Sorry, object " << object->getLeafClassname() << " is not supported.";
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "PatientDB Inserter",
                    stream.str(),
                    ::fwGui::dialog::MessageDialog::WARNING);
        }

        // Merge document
        if ( pPDB )
        {
            ::fwComEd::fieldHelper::MedicalImageHelpers::mergePatientDBInfo( pPDB, this->getPatientDB(), this->getSptr() );
        }
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

::fwData::PatientDB::sptr SPatientDBInserter::createPDB(::fwData::Patient::sptr patient) const
{
    ::fwData::PatientDB::NewSptr patientDB;
    patientDB->addPatient( patient );
    return patientDB;
}

//-----------------------------------------------------------------------------

::fwData::PatientDB::sptr SPatientDBInserter::createPDB(::fwData::Study::sptr study) const
{
    ::fwData::Patient::NewSptr patient;
    patient->addStudy(study);
    patient->setName("anonymous");
    patient->setFirstname("anonymous");
    return this->createPDB(patient);
}

//-----------------------------------------------------------------------------

::fwData::PatientDB::sptr SPatientDBInserter::createPDB(::fwData::Acquisition::sptr acq) const
{
    ::fwData::Study::NewSptr study;
    study->setHospital("Ircad");
    study->addAcquisition(acq);
    return this->createPDB(study);
}

//-----------------------------------------------------------------------------

::fwData::PatientDB::sptr SPatientDBInserter::createPDB(::fwData::Image::sptr img) const
{
    ::fwData::Acquisition::NewSptr acquisition;
    acquisition->setImage(img);
    return this->createPDB(acquisition);
}

//-----------------------------------------------------------------------------

::fwData::PatientDB::sptr SPatientDBInserter::getPatientDB() const
{
    ::fwData::PatientDB::sptr pdb;
    pdb =  ::fwData::PatientDB::dynamicCast( ::fwTools::fwID::getObject( m_patientDBfwID ) );
    OSLM_ASSERT( "Sorry, selection object (fwId="<< m_patientDBfwID<<") does not exist in the system or it is not a PatientDB object", pdb );

    return pdb;
}

//-----------------------------------------------------------------------------

} // namespace uiPatientDBQt


