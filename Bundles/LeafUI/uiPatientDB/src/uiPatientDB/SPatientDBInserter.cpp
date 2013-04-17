/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// Services tools
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwData/String.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>
#include <fwGui/dialog/InputDialog.hpp>

#include "uiPatientDB/SPatientDBInserter.hpp"

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::io::IWriter, ::uiPatientDB::SPatientDBInserter, ::fwData::Object );

//-----------------------------------------------------------------------------

namespace uiPatientDB
{

//-----------------------------------------------------------------------------

SPatientDBInserter::SPatientDBInserter() : m_patientDBfwID("")
{
//    this->handlingEventOff();
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
    if(m_patientDBfwID.empty())
    {
        std::vector< std::string > availablePatientDB;
        std::vector<  ::fwData::Object::sptr > allObjs =  ::fwServices::OSR::getObjects();
        BOOST_FOREACH(::fwData::Object::sptr obj, allObjs)
        {
            ::fwData::PatientDB::sptr pdb = ::fwData::PatientDB::dynamicCast(obj);
            if( pdb )
            {
                availablePatientDB.push_back(pdb->getID());
            }
        }
        ::fwData::Object::sptr object = this->getObject();

        ::fwGui::dialog::SelectorDialog::NewSptr selector;
        selector->setSelections(availablePatientDB);
        selector->setTitle("Select PatientDB");
        std::stringstream stream;
        stream << "Select a PatientDB to push specified "<< object->getLeafClassname() << " object";
        selector->setMessage(stream.str());
        std::string selection = selector->show();
        m_patientDBfwID = selection;
    }
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
        ::fwData::Object::sptr object = this->getObject();

        ::fwData::PatientDB::sptr pdb   = ::fwData::PatientDB::dynamicCast(object);
        ::fwData::Patient::sptr patient = ::fwData::Patient::dynamicCast(object);
        ::fwData::Study::sptr study     = ::fwData::Study::dynamicCast(object);
        ::fwData::Acquisition::sptr acq = ::fwData::Acquisition::dynamicCast(object);
        ::fwData::Image::sptr img       = ::fwData::Image::dynamicCast(object);

        // Load a new patient DB
        ::fwData::PatientDB::sptr pPDB;
        if(pdb)
        {
            ::fwData::PatientDB::sptr copy;
            copy = ::fwData::Object::copy(pdb);
            pPDB = copy;
        }
        else if(patient)
        {
            ::fwData::Patient::sptr copy;
            copy = ::fwData::Object::copy(patient);
            pPDB = this->createPDB(copy);
        }
        else if(study)
        {
            ::fwData::Study::sptr copy;
            copy = ::fwData::Object::copy(study);
            pPDB = this->createPDB(copy);
        }
        else if(acq)
        {
            ::fwData::Acquisition::sptr copy;
            copy = ::fwData::Object::copy(acq);
            ::fwData::Image::sptr copyImg = copy->getImage();
            ::fwData::String::sptr comment;
            comment = copyImg->setDefaultField< ::fwData::String >( ::fwComEd::Dictionary::m_commentId,
                                                                    ::fwData::String::New(""));
            std::string result;
            result = ::fwGui::dialog::InputDialog::showInputDialog("Image comment", "Enter comment", comment->value());
            comment->value() = result;
            pPDB = this->createPDB(copy);
        }
        else if(img)
        {
            bool exportImage = true;
            if (!::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(img))
            {
                ::fwGui::dialog::MessageDialog dialog;
                dialog.setTitle("PatientDB Inserter");
                dialog.setMessage("This image is not valid, would you really export it ?");
                dialog.setIcon(::fwGui::dialog::MessageDialog::QUESTION);
                dialog.addButton(::fwGui::dialog::MessageDialog::YES_NO);
                ::fwGui::dialog::MessageDialog::Buttons result;
                result = dialog.show();
                exportImage = (result == ::fwGui::dialog::MessageDialog::YES);
            }
            if (exportImage)
            {
                ::fwData::Image::sptr copy;
                copy = ::fwData::Object::copy(img);
                ::fwComEd::fieldHelper::MedicalImageHelpers::checkComment(copy);
                ::fwData::String::sptr comment;
                comment = copy->setDefaultField< ::fwData::String >( ::fwComEd::Dictionary::m_commentId,
                                                              ::fwData::String::New(""));
                std::string result;
                result = ::fwGui::dialog::InputDialog::showInputDialog("Image comment", "Enter comment",
                                                                       comment->value());
                comment->value() = result;
                pPDB = this->createPDB(copy);
            }
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

void SPatientDBInserter::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
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

} // namespace uiPatientDB


