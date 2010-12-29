/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/lexical_cast.hpp>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/fwID.hpp>
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

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt )
{
    OSLM_ASSERT("ConfigurationElement node name must be \"service\" not "<<_elt->getName(), _elt->getName() == "service" ) ;
    SLM_ASSERT("Attribute \"type\" is missing", _elt->hasAttribute("type") ) ;

    ::fwServices::IService::sptr service ;
    // Search for service type
    std::string serviceType = _elt->getExistingAttributeValue("type") ;
    // Search for implementation type : use default if none specified
    std::string implementationType;
    if(_elt->hasAttribute("implementation") )
    {
        implementationType = _elt->getExistingAttributeValue("implementation");
    }
    else
    {
        SLM_FATAL("ACH => not tolerated  today ?");
        implementationType = ::fwServices::getDefaultImplementationIds( obj , serviceType );
    }
    // Add service with possible id
    if( _elt->hasAttribute("uid")  )
    {
        service = ::fwServices::add( obj , serviceType , implementationType , _elt->getExistingAttributeValue("uid") );
    }
    else
    {
        service =  ::fwServices::add( obj , serviceType , implementationType )  ;
    }
    OSLM_ASSERT("Instantiation service "<<implementationType<<" failed on " << obj->getClassname() << " object", service);

    // Search for configuration : inline or offline
    ::fwRuntime::ConfigurationElement::sptr cfg;
    if( _elt->hasAttribute("config"))
    {
        cfg = ::fwServices::bundle::findConfigurationForPoint( _elt->getExistingAttributeValue("config") , implementationType );
    }
    else
    {
        cfg =  _elt;
    }
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
    OSLM_ASSERT("Unable to add service " <<serviceId<< " on a null object", obj);
    std::vector< std::string > availableImplementations = ::fwServices::getImplementationIds( obj , serviceId ) ;
    OSLM_ASSERT("Any implementation found for "<<serviceId, !availableImplementations.empty() ) ;
    std::string defaultImplementation = *( availableImplementations.begin() );
    return ::fwServices::add( obj , serviceId , defaultImplementation ) ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId )
{

#ifdef USE_SRVFAC
    IService::sptr srv = ::fwServices::ServiceFactoryRegistry::getDefault()->create( serviceId, _implementationId );
    ::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , srv );
    return srv;
#else
    OSLM_ASSERT("Unable to add service " << _implementationId<< " on a null object", obj);
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
#endif
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId , std::string uid)
{
#ifdef USE_SRVFAC
    IService::sptr srv = ::fwServices::add( obj , serviceId , _implementationId ) ;
    assert( !srv->hasID() );
    srv->setID( uid ) ;
    return srv ;
#else
    OSLM_ASSERT("Unable to add service " << _implementationId<< " on a null object", obj);
    ::fwServices::IService::sptr service ;
    // Looking for services provided by components (n implementations)
    if( ::fwServices::bundle::support(obj, serviceId) )
    {
        service = ::fwServices::bundle::add(obj, serviceId , _implementationId ) ;
        assert( !service->hasID() );
        service->setID( uid ) ;
        return service ;
    }

    // Looking for services provided by simple libraries (one implementation)
    if( ::fwServices::library::support(obj, serviceId) )
    {
        service = ::fwServices::library::add(obj, serviceId , _implementationId ) ;
        assert( !service->hasID() );
        service->setID( uid ) ;
        return service ;
    }

    OSLM_WARN( "No service matching the identifier " << serviceId );
    return service ;
#endif
}

//------------------------------------------------------------------------------

}

