#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Image.hpp>

#include <gui/editor/IDialogEditor.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "uiIO/action/ExportAcquisition.hpp"

namespace uiIO
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv, ::uiIO::action::ExportAcquisition , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

ExportAcquisition::ExportAcquisition( ) throw()
: m_ioSelectorSrvConfig ("IOSelectorServiceConfigVRRenderWriter")
{}

//------------------------------------------------------------------------------

ExportAcquisition::~ExportAcquisition() throw()
{}

//------------------------------------------------------------------------------

void ExportAcquisition::info(std::ostream &_sstream )
{
    _sstream << "Action for export Acquisition" << std::endl;
}

//------------------------------------------------------------------------------

void ExportAcquisition::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();

    BOOST_FOREACH( ConfigurationType ioConfig, m_configuration->getElements())
    {
        if( ioConfig->getName() == "IOSelectorSrvConfig" )
        {
            SLM_ASSERT("Attribute name is missing", ioConfig->hasAttribute("name")) ;
            m_ioSelectorSrvConfig = ioConfig->getExistingAttributeValue("name") ;
        }
    }
}

//------------------------------------------------------------------------------

void ExportAcquisition::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//------------------------------------------------------------------------------

void ExportAcquisition::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void ExportAcquisition::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pPatientDB not instanced", pPatientDB);

    if ( pPatientDB->getNumberOfPatients() )
    {
        ::fwData::Acquisition::sptr pAcquisition = ::fwComEd::fieldHelper::BackupHelper::getSelectedAcquisition(pPatientDB);
        if(!pAcquisition)
        {
            std::string msgInfo = "Sorry, it is impossible to export acquisition. There are not selected patients in the software.";
            ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Acquisition export");
            messageBox.setMessage( msgInfo );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        else
        {
            ::fwRuntime::ConfigurationElement::csptr ioCfg;
            ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(m_ioSelectorSrvConfig, "::uiIO::editor::IOSelectorService" ) ;
            OSLM_ASSERT("Sorry, there is not service configuration " << m_ioSelectorSrvConfig << " for ::uiIO::editor::IOSelectorService", ioCfg ) ;

            // Init and execute the service
            ::gui::editor::IDialogEditor::sptr pIOSelectorSrv =
                    ::fwServices::add< ::gui::editor::IDialogEditor >( pAcquisition, "::uiIO::editor::IOSelectorService" );
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

        std::string msgInfo = "Sorry, it is impossible to export acquisition. There are not loaded patients in the software.";
        ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Image export");
        messageBox.setMessage( msgInfo );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void ExportAcquisition::updating(::fwServices::ObjectMsg::csptr _msg) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace basicVRCtrl
