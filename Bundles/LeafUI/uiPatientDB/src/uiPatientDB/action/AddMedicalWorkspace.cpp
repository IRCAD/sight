/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Composite.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/Acquisition.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/helper/Composite.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <gui/editor/IDialogEditor.hpp>

#include "uiPatientDB/action/AddMedicalWorkspace.hpp"
#include "uiPatientDB/action/AddPatientDB.hpp"

namespace uiPatientDB
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::uiPatientDB::action::AddMedicalWorkspace , ::fwData::Composite ) ;

//------------------------------------------------------------------------------

AddMedicalWorkspace::AddMedicalWorkspace( ) throw()
: m_ioSelectorSrvConfig ("MwzIOSelectorReaderConfig")
{}

//------------------------------------------------------------------------------

AddMedicalWorkspace::~AddMedicalWorkspace() throw()
{}

//------------------------------------------------------------------------------

void AddMedicalWorkspace::configuring() throw( ::fwTools::Failed )
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

void AddMedicalWorkspace::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::LockAction lock(this->getSptr());

    // Get current medical workspace
    ::fwData::Composite::sptr medicalWorkspace = this->getObject< ::fwData::Composite >();

    /// Create IOSelectorService on the new medicalWorkspace and execute it.

    // Create composite
    ::fwData::Composite::sptr loadedMedicalWorkspace = ::fwData::Composite::New();

    // Get the config
    ::fwRuntime::ConfigurationElement::csptr ioCfg;
    ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( m_ioSelectorSrvConfig , "::uiIO::editor::IOSelectorService" ) ;
    SLM_ASSERT("Sorry, there is not service configuration " << m_ioSelectorSrvConfig << " for ::uiIO::editor::IOSelectorService", ioCfg ) ;

    // Init and execute the service
    ::gui::editor::IDialogEditor::sptr pIOSelectorSrv =
            ::fwServices::add< ::gui::editor::IDialogEditor >( loadedMedicalWorkspace, "::uiIO::editor::IOSelectorService" );
    pIOSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(ioCfg) ) ;
    pIOSelectorSrv->configure() ;
    pIOSelectorSrv->start();
    pIOSelectorSrv->update();
    pIOSelectorSrv->stop();
    ::fwServices::OSR::unregisterService( pIOSelectorSrv );

    if ( ! loadedMedicalWorkspace->empty() )
    {
        /// Merge information
        ::fwData::PatientDB::sptr patientDBTo = ::fwData::PatientDB::dynamicCast( (*medicalWorkspace)["patientDB"] );
        SLM_ASSERT("Sorry, key patientDB no correspond to a fwData::PatientDB", patientDBTo );
        ::fwData::PatientDB::sptr patientDBFrom =  ::fwData::PatientDB::dynamicCast( (*loadedMedicalWorkspace)["patientDB"] );
        SLM_ASSERT("Sorry, key patientDB (loadedMedicalWorkspace) no correspond to a fwData::PatientDB", patientDBFrom );
        ::fwComEd::fieldHelper::MedicalImageHelpers::mergePatientDBInfo( patientDBFrom, patientDBTo, this->getSptr() );

        ::fwData::Composite::sptr planningDBTo = ::fwData::Composite::dynamicCast( (*medicalWorkspace)["planningDB"] );
        SLM_ASSERT("Sorry, key planningDB no correspond to a fwData::Composite", planningDBTo );
        ::fwData::Composite::sptr planningDBFrom =  ::fwData::Composite::dynamicCast( (*loadedMedicalWorkspace)["planningDB"] );
        SLM_ASSERT("Sorry, key planningDB (loadedMedicalWorkspace) no correspond to a fwData::Composite", planningDBFrom );
        mergePlanningDBInfo( planningDBFrom, planningDBTo, this->getSptr() );

        ::fwData::Composite::sptr processingDBTo = ::fwData::Composite::dynamicCast( (*medicalWorkspace)["processingDB"] );
        SLM_ASSERT("Sorry, key processingDB no correspond to a fwData::Composite", processingDBTo );
        ::fwData::Composite::sptr processingDBFrom =  ::fwData::Composite::dynamicCast( (*loadedMedicalWorkspace)["processingDB"] );
        SLM_ASSERT("Sorry, key processingDB (loadedMedicalWorkspace) no correspond to a fwData::Composite", processingDBFrom );
        mergeProcessingDBInfo( processingDBFrom, processingDBTo, this->getSptr() );
    }
}


//------------------------------------------------------------------------------

void AddMedicalWorkspace::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AddMedicalWorkspace::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void AddMedicalWorkspace::mergePlanningDBInfo( ::fwData::Composite::sptr _planningDBFrom, ::fwData::Composite::sptr _planningDBTo, ::fwServices::IService::sptr _msgSender )
{
    // Merges info
    ::fwComEd::helper::Composite helperComposite ( _planningDBTo );
    BOOST_FOREACH( ::fwData::Composite::value_type val, (*_planningDBFrom) )
    {
        if ( _planningDBTo->find( val.first ) == _planningDBTo->end() )
        {
            helperComposite.add( val.first, val.second );
        }
        else
        {
            std::stringstream ss;
            ss << "Warning during loading. Planning key = " <<  val.first << " already exist. Would you overwrite it ?";
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::YES_NO);
            ::fwGui::dialog::IMessageDialog::Buttons button = messageBox.show();

            if (button == ::fwGui::dialog::IMessageDialog::YES)
            {
                helperComposite.swap( val.first, val.second );
            }
        }
    }

    // Notifies msg
    helperComposite.notify( _msgSender );
}

//------------------------------------------------------------------------------

void AddMedicalWorkspace::mergeProcessingDBInfo( ::fwData::Composite::sptr _processingDBFrom, ::fwData::Composite::sptr _processingDBTo, ::fwServices::IService::sptr _msgSender )
{
    // Merges info
    ::fwComEd::helper::Composite helperComposite ( _processingDBTo );
    BOOST_FOREACH( ::fwData::Composite::value_type val, (*_processingDBFrom) )
    {
        if ( _processingDBTo->find( val.first ) == _processingDBTo->end() )
        {
            helperComposite.add( val.first, val.second );
        }
        else
        {
            std::stringstream ss;
            ss << "Warning during loading. Processing key = " <<  val.first << " already exist. Would you overwrite it ?";
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::YES_NO);
            ::fwGui::dialog::IMessageDialog::Buttons button = messageBox.show();

            if (button == ::fwGui::dialog::IMessageDialog::YES)
            {
                helperComposite.swap( val.first, val.second );
            }
        }
    }

    // Notifies msg
    helperComposite.notify( _msgSender );
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiPatientDB
