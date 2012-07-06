/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

#include <fwGui/dialog/MessageDialog.hpp>

#include "uiPatientDB/action/ExportPatient.hpp"


namespace uiPatientDB
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiPatientDB::action::ExportPatient , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

ExportPatient::ExportPatient( ) throw()
: m_ioSelectorSrvConfig ("IOSelectorServiceConfigVRRenderWriter")
{}

//------------------------------------------------------------------------------

ExportPatient::~ExportPatient() throw()
{}

//------------------------------------------------------------------------------

void ExportPatient::info(std::ostream &_sstream )
{
    _sstream << "Action for export Patient" << std::endl;
}

//------------------------------------------------------------------------------

void ExportPatient::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->::fwGui::IActionSrv::initialize();

    for(    ::fwRuntime::ConfigurationElementContainer::Iterator iter = m_configuration->begin() ;
            iter != m_configuration->end() ;
            ++iter )
    {
        if( (*iter)->getName() == "IOSelectorSrvConfig" )
        {
            SLM_ASSERT("Missing 'name' attribute", (*iter)->hasAttribute("name")) ;
            m_ioSelectorSrvConfig = (*iter)->getExistingAttributeValue("name") ;
        }
    }
}

//------------------------------------------------------------------------------
void ExportPatient::updating( ) throw(::fwTools::Failed)
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
            messageBox.setMessage( "Do you want anonymise patient data ?" );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::NO);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
            ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();

            // Get Config
            ::fwRuntime::ConfigurationElement::csptr ioCfg;
            ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( m_ioSelectorSrvConfig , "::uiIO::editor::IOSelectorService" ) ;
            SLM_ASSERT("Sorry, there is not service configuration " << m_ioSelectorSrvConfig << " for ::uiIO::editor::IOSelectorService", ioCfg ) ;

            ::gui::editor::IDialogEditor::sptr pIOSelectorSrv;
            if ( answer == ::fwGui::dialog::IMessageDialog::YES )
            {
                ::fwData::Patient::sptr pAnonymisedPatient = ::fwComEd::fieldHelper::AnonymiseImage::createAnonymisedPatient(patient);

                // Init and execute the service
                pIOSelectorSrv = ::fwServices::add< ::gui::editor::IDialogEditor >( pAnonymisedPatient, "::uiIO::editor::IOSelectorService", "IOSelectorServiceWriter" );
                pIOSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(ioCfg) ) ;
                pIOSelectorSrv->configure() ;
                pIOSelectorSrv->start();
                pIOSelectorSrv->update();
                pIOSelectorSrv->stop();
                ::fwServices::OSR::unregisterService( pIOSelectorSrv );
            }
            else if ( answer == ::fwGui::dialog::IMessageDialog::NO )
            {
                // Init and execute the service
                pIOSelectorSrv = ::fwServices::add< ::gui::editor::IDialogEditor >( patient, "::uiIO::editor::IOSelectorService", "IOSelectorServiceWriter" );
                pIOSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(ioCfg) ) ;
                pIOSelectorSrv->configure() ;
                pIOSelectorSrv->start();
                pIOSelectorSrv->update();
                pIOSelectorSrv->stop();
                ::fwServices::OSR::unregisterService( pIOSelectorSrv );
            }
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Patient export",
                    "Sorry, it is impossible to export patients. No patient selected.",
                    ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Patient export",
                "Sorry, it is impossible to export patients. There are not loaded patients in the software.",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }

}

//------------------------------------------------------------------------------

void ExportPatient::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ExportPatient::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void ExportPatient::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiPatientDB
