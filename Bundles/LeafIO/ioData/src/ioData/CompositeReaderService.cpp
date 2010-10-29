/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>

#include <io/IReader.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwData/Composite.hpp>
#include <fwCore/base.hpp>

#include "ioData/CompositeReaderService.hpp"

namespace ioData
{

REGISTER_SERVICE( ::io::IReader , ::ioData::CompositeReaderService , ::fwData::Composite ) ;


CompositeReaderService::CompositeReaderService()
{
}

//-----------------------------------------------------------------------------

void CompositeReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " composite object reader" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > CompositeReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back("*");
    return extensions ;
}

//-----------------------------------------------------------------------------

void CompositeReaderService::starting( ) throw(::fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;
    assert( composite ) ;
    // For each input, one tries to start the io service.
    // For configured input io service, the configuration is applied
    for(::fwData::Composite::ContainerType::iterator iter = composite->begin() ; iter != composite->end() ; ++iter )
    {
        // Only if io is supported by current input
        if( fwServices::has< ::io::IReader >( iter->second ) )
        {
            fwServices::get< ::io::IReader >( iter->second )->start();
        }
    }
}

//-----------------------------------------------------------------------------

CompositeReaderService::~CompositeReaderService() throw()
{
}

//-----------------------------------------------------------------------------

void CompositeReaderService::configuring( ) throw(::fwTools::Failed)
{
    OSLM_INFO( "CompositeReaderService::configure : " << *m_configuration );
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;
    ::fwRuntime::ConfigurationElementContainer::Iterator iter ;
    for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
    {
        OSLM_INFO( "CompositeReaderService "  << (*iter)->getName());
        if( (*iter)->getName() == "input" )
        {
            assert( (*iter)->hasAttribute("id")) ;
            ::fwData::Object::sptr obj = composite->getRefMap()[(*iter)->getExistingAttributeValue("id")] ;
            assert( obj ) ;
            // Finding out the specified IReader implementation to attach to input
            ::fwRuntime::ConfigurationElement::sptr implementation = (*iter)->findConfigurationElement( "service" ) ;
            assert( implementation ) ;
            assert( implementation->hasAttribute("type")) ;
            std::string implementationId = implementation->getExistingAttributeValue("type") ;
            ::io::IReader::sptr srv = ::fwServices::add< ::io::IReader >( obj , implementationId ) ;
            assert( srv ) ;
            // Finding its configuration
            if( implementation->hasAttribute("config"))
            {
                std::string configId = implementation->getExistingAttributeValue("config") ;
                ::fwRuntime::ConfigurationElement::sptr cfg = ::fwServices::bundle::findConfigurationForPoint( configId , implementationId ) ;
                srv->setConfiguration( cfg ) ;
            }
            else
            {
                srv->setConfiguration( implementation ) ;
            }
            if(implementation->hasAttribute("uid"))
            {
                std::string uid = implementation->getExistingAttributeValue("uid") ;
                srv->setID(uid);
            }

            srv->configure() ;
        }
    }
}

//-----------------------------------------------------------------------------

void CompositeReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void CompositeReaderService::updating() throw(::fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;
    assert( composite ) ;
    // For each input, one tries to start the io service
    // For configured input io service, the configuration is applied
    for(::fwData::Composite::ContainerType::iterator iter = composite->begin() ; iter != composite->end() ; ++iter )
    {
        if( fwServices::has< ::io::IReader >( iter->second ) )
        {
            fwServices::get< ::io::IReader >(iter->second )->update() ;
        }
    }
}

}
