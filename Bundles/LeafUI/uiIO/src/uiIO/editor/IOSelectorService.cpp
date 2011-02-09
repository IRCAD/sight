/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>
#include <sstream>

#include <boost/foreach.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwServices/macros.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <io/IReader.hpp>
#include <io/IWriter.hpp>

#include "uiIO/editor/IOSelectorService.hpp"

namespace uiIO
{

namespace editor
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::editor::IDialogEditor , ::uiIO::editor::IOSelectorService , ::fwTools::Object );

//------------------------------------------------------------------------------

IOSelectorService::IOSelectorService() :
    m_mode                  ( READER_MODE ),
    m_servicesAreExcluded   ( true ),
    m_selectedServices      ( std::vector< std::string >() )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

IOSelectorService::~IOSelectorService()  throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void IOSelectorService::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    bool vectorIsAlreadyCleared = false;

    //  Config Elem
    //  <selection mode="exclude">
    //  <addSelection service="::ioMfo::MfoPatientDBReaderService" />
    //  <addSelection service="::ioMfo::MfoDBPatientDBReaderService" />

    ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin() ;
    for( ; iter != this->m_configuration->end() ; ++iter )
    {
        OSLM_INFO( "IOSelectorService "  << (*iter)->getName());

        if( (*iter)->getName() == "selection" )
        {
            assert( (*iter)->hasAttribute("mode")) ;
            std::string mode = (*iter)->getExistingAttributeValue("mode") ;
            m_servicesAreExcluded = ( mode == "exclude" );
            assert( mode == "exclude" || mode == "include" );
            OSLM_DEBUG( "mode => " << mode );
        }

        if( (*iter)->getName() == "addSelection" )
        {
            if( ! vectorIsAlreadyCleared )
            {
                vectorIsAlreadyCleared = true;
                m_selectedServices.clear();
            }
            assert( (*iter)->hasAttribute("service")) ;
            m_selectedServices.push_back( (*iter)->getExistingAttributeValue("service") ) ;
            OSLM_DEBUG( "add selection => " << (*iter)->getExistingAttributeValue("service") );
        }

        if( (*iter)->getName() == "type" )
        {
            assert( (*iter)->hasAttribute("mode")) ;
            std::string mode = (*iter)->getExistingAttributeValue("mode") ;
            assert( mode == "writer" || mode == "reader" );
            m_mode = ( mode == "writer" ) ? WRITER_MODE : READER_MODE;
            OSLM_DEBUG( "mode => " << mode );
        }

        if( (*iter)->getName() == "config" )
        {
            SLM_ASSERT( "Sorry, xml elemenet <config> must have attribute 'id'.", (*iter)->hasAttribute("id")) ;
            SLM_ASSERT( "Sorry, xml elemenet <config> must have attribute 'service'.", (*iter)->hasAttribute("service")) ;
            std::string configId = (*iter)->getExistingAttributeValue("id") ;
            std::string configSrv = (*iter)->getExistingAttributeValue("service") ;
            m_serviceToConfig[ configSrv ] = configId;
        }
    }

}

//------------------------------------------------------------------------------

void IOSelectorService::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void IOSelectorService::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void IOSelectorService::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Retrieve implementation of type ::io::IReader for this object
    std::vector< std::string > availableExtensionsId;
    if ( m_mode == READER_MODE )
    {
        // Erase all services of type ::io::IReader on the object
        // TODO : comment this line, because must be useless
        ::fwServices::eraseServices< ::io::IReader >( this->getObject() ) ;
        availableExtensionsId = ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromTypeAndObject("::io::IReader", this->getObject()->getClassname()) ;
    }
    else // m_mode == WRITER_MODE
    {
        ::fwServices::eraseServices< ::io::IWriter >( this->getObject() ) ;
        availableExtensionsId = ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromTypeAndObject("::io::IWriter", this->getObject()->getClassname()) ;
    }

    // Filter available extensions and replace id by service description
    std::vector< std::pair < std::string, std::string > > availableExtensionsMap;
    std::vector< std::string > availableExtensionsSelector;

    for(    std::vector< std::string >::iterator itExt = availableExtensionsId.begin();
            itExt < availableExtensionsId.end();
            itExt++ )
    {
        std::string serviceId = *itExt;

        bool serviceIsSelectedByUser = std::find( m_selectedServices.begin(), m_selectedServices.end(), serviceId ) != m_selectedServices.end();

        // Test if the service is considered here as available by users, if yes push in availableExtensionsSelector
        // excluded mode => add services that are not selected by users
        // included mode => add services selected by users
        if( m_servicesAreExcluded && ! serviceIsSelectedByUser ||
            ! m_servicesAreExcluded && serviceIsSelectedByUser )
        {
            // Add this service
            const std::string infoUser = ::fwServices::registry::ServiceFactory::getDefault()->getServiceDescription(serviceId);
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
    if ( ! availableExtensionsMap.empty() )
    {
        std::string extensionId = availableExtensionsMap[0].first ;
        bool extensionSelectionIsCanceled = false;

        // Selection of extension when availableExtensions.size() > 1
        bool extensionIdFound = false;
        if ( availableExtensionsSelector.size() > 1 )
        {
            ::fwGui::dialog::SelectorDialog::NewSptr selector;

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
                for(    std::vector< std::pair < std::string, std::string > >::iterator itExt = availableExtensionsMap.begin();
                        itExt < availableExtensionsMap.end();
                        itExt++ )
                    {
                        if (itExt->second == selection )
                        {
                            extensionId = itExt->first ;
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

        if ( ! extensionSelectionIsCanceled )
        {

            // Get Config
            bool hasConfigForService = false;
            ::fwRuntime::ConfigurationElement::sptr srvCfg;
            if ( m_serviceToConfig.find( extensionId ) != m_serviceToConfig.end() )
            {
                hasConfigForService = true;
                srvCfg = ::fwServices::bundle::findConfigurationForPoint(  m_serviceToConfig[extensionId] , "::fwServices::ServiceConfig" ) ;
                SLM_ASSERT("Sorry, there is not service configuration of type ::fwServices::ServiceConfig found", srvCfg ) ;
            }

            // Configure and start service
            if ( m_mode == READER_MODE )
            {
                ::io::IReader::sptr reader = ::fwServices::add< ::io::IReader >( this->getObject() , extensionId ) ;
                if ( hasConfigForService )
                {
                    reader->setConfiguration( srvCfg );
                    reader->configure();
                }
                reader->start();
                reader->configureWithIHM();

                ::fwGui::Cursor cursor;
                cursor.setCursor(::fwGui::ICursor::BUSY);
                reader->update();
                cursor.setDefaultCursor();

                reader->stop();
            }
            else
            {
                ::io::IWriter::sptr writer = ::fwServices::add< ::io::IWriter >( this->getObject() , extensionId ) ;
                if ( hasConfigForService )
                {
                    writer->setConfiguration( srvCfg );
                    writer->configure();
                }
                writer->start();
                writer->configureWithIHM();

                ::fwGui::Cursor cursor;
                cursor.setCursor(::fwGui::ICursor::BUSY);
                writer->update();
                cursor.setDefaultCursor();

                writer->stop();
            }
        }
    }
    else
    {
        SLM_WARN("IOSelectorService::load : availableExtensions is empty.");
        if ( m_mode == READER_MODE )
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Reader not found");
            messageBox.setMessage( "Sorry, there are not available readers for this data type." );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        else // m_mode == WRITER_MODE
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Writer not found");
            messageBox.setMessage( "Sorry, there are not available writers for this data type." );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }

    // Erase all reader/writer services on this object
    // Todo : replace this line to erase only the used service
    if ( m_mode == READER_MODE )
    {
        ::fwServices::eraseServices< ::io::IReader >( this->getObject() ) ;
    }
    else // m_mode == WRITER_MODE
    {
        ::fwServices::eraseServices< ::io::IWriter >( this->getObject() ) ;
    }
}

//------------------------------------------------------------------------------
void IOSelectorService::updating( fwServices::ObjectMsg::csptr ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void IOSelectorService::info( std::ostream &_sstream )
{
    // Update message
    _sstream << "IOSelectorService";
}

//------------------------------------------------------------------------------

void IOSelectorService::setIOMode( IOMode _mode )
{
    m_mode = _mode;
}

//------------------------------------------------------------------------------

} // namespace editor

} // namespace gui
