/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Composite.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/Acquisition.hpp>

#include <fwMath/MeshFunctions.hpp>

#include <fwTools/Factory.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwGui/Cursor.hpp>

#include <gui/editor/IDialogEditor.hpp>

#include "uiPatientDB/action/AddPatientDB.hpp"

namespace uiPatientDB
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiPatientDB::action::AddPatientDB , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

AddPatientDB::AddPatientDB( ) throw()
: m_ioSelectorSrvConfig ("IOSelectorServiceConfigVRRenderReader")
{}

//------------------------------------------------------------------------------

AddPatientDB::~AddPatientDB() throw()
{}

//------------------------------------------------------------------------------

void AddPatientDB::info(std::ostream &_sstream )
{
    _sstream << "Action for add Patient" << std::endl;
}

//------------------------------------------------------------------------------

void AddPatientDB::configuring() throw( ::fwTools::Failed )
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

void AddPatientDB::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::LockAction lock(this->getSptr());
    ::fwData::PatientDB::sptr pDocumentPDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pDocumentPDB not instanced", pDocumentPDB);

    // Load a new patient DB
    ::fwData::PatientDB::NewSptr pPDB;

    /// Create IOSelectorService on the new patientDB and execute it.

    // Get the config
    ::fwRuntime::ConfigurationElement::csptr ioCfg;
    ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( m_ioSelectorSrvConfig , "::uiIO::editor::IOSelectorService" ) ;
    SLM_ASSERT("Sorry, there is not service configuration " << m_ioSelectorSrvConfig << " for ::uiIO::editor::IOSelectorService", ioCfg ) ;

    // Init and execute the service
    ::gui::editor::IDialogEditor::sptr pIOSelectorSrv =
            ::fwServices::add< ::gui::editor::IDialogEditor >( pPDB, "::uiIO::editor::IOSelectorService", "IOSelectorServiceReader" );
    pIOSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(ioCfg) ) ;
    pIOSelectorSrv->configure() ;
    pIOSelectorSrv->start();
    pIOSelectorSrv->update();
    pIOSelectorSrv->stop();
    ::fwServices::OSR::unregisterService( pIOSelectorSrv );

    ::fwComEd::fieldHelper::MedicalImageHelpers::mergePatientDBInfo( pPDB, pDocumentPDB, this->getSptr() );
}

//------------------------------------------------------------------------------

void AddPatientDB::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AddPatientDB::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void AddPatientDB::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiPatientDB
