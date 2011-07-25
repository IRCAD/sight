/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

#include "uiPatientDB/action/ExportImage.hpp"

namespace uiPatientDB
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiPatientDB::action::ExportImage , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

ExportImage::ExportImage( ) throw()
: m_ioSelectorSrvConfig ("IOSelectorServiceConfigVRRenderWriter")
{}

//------------------------------------------------------------------------------

ExportImage::~ExportImage() throw()
{}

//------------------------------------------------------------------------------

void ExportImage::info(std::ostream &_sstream )
{
    _sstream << "Action for export Image" << std::endl;
}

//------------------------------------------------------------------------------

void ExportImage::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->::fwGui::IActionSrv::initialize();

    for(    ::fwRuntime::ConfigurationElementContainer::Iterator iter = m_configuration->begin() ;
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

//------------------------------------------------------------------------------

void ExportImage::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE("ExportImage::updating");
    ::fwGui::LockAction lock(this->getSptr());

    ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pPatientDB not instanced", pPatientDB);

    if ( pPatientDB->getPatientSize() > 0 )
    {
        ::fwData::Image::sptr pImage = ::fwComEd::fieldHelper::BackupHelper::getSelectedImage(pPatientDB);
        if(pImage == 0)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Image export",
                    "Sorry, it is impossible to export image. There are not selected patients in the software.",
                    ::fwGui::dialog::IMessageDialog::WARNING);
        }
        else
        {
            /// Create IOSelectorService on the new patientDB and execute it.
            // Get the config
            ::fwRuntime::ConfigurationElement::csptr ioCfg;
            ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( m_ioSelectorSrvConfig , "::uiIO::editor::IOSelectorService" ) ;
            SLM_ASSERT("Sorry, there is not service configuration " << m_ioSelectorSrvConfig << " for ::uiIO::editor::IOSelectorService", ioCfg ) ;

            // Init and execute the service
            ::gui::editor::IDialogEditor::sptr pIOSelectorSrv =
                    ::fwServices::add< ::gui::editor::IDialogEditor >( pImage, "::uiIO::editor::IOSelectorService", "IOSelectorServiceWriter" );
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
                            "Image export",
                            "Sorry, it is impossible to export image. There are not loaded patients in the software.",
                            ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void ExportImage::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ExportImage::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void ExportImage::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiPatientDB
