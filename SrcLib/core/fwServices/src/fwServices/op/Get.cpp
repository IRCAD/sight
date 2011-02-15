/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Get.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( ::fwTools::Object::sptr obj, std::string serviceId, unsigned int _index ) throw(fwTools::Failed )
{
    std::vector< ::fwServices::IService::sptr >  proxyServices = ::fwServices::getServices( obj , serviceId );
    if( _index >= proxyServices.size() )
    {
        std::stringstream msg;
        msg << "Requested service not attached to object " << obj->className() << " (adr: " << obj << ")" << std::endl;
        OSLM_TRACE( "Factory : bundle Add service " << serviceId );
        ::fwServices::IService::sptr newService = ::fwServices::add( obj , serviceId ) ;
        if( newService )
        {
            return ::fwServices::get( obj , serviceId , _index ) ;
        }
        SLM_FATAL( msg.str() );
        throw( fwTools::Failed(msg.str()) ) ;
    }
    if( _index < proxyServices.size() )
    {
        assert( proxyServices[_index] ) ;
        return proxyServices[_index];
    }

    return ::fwServices::IService::sptr ();
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( ::fwTools::Object::sptr obj, std::string serviceId, std::string uid ) throw(fwTools::Failed )
{
    ::fwServices::IService::sptr service ;

    std::vector< ::fwServices::IService::sptr >  services = ::fwServices::getServices( obj , serviceId );
    for( std::vector< ::fwServices::IService::sptr >::iterator iter = services.begin() ; iter != services.end() ; ++iter )
    {
        if( ( *iter )->hasID() )
        {
            if( ( *iter )->getID() == uid )
            {
                service = *iter ;
            }
        }
    }
    return service ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( std::string uid ) throw(fwTools::Failed )
{
    OSLM_ASSERT("service not exist with uid "<<uid, ::fwTools::fwID::exist(uid )) ;
    return ::fwServices::IService::dynamicCast( ::fwTools::fwID::getObject( uid ) );
}

//------------------------------------------------------------------------------

std::vector< ::fwServices::IService::sptr > getServices( ::fwTools::Object::sptr obj , std::string serviceId )
{
    std::vector< std::string > availableImplementations =
            ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType( obj->getClassname(), serviceId );

    std::vector< ::fwServices::IService::sptr > allServices = ::fwServices::getRegisteredServices(obj);
    std::vector< ::fwServices::IService::sptr > services ;

    // Search should be optimized
    for( std::vector< ::fwServices::IService::sptr >::iterator iter = allServices.begin() ; iter != allServices.end()  ; ++iter )
    {
        std::string className = (*iter)->getClassname() ;
        if( std::find( availableImplementations.begin() , availableImplementations.end() , className ) != availableImplementations.end()  )
        {
            services.push_back( *iter ) ;
        }
    }
    return services ;
}

//------------------------------------------------------------------------------

std::vector< ::fwServices::IService::sptr > getServices( std::string serviceId )
{
    std::vector< ::fwServices::IService::sptr > services ;
    std::back_insert_iterator< std::vector< ::fwServices::IService::sptr > > backInserter( services ) ;
    std::vector<  ::fwTools::Object::sptr > registeredObjects = ::fwServices::OSR::getObjects();
    std::vector<  ::fwTools::Object::sptr >::iterator objIter = registeredObjects.begin() ;
    for( ; objIter != registeredObjects.end() ; ++objIter )
    {
        ::fwTools::Object::sptr currentObj = *objIter ;
        std::vector< ::fwServices::IService::sptr > servicesForCurrentObject = getServices( currentObj , serviceId ) ;
        std::copy( servicesForCurrentObject.begin() , servicesForCurrentObject.end() , backInserter ) ;
    }

    return services ;
}

//------------------------------------------------------------------------------

std::vector< ::fwServices::IService::sptr > getRegisteredServices( ::fwTools::Object::sptr _obj )
{
    std::vector< ::fwServices::IService::sptr >  lfwServices = OSR::getServices<IService>(_obj);
    return lfwServices;
}

//------------------------------------------------------------------------------

}

