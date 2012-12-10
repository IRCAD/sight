/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>

#include <gui/editor/IDialogEditor.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwComEd/fieldHelper/AnonymiseImage.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/fieldHelper/BackupHelper.hpp>
#include <fwComEd/PatientDBMsg.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "uiPatientDB/action/AnonymisePatient.hpp"


namespace uiPatientDB
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::uiPatientDB::action::AnonymisePatient , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

AnonymisePatient::AnonymisePatient( ) throw()
{}

//------------------------------------------------------------------------------

AnonymisePatient::~AnonymisePatient() throw()
{}

//------------------------------------------------------------------------------

void AnonymisePatient::info(std::ostream &_sstream )
{
    _sstream << "Action for anonymise Patient" << std::endl;
}

//------------------------------------------------------------------------------

void AnonymisePatient::configuring() throw( ::fwTools::Failed )
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------
void AnonymisePatient::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::LockAction lock(this->getSptr());

    ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pPatientDB not instanced", pPatientDB);

    if (pPatientDB->getNumberOfPatients() > 0)
    {
        ::fwData::Patient::sptr patient = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(pPatientDB);
        if(patient)
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Patient anonymisation");
            messageBox.setMessage( "Do you really want anonymise patient data ?" );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::NO);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
            ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();

            ::gui::editor::IDialogEditor::sptr pIOSelectorSrv;
            if ( answer == ::fwGui::dialog::IMessageDialog::YES )
            {
                ::fwComEd::fieldHelper::AnonymiseImage::anonymisePatient(patient);

                ::fwComEd::PatientDBMsg::NewSptr msg;
                msg->addEvent(::fwComEd::PatientDBMsg::NEW_PATIENT);
                ::fwServices::IEditionService::notify( this->getSptr(), pPatientDB, msg);
            }
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Patient anonymisation",
                    "Sorry, it is impossible to anonymise patient. There is not any patients selected.",
                    ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Patient anonymisation",
                "Sorry, it is impossible to anonymise patient. There are not loaded patients in the software.",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }

}

//------------------------------------------------------------------------------

void AnonymisePatient::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AnonymisePatient::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void AnonymisePatient::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiPatientDB
