/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "uiPatientDB/action/ErasePatient.hpp"


namespace uiPatientDB
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiPatientDB::action::ErasePatient , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

ErasePatient::ErasePatient( ) throw()
{}

//------------------------------------------------------------------------------

ErasePatient::~ErasePatient() throw()
{}

//------------------------------------------------------------------------------

void ErasePatient::info(std::ostream &_sstream )
{
    _sstream << "Action for erase PatientDB" << std::endl;
}
//------------------------------------------------------------------------------

void ErasePatient::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr pPatientDB = this->getObject<  ::fwData::PatientDB > ();

    if(!pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
        return;

    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Erase selected data");
    messageBox.setMessage( "Are you sure to erase selected data ?" );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();

    if ( answer != ::fwGui::dialog::IMessageDialog::OK )
        return;

    ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

    /*::fwData::Object * const vSelection = mySpecificMsg->m_modifiedObject;*/
    ::fwData::Integer::sptr myIntPat = ::fwData::Integer::dynamicCast( pDataInfo->children().at(0) );
    ::fwData::Integer::sptr myIntStu = ::fwData::Integer::dynamicCast( pDataInfo->children().at(1) );
    ::fwData::Integer::sptr myIntAcq = ::fwData::Integer::dynamicCast( pDataInfo->children().at(2) );


    ::fwData::Patient::sptr pPatientBAK = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(pPatientDB);

    // Patient selection
    ::fwData::PatientDB::PatientIterator patientIter = pPatientDB->getPatients().first;
    patientIter +=  myIntPat->value();
    OSLM_DEBUG("erase Patient: "<<myIntPat->value());

    // Study selection
    ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
    int nbStudies = (*patientIter)->getStudySize();
    studyIter +=  myIntStu->value();
    OSLM_DEBUG("erase Study: "<<myIntStu->value());

    // Acquisition selection
    ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
    int nbAcquisitions = (*studyIter)->getAcquisitionSize();
    acquisitionIter +=  myIntAcq->value();
    OSLM_DEBUG("erase Acquisition: "<<myIntAcq->value());

    (*studyIter)->getField( ::fwData::Study::ID_ACQUISITIONS )->children().erase( acquisitionIter.base() );

    if( nbAcquisitions == 1 )
    {
        // Erase study
        (*patientIter)->getField( ::fwData::Patient::ID_STUDIES )->children().erase( studyIter.base() );

        if ( nbStudies == 1 )
        {
            // Erase patient
            pPatientDB->getField( ::fwData::PatientDB::ID_PATIENTS )->children().erase( patientIter.base() );
            pPatientDB->removeField( fwComEd::Dictionary::m_imageSelectedId);
        }
        else
        {
            // Select previous study
            if (myIntStu->value() > 0)
            {
                myIntStu->value()--;
            }
        }
    }
    else
    {
        // Select previous acquisition
        if (myIntAcq->value() > 0)
        {
            myIntAcq->value()--;
        }
    }

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent(::fwComEd::PatientDBMsg::CLEAR_PATIENT);
    ::fwServices::IEditionService::notify(this->getSptr(), pPatientDB, msg);
}

//------------------------------------------------------------------------------

void ErasePatient::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ErasePatient::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ErasePatient::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void ErasePatient::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiPatientDB
