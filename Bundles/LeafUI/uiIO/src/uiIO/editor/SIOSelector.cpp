/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiIO/editor/SIOSelector.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <io/IReader.hpp>
#include <io/IWriter.hpp>

#include <sstream>
#include <string>

namespace uiIO
{

namespace editor
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::gui::editor::IDialogEditor, ::uiIO::editor::SIOSelector, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const ::fwCom::Slots::SlotKeyType FORWARD_JOB_SLOT       = "forwardJob";

//------------------------------------------------------------------------------

SIOSelector::SIOSelector() :
    m_mode                  ( READER_MODE ),
    m_servicesAreExcluded   ( true )
{
    m_sigJobCreated  = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
    m_slotForwardJob = newSlot( FORWARD_JOB_SLOT, &SIOSelector::forwardJob, this );
}

//------------------------------------------------------------------------------

SIOSelector::~SIOSelector()  throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SIOSelector::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    bool vectorIsAlreadyCleared = false;

    //  Config Elem
    //  <selection mode="include" />
    //  <addSelection service="::ioAtoms::SWriter" />

    ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin();
    for(; iter != this->m_configuration->end(); ++iter )
    {
        OSLM_INFO( "SIOSelector " << (*iter)->getName());

        if( (*iter)->getName() == "selection" )
        {
            SLM_ASSERT( "The xml element <selection> must have the attribute 'mode'.", (*iter)->hasAttribute("mode"));
            std::string mode = (*iter)->getExistingAttributeValue("mode");
            m_servicesAreExcluded = ( mode == "exclude" );
            SLM_ASSERT( "The xml attribute <mode> must be 'include' (to add the selection to selector list ) or "
                        "'exclude' (to exclude the selection of the selector list).",
                        mode == "exclude" || mode == "include" );
            OSLM_DEBUG( "mode => " << mode );
        }

        if( (*iter)->getName() == "addSelection" )
        {
            if( !vectorIsAlreadyCleared )
            {
                vectorIsAlreadyCleared = true;
                m_selectedServices.clear();
            }
            SLM_ASSERT( "The xml elemenet <addSelection> must have the attribute 'service'.",
                        (*iter)->hasAttribute("service"));
            m_selectedServices.push_back( (*iter)->getExistingAttributeValue("service") );
            OSLM_DEBUG( "add selection => " << (*iter)->getExistingAttributeValue("service") );
        }

        if( (*iter)->getName() == "type" )
        {
            SLM_ASSERT( "The xml elemenet <type> must have the attribute 'mode'.", (*iter)->hasAttribute("mode"));
            std::string mode = (*iter)->getExistingAttributeValue("mode");
            SLM_ASSERT("The xml attribute <mode> must be 'reader' (to open file) or 'writer' (to write a new file).",
                       mode == "writer" || mode == "reader" );
            m_mode = ( mode == "writer" ) ? WRITER_MODE : READER_MODE;
            OSLM_DEBUG( "mode => " << mode );
        }

        if( (*iter)->getName() == "config" )
        {
            SLM_ASSERT( "The xml element <config> must have the attribute 'id'.", (*iter)->hasAttribute("id"));
            SLM_ASSERT( "The xml element <config> must have the attribute 'service'.",
                        (*iter)->hasAttribute("service"));
            std::string configId  = (*iter)->getExistingAttributeValue("id");
            std::string configSrv = (*iter)->getExistingAttributeValue("service");
            m_serviceToConfig[ configSrv ] = configId;
        }

    }

    typedef std::vector < SPTR(::fwRuntime::ConfigurationElement) >  ConfigurationElementContainer;
    ConfigurationElementContainer inject = m_configuration->find("inject");

    if(!inject.empty())
    {
        m_inject = inject.at(0)->getValue();
    }

}

//------------------------------------------------------------------------------

void SIOSelector::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SIOSelector::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SIOSelector::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Retrieve implementation of type ::io::IReader for this object
    std::vector< std::string > availableExtensionsId;
    if ( m_mode == READER_MODE )
    {
        availableExtensionsId =
            ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType(
                this->getObject()->getClassname(),"::io::IReader");
    }
    else // m_mode == WRITER_MODE
    {
        availableExtensionsId =
            ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType(
                this->getObject()->getClassname(),"::io::IWriter");
    }

    // Filter available extensions and replace id by service description
    std::vector< std::pair < std::string, std::string > > availableExtensionsMap;
    std::vector< std::string > availableExtensionsSelector;

    for( const std::string& serviceId :  availableExtensionsId )
    {
        bool serviceIsSelectedByUser =
            std::find( m_selectedServices.begin(), m_selectedServices.end(),
                       serviceId ) != m_selectedServices.end();

        // Test if the service is considered here as available by users, if yes push in availableExtensionsSelector
        // excluded mode => add services that are not selected by users
        // included mode => add services selected by users
        if( (m_servicesAreExcluded && !serviceIsSelectedByUser) ||
            (!m_servicesAreExcluded && serviceIsSelectedByUser) )
        {
            // Add this service
            std::string infoUser =
                ::fwServices::registry::ServiceFactory::getDefault()->getServiceDescription(serviceId);

            std::map< std::string, std::string >::const_iterator iter = m_serviceToConfig.find( serviceId );
            if ( iter != m_serviceToConfig.end() )
            {
                infoUser = ::fwServices::registry::ServiceConfig::getDefault()->getConfigDesc(iter->second);
            }

            if (infoUser != "")
            {
                availableExtensionsMap.push_back( std::pair < std::string, std::string > (serviceId, infoUser) );
                availableExtensionsSelector.push_back( infoUser );
            }
            else
            {
                availableExtensionsMap.push_back( std::pair < std::string, std::string > (serviceId, serviceId) );
                availableExtensionsSelector.push_back( serviceId );
            }
        }
    }

    // Sort available services (lexical string sort)
    std::sort( availableExtensionsSelector.begin(), availableExtensionsSelector.end() );

    // Test if we have an extension
    if ( !availableExtensionsMap.empty() )
    {
        std::string extensionId           = availableExtensionsMap[0].first;
        bool extensionSelectionIsCanceled = false;

        // Selection of extension when availableExtensions.size() > 1
        if ( availableExtensionsSelector.size() > 1 )
        {
            ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();

            if ( m_mode != READER_MODE )
            {
                selector->setTitle("Writer to use");
            }
            else
            {
                selector->setTitle("Reader to use");
            }
            selector->setSelections(availableExtensionsSelector);
            std::string selection = selector->show();
            if( !selection.empty() )
            {
                bool extensionIdFound = false;

                typedef std::pair < std::string, std::string > PairType;
                for(PairType pair :  availableExtensionsMap)
                {
                    if (pair.second == selection )
                    {
                        extensionId      = pair.first;
                        extensionIdFound = true;
                    }
                }
                OSLM_ASSERT("Problem to find the selected string.", extensionIdFound );
            }
            else
            {
                extensionSelectionIsCanceled = true;
            }
        }

        if ( !extensionSelectionIsCanceled )
        {

            // Get Config
            bool hasConfigForService = false;
            ::fwRuntime::ConfigurationElement::csptr srvCfg;
            if ( m_serviceToConfig.find( extensionId ) != m_serviceToConfig.end() )
            {
                hasConfigForService = true;
                srvCfg              = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
                    m_serviceToConfig[extensionId], extensionId );
                SLM_ASSERT(
                    "No service configuration of type ::fwServices::registry::ServiceConfig was found",
                    srvCfg );
            }

            // Configure and start service
            if ( m_mode == READER_MODE )
            {
                ::fwData::Object::sptr object;
                if(m_inject.empty() ||  this->getObject()->getClassname().compare("::fwData::Composite") )
                {
                    object = this->getObject< ::fwData::Object >();
                }
                else
                {
                    ::fwServices::registry::ServiceFactory::sptr services =
                        ::fwServices::registry::ServiceFactory::getDefault();
                    std::string objType = services->getObjectImplementation(extensionId);
                    if(!objType.compare("::fwData::Object"))
                    {
                        object = this->getObject< ::fwData::Composite>();
                    }
                    else
                    {
                        object                              = ::fwData::factory::New(objType);
                        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite>();
                        ::fwDataTools::helper::Composite helper(composite);
                        helper.add(m_inject, object);

                        helper.notify();
                    }
                }

                ::io::IReader::sptr reader;
                if (this->isVersion2())
                {
                    auto factory = ::fwServices::registry::ServiceFactory::getDefault();
                    reader = ::io::IReader::dynamicCast(factory->create( "::io::IReader", extensionId ));
                    ::fwServices::OSR::registerService(object, ::io::s_DATA_KEY,
                                                       ::fwServices::IService::AccessType::INOUT, reader);
                }
                else
                {
                    reader = ::fwServices::add< ::io::IReader >( object, extensionId );
                }
                reader->setWorker(m_associatedWorker);

                if ( hasConfigForService )
                {
                    reader->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(srvCfg) );
                    reader->configure();
                }

                auto jobCreatedSignal = reader->signal("jobCreated");
                if(jobCreatedSignal)
                {
                    jobCreatedSignal->connect(m_slotForwardJob);
                }

                try
                {
                    reader->start();
                    reader->configureWithIHM();

                    ::fwGui::Cursor cursor;
                    cursor.setCursor(::fwGui::ICursor::BUSY);
                    reader->update();
                    cursor.setDefaultCursor();

                    reader->stop();
                    ::fwServices::OSR::unregisterService(reader);
                }
                catch (std::exception& e)
                {
                    std::string msg = "Failed to read : \n" + std::string(e.what());
                    ::fwGui::dialog::MessageDialog::showMessageDialog("Reader Error", msg);
                }
            }
            else
            {
                ::io::IWriter::sptr writer;
                if (this->isVersion2())
                {
                    auto factory = ::fwServices::registry::ServiceFactory::getDefault();
                    writer = ::io::IWriter::dynamicCast(factory->create( "::io::IWriter", extensionId ));
                    ::fwServices::OSR::registerService(this->getObject(), ::io::s_DATA_KEY,
                                                       ::fwServices::IService::AccessType::INPUT, writer);
                }
                else
                {
                    writer = ::fwServices::add< ::io::IWriter >( this->getObject(), extensionId );
                }
                writer->setWorker(m_associatedWorker);

                if ( hasConfigForService )
                {
                    writer->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(srvCfg) );
                    writer->configure();
                }

                auto jobCreatedSignal = writer->signal("jobCreated");
                if(jobCreatedSignal)
                {
                    jobCreatedSignal->connect(m_slotForwardJob);
                }

                try
                {
                    writer->start();
                    writer->configureWithIHM();

                    ::fwGui::Cursor cursor;
                    cursor.setCursor(::fwGui::ICursor::BUSY);
                    writer->update();
                    cursor.setDefaultCursor();

                    writer->stop();
                    ::fwServices::OSR::unregisterService(writer);
                }
                catch (std::exception& e)
                {
                    std::string msg = "Failed to write : \n" +  std::string(e.what());
                    ::fwGui::dialog::MessageDialog::showMessageDialog("Writer Error", msg);
                }
            }
        }
    }
    else
    {
        SLM_WARN("SIOSelector::load : availableExtensions is empty.");
        if ( m_mode == READER_MODE )
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Reader not found");
            messageBox.setMessage( "There are no available readers for this data type." );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        else // m_mode == WRITER_MODE
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Writer not found");
            messageBox.setMessage( "There are no available writers for this data type." );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//------------------------------------------------------------------------------

void SIOSelector::info( std::ostream& _sstream )
{
    // Update message
    _sstream << "SIOSelector";
}

//------------------------------------------------------------------------------

void SIOSelector::setIOMode( IOMode _mode )
{
    m_mode = _mode;
}

//------------------------------------------------------------------------------

void SIOSelector::forwardJob(::fwJobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

//------------------------------------------------------------------------------
} // namespace editor

} // namespace gui
