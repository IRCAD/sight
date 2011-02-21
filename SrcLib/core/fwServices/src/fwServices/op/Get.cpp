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

::fwServices::IService::sptr get( ::fwTools::Object::sptr obj, std::string serviceType ) throw(fwTools::Failed )
{
    ::fwServices::IService::sptr service;
    std::vector< ::fwServices::IService::sptr >  services = ::fwServices::getServices( obj , serviceType );
    OSLM_ASSERT("Service "<<serviceType<<" not unique, registered "<<services.size()<<" time", services.size() == 1);
    return services.at(0) ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr get( std::string uid ) throw(fwTools::Failed )
{
    OSLM_ASSERT("service not exist with uid "<<uid, ::fwTools::fwID::exist(uid )) ;
    return ::fwServices::IService::dynamicCast( ::fwTools::fwID::getObject( uid ) );
}

//------------------------------------------------------------------------------

std::vector< ::fwServices::IService::sptr > getServices( ::fwTools::Object::sptr obj , std::string serviceType )
{
    std::vector< ::fwServices::IService::sptr > allServices = ::fwServices::getRegisteredServices(obj);
    std::vector< ::fwServices::IService::sptr > services ;

    // Search should be optimized
    BOOST_FOREACH(::fwServices::IService::sptr srv, allServices)
    {
        if( srv->isA(serviceType) )
        {
            services.push_back( srv ) ;
        }
    }
    return services ;
}

//------------------------------------------------------------------------------

std::vector< ::fwServices::IService::sptr > getServices( std::string serviceType )
{
    std::vector< ::fwServices::IService::sptr >  lfwServices;
    ::fwServices::OSR::KSContainer::right_map right = ::fwServices::OSR::getDefault()->getKSContainer().right;
    BOOST_FOREACH( ::fwServices::OSR::KSContainer::right_map::value_type elt, right)
    {
        ::fwServices::IService::sptr service = elt.first;
        if ( service->isA(serviceType) )
        {
            lfwServices.push_back( service ) ;
        }
    }
    SLM_DEBUG_IF("No service registered", lfwServices.empty());
    return lfwServices;
}

//------------------------------------------------------------------------------

std::vector< ::fwServices::IService::sptr > getRegisteredServices( ::fwTools::Object::sptr _obj )
{
    std::vector< ::fwServices::IService::sptr >  lfwServices = OSR::getServices<IService>(_obj);
    return lfwServices;
}

//------------------------------------------------------------------------------

}

