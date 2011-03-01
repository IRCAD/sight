/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <sstream>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <fwCore/LogicStamp.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/fwID.hpp>

#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/GlobalEventManager.hpp"

#include "fwServices/registry/ServiceConfig.hpp"
#include "fwServices/registry/ServiceFactory.hpp"
#include "fwServices/registry/AppConfig.hpp"

namespace fwServices
{
namespace registry
{

//------------------------------------------------------------------------------

ObjectService::sptr ObjectService::m_instance;

//------------------------------------------------------------------------------

const std::string ObjectService::CONFIG_EXTENSION_POINT = "::fwServices::registry::AppConfig";

//------------------------------------------------------------------------------

ObjectService::ObjectService()
{}

//------------------------------------------------------------------------------

ObjectService::~ObjectService()
{
    m_instance.reset() ;
}

//------------------------------------------------------------------------------

ObjectService::sptr ObjectService::getDefault()
{
    if ( m_instance==0 )
    {
        m_instance = ObjectService::sptr(new ObjectService() );
    }
    return m_instance;
}

//------------------------------------------------------------------------------

void  ObjectService::registerService( ::fwTools::Object::sptr object, ::fwServices::IService::sptr service)
{
    OSLM_ASSERT("Sorry, this service "<< service->getClassname() << " is not valid for object " << object->getClassname(),
                ::fwServices::registry::ServiceFactory::getDefault()->checkServiceValidity(object->getClassname(), service->getClassname()));

    OSLM_ASSERT("Service "<< service->getID()<<" already registered",
            getDefault()->m_container.right.find(service) == getDefault()->m_container.right.end());

    OSLM_ASSERT("Service "<< service->getID()<<" has already a valid associated object" ,
            service->m_associatedObject.expired() || object != service->m_associatedObject.lock());

    service->m_associatedObject = object;
    getDefault()->m_container.insert( KSContainer::value_type( object->getOSRKey()->getLogicStamp(), service ) );
}

//------------------------------------------------------------------------------

void ObjectService::swapService(  ::fwTools::Object::sptr  _objSrc, ::fwTools::Object::sptr  _objDst, ::fwServices::IService::sptr _service )
{
    OSLM_ASSERT("Object "<< _objSrc->getID()<<" is not registered in OSR",
                getDefault()->m_container.left.find(_objSrc->getOSRKey()->getLogicStamp()) != getDefault()->m_container.left.end());

    OSLM_ASSERT("Service "<< _service->getID()<<" is not registered in OSR",
                    getDefault()->m_container.right.find(_service) != getDefault()->m_container.right.end());

    getDefault()->m_container.right.erase(_service);
    getDefault()->registerService(_objDst, _service);
}

//------------------------------------------------------------------------------

void ObjectService::unregisterService( ::fwServices::IService::sptr _service )
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT( "Sorry, the service ( "<< _service->getID() <<" ) must be stop before unregister it.", _service->isStopped() );
    // TODO verify that there are no com channel on this service.
    removeFromContainer( _service );
}

//------------------------------------------------------------------------------

void ObjectService::removeFromContainer( ::fwServices::IService::sptr _service )
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT("Unknown service "<<_service->getID()<<" in OSR",
            getDefault()->m_container.right.find(_service) != getDefault()->m_container.right.end());
    getDefault()->m_container.right.erase(_service);
}

//------------------------------------------------------------------------------

std::vector< ::fwTools::Object::sptr > ObjectService::getObjects()
{
    std::vector< ::fwTools::Object::sptr >   lobjects;
    ObjectService::KSContainer::right_map & right = getDefault()->m_container.right;
    BOOST_FOREACH( ObjectService::KSContainer::right_map::value_type elt, right)
    {
        if ( std::find(lobjects.begin(), lobjects.end(), elt.first->getObject()) != lobjects.end() )
        {
            lobjects.push_back( elt.first->getObject() ) ;
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", lobjects.empty() );
    return lobjects;
}

//------------------------------------------------------------------------------

const ObjectService::KSContainer  & ObjectService::getKSContainer()
{
    return getDefault()->m_container;
}

//------------------------------------------------------------------------------

std::string ObjectService::getRegistryInformation()
{
    std::stringstream info;
    BOOST_FOREACH( KSContainer::left_map::value_type objSrvMap, getDefault()->m_container.left)
    {
        // TODO FIXME getObject() failed if there are expired object in OSR
        //::fwTools::Object::sptr obj = objSrvMap.second->getObject();
        ::fwCore::LogicStamp::LogicStampType key = objSrvMap.first;
        info << "New object found in OSR" << std::endl;
        //info << "Object ( uid = "<< obj->getID() <<" , classname = "<< obj->getClassname() <<" ) has "
        info << "Object ( key = "<<key<<" ) has "
                << getDefault()->m_container.left.count(key) <<" services." << std::endl;

        ::fwServices::IService::sptr service = objSrvMap.second;
        info << "    srv : uid = "<< service->getID() <<" , classname = "<< service->getClassname()
                         <<" , service is stopped = "<< ( service->isStopped() ? "yes" : "no" ) << std::endl;
    }
    return info.str();
}

//------------------------------------------------------------------------------

std::vector< ::fwServices::IService::sptr > ObjectService::getServices( ::fwTools::Object::sptr obj , std::string serviceType )
{
    std::vector< ::fwServices::IService::sptr > allServices = ::fwServices::OSR::getServices(obj);
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

std::vector< ::fwServices::IService::sptr > ObjectService::getServices( std::string serviceType )
{
    std::vector< ::fwServices::IService::sptr >  lfwServices;
    ::fwServices::OSR::KSContainer::right_map right = ::fwServices::OSR::getKSContainer().right;
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

std::vector< ::fwServices::IService::sptr > ObjectService::getServices( ::fwTools::Object::sptr obj )
{
    std::vector< ::fwServices::IService::sptr >  lfwServices;
    if(getDefault()->m_container.left.find(obj->getOSRKey()->getLogicStamp()) != getDefault()->m_container.left.end())
    {
        ObjectService::KSContainer::left_map::iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ObjectService::KSContainer::left_map::iterator firstElement = getDefault()->m_container.left.find(key);
        ObjectService::KSContainer::left_map::iterator lastElement = getDefault()->m_container.left.upper_bound(key);
        for (iter = firstElement ; iter != lastElement ; ++iter)
        {
            lfwServices.push_back( iter->second ) ;
        }
    }
    return lfwServices;
}

//------------------------------------------------------------------------------

bool ObjectService::has( ::fwTools::Object::sptr obj , const std::string & srvType)
{
    bool hasServices = false;
    if( ObjectService::getDefault()->m_container.left.find(obj->getOSRKey()->getLogicStamp()) != ObjectService::getDefault()->m_container.left.end())
    {
        ObjectService::KSContainer::left_map::iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ObjectService::KSContainer::left_map::iterator firstElement = getDefault()->m_container.left.find(key);
        ObjectService::KSContainer::left_map::iterator lastElement = getDefault()->m_container.left.upper_bound(key);
        for (iter = firstElement ; iter != lastElement ; ++iter)
        {
            if( iter->second->isA(srvType))
            {
                hasServices = true;
                break;
            }
        }
    }
    return hasServices ;
}

//------------------------------------------------------------------------------

bool ObjectService::hasKey( ::fwCore::LogicStamp::csptr key )
{
    return (ObjectService::getDefault()->m_container.left.find(key->getLogicStamp()) != ObjectService::getDefault()->m_container.left.end());
}

//------------------------------------------------------------------------------

} // namespace registry
} // namespace fwServices
