#include <wx/wx.h>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Image.hpp>
#include <gui/editor/IEditor.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/bundle/runtime.hpp>

#include "uiIO/action/ExportAcquisition.hpp"

namespace uiIO
{
namespace action
{

REGISTER_SERVICE( ::gui::action::IAction , ::uiIO::action::ExportAcquisition , ::fwData::PatientDB ) ;

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

        this->::gui::action::IAction::configuring();

        for(   ::fwRuntime::ConfigurationElementContainer::Iterator iter = m_configuration->begin() ;
                iter != m_configuration->end() ;
                ++iter )
        {
            if( (*iter)->getName() == "IOSelectorSrvConfig" )
            {
                assert( (*iter)->hasAttribute("name")) ;
                m_ioSelectorSrvConfig = (*iter)->getExistingAttributeValue("name") ;
            }
        }
}

void ExportAcquisition::starting() throw( ::fwTools::Failed )
{
    this->::gui::action::IAction::starting();
}

//------------------------------------------------------------------------------

void ExportAcquisition::stopping() throw( ::fwTools::Failed )
{
    this->::gui::action::IAction::stopping();
}

//------------------------------------------------------------------------------

void ExportAcquisition::updating( ) throw(::fwTools::Failed)
{
        SLM_TRACE("ExportAcquisition::updating");

        ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();
        assert( pPatientDB );

        if ( pPatientDB->getPatientSize() > 0 )
        {
                ::fwData::Acquisition::sptr pAcquisition = ::fwComEd::fieldHelper::BackupHelper::getSelectedAcquisition(pPatientDB);
                if(!pAcquisition)
                {
                        wxMessageBox(   _("Sorry, it is impossible to export acquisition. There are not selected patients in the software."),
                                                                        _("Acquisition export"),
                                                                        wxOK | wxICON_WARNING,
                                                                        wxTheApp->GetTopWindow() );
                }
                else
                {
                    ::fwRuntime::ConfigurationElement::sptr ioCfg;
                    ioCfg = ::fwServices::bundle::findConfigurationForPoint( m_ioSelectorSrvConfig , "::uiIO::editor::IOSelectorServiceConfig" ) ;
                    SLM_ASSERT("Sorry, there is not service configuration of type ::uiIO::editor::IOSelectorServiceConfig found", ioCfg ) ;

                        // Init and execute the service
                        ::gui::editor::IEditor::sptr pIOSelectorSrv =
                                        ::fwServices::add< ::gui::editor::IEditor >( pAcquisition, "::uiIO::editor::IOSelectorService" );
                        pIOSelectorSrv->setConfiguration( ioCfg ) ;
                        pIOSelectorSrv->configure() ;
                        pIOSelectorSrv->start();
                        pIOSelectorSrv->update();
                        pIOSelectorSrv->stop();
                        ::fwServices::OSR::unregisterService( pIOSelectorSrv );
                }
        }
        else
        {
                wxMessageBox(   _("Sorry, it is impossible to export acquisition. There are not loaded patients in the software."),
                                                _("Image export"),
                                                wxOK | wxICON_WARNING,
                                                wxTheApp->GetTopWindow() );
        }


}

//------------------------------------------------------------------------------

void ExportAcquisition::updating(::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace basicVRCtrl
