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
#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <fwDataTools/Patient.hpp>

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
    ::fwData::Patient::sptr patient = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(pPatientDB);
    ::fwData::Study::sptr study = ::fwComEd::fieldHelper::BackupHelper::getSelectedStudy(pPatientDB);
    ::fwData::Acquisition::sptr acquisition = ::fwComEd::fieldHelper::BackupHelper::getSelectedAcquisition(pPatientDB);
    ::fwData::Image::sptr image = ::fwComEd::fieldHelper::BackupHelper::getSelectedImage(pPatientDB);

    if(!image)
    {
        return;
    }

    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Erase selected data");
    messageBox.setMessage( "Are you sure to erase selected data ?" );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();

    if ( answer != ::fwGui::dialog::IMessageDialog::OK )
    {
        return;
    }

    ::fwComEd::fieldHelper::BackupHelper::SelectionIdType myIntPat, myIntStu, myIntAcq;
    myIntPat = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatientIdx(pPatientDB);
    myIntStu = ::fwComEd::fieldHelper::BackupHelper::getSelectedStudyIdx(pPatientDB);
    myIntAcq = ::fwComEd::fieldHelper::BackupHelper::getSelectedAcquisitionIdx(pPatientDB);

    // Erase acquisition
    ::fwDataTools::Patient::removeAcquisition(study, acquisition);
    myIntAcq--;
    if( study->getAcquisitions().empty() )
    {
        // Erase study
        ::fwDataTools::Patient::removeStudy(patient, study);
        myIntStu--;
        if ( patient->getStudies().empty() )
        {
            // Erase patient
            ::fwDataTools::Patient::removePatient(pPatientDB, patient);
            myIntPat--;
        }
    }

    ::fwComEd::fieldHelper::BackupHelper::setSelection(pPatientDB, myIntPat, myIntStu, myIntAcq);
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
