/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/lexical_cast.hpp>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/UUID.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Add.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt )
{
    assert( _elt->getName() == "service" ) ;
    assert( _elt->hasAttribute("type") ) ;

    ::fwServices::IService::sptr service ;
    // Search for service type
    std::string serviceType = _elt->getExistingAttributeValue("type") ;
    // Search for implementation type : use default if none specified
    std::string implementationType                      = (_elt->hasAttribute("implementation") ? _elt->getExistingAttributeValue("implementation") : ::fwServices::getDefaultImplementationIds( obj , serviceType ) );
    // Add service with possible id
    service                                             = ( _elt->hasAttribute("uid")           ? fwServices::add( obj , serviceType , implementationType , _elt->getExistingAttributeValue("uid") ) : fwServices::add( obj , serviceType , implementationType ) ) ;
    assert(service);
    // Search for configuration : inline or offline
    ::fwRuntime::ConfigurationElement::sptr cfg     = ( _elt->hasAttribute("config")        ? ::fwServices::bundle::findConfigurationForPoint( _elt->getExistingAttributeValue("config") , implementationType ) : _elt ) ;
    // Set configuration
    service->setConfiguration( cfg ) ;
    // Configure
    service->configure();

    // Standard communication management
    SLM_ASSERT("autoComChannel attribute missing in service "<< service->getClassname(), _elt->hasAttribute("autoComChannel"));

    std::string autoComChannel = _elt->getExistingAttributeValue("autoComChannel");
    SLM_ASSERT("wrong autoComChannel definition", autoComChannel=="yes" || autoComChannel=="no");
    if(autoComChannel=="yes")
    {
        ::fwServices::ComChannelService::sptr comChannel = ::fwServices::registerCommunicationChannel( obj , service);
        // Add priority for the new comChannel if defined, otherwise the default value is 0.5
        if( _elt->hasAttribute("priority"))
        {
            std::string priorityStr = _elt->getExistingAttributeValue("priority");
            double priority = ::boost::lexical_cast< double >( priorityStr );
            if(priority < 0.0) priority = 0.0;
            if(priority > 1.0) priority = 1.0;
            comChannel->setPriority(priority);
        }
        comChannel->start();
    }


    // Recursive attachment of possibly present subservices
    typedef std::map< std::string , ::fwRuntime::ConfigurationElement::sptr > CfgMapType ;
    for( ::fwRuntime::ConfigurationElementContainer::Iterator iter = cfg->begin() ; iter != cfg->end() ; ++iter )
    {
        if( (*iter)->getName() == "service" )
        {
            ::fwServices::IService::sptr subService = ::fwServices::add( service , (*iter) ) ;
            assert( subService );
        }
    }

    // Return
    return service ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId )
{
    std::vector< std::string > availableImplementations = ::fwServices::getImplementationIds( obj , serviceId ) ;
    assert( !availableImplementations.empty() ) ;
    std::string defaultImplementation = *( availableImplementations.begin() );
    return ::fwServices::add( obj , serviceId , defaultImplementation ) ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId )
{
    // Looking for services provided by components (n implementations)
    if( ::fwServices::bundle::support(obj, serviceId) )
    {
        return ::fwServices::bundle::add(obj, serviceId , _implementationId ) ;
    }

    // Looking for services provided by simple libraries (one implementation)
    if( ::fwServices::library::support(obj, serviceId) )
    {
        return ::fwServices::library::add(obj, serviceId , _implementationId ) ;
    }

    OSLM_WARN( "No service matching the identifier " << serviceId );
    return ::fwServices::IService::sptr() ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId , std::string uid)
{
    ::fwServices::IService::sptr service ;
    // Looking for services provided by components (n implementations)
    if( ::fwServices::bundle::support(obj, serviceId) )
    {
        service = ::fwServices::bundle::add(obj, serviceId , _implementationId ) ;
        assert( !service->hasUUID() );
        service->setUUID( uid ) ;
        return service ;
    }

    // Looking for services provided by simple libraries (one implementation)
    if( ::fwServices::library::support(obj, serviceId) )
    {
        service = ::fwServices::library::add(obj, serviceId , _implementationId ) ;
        assert( !service->hasUUID() );
        service->setUUID( uid ) ;
        return service ;
    }

    OSLM_WARN( "No service matching the identifier " << serviceId );
    return service ;
}


}

