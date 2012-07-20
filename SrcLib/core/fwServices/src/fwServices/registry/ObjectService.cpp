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

#include "fwServices/IService.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/GlobalEventManager.hpp"

#include "fwServices/registry/ServiceConfig.hpp"
#include "fwServices/registry/ServiceFactory.hpp"
#include "fwServices/registry/AppConfig.hpp"

#include "fwServices/registry/ObjectService.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OSR
{

::fwServices::registry::ObjectService::sptr get()
{
    return ::fwCore::util::LazyInstantiator< ::fwServices::registry::ObjectService >::getInstance();
}

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::ObjectVectorType getObjects()
{
    return ::fwServices::OSR::get()->getObjects();
}

//------------------------------------------------------------------------------

std::string getRegistryInformation()
{
    return ::fwServices::OSR::get()->getRegistryInformation();
}

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::ServiceVectorType getServices( const std::string &serviceType )
{
    return ::fwServices::OSR::get()->getServices(serviceType);
}

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::ServiceVectorType getServices( ::fwData::Object::sptr obj,
                                                                      const std::string &serviceType )
{
    return ::fwServices::OSR::get()->getServices(obj, serviceType);
}

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::ServiceVectorType getServices( ::fwData::Object::sptr obj )
{
    return ::fwServices::OSR::get()->getServices(obj);
}

//------------------------------------------------------------------------------

bool has( ::fwData::Object::sptr obj , const std::string &srvType)
{
    return ::fwServices::OSR::get()->has(obj, srvType);
}

//------------------------------------------------------------------------------

void registerService( ::fwData::Object::sptr obj, ::fwServices::IService::sptr service )
{
    return ::fwServices::OSR::get()->registerService(obj, service);
}

//------------------------------------------------------------------------------

void swapService( ::fwData::Object::sptr objSrc, ::fwData::Object::sptr objDst, ::fwServices::IService::sptr service )
{
    return ::fwServices::OSR::get()->swapService(objSrc, objDst, service);
}

//------------------------------------------------------------------------------

void unregisterService(  ::fwServices::IService::sptr service )
{
    return ::fwServices::OSR::get()->unregisterService(service);
}

} //namespace OSR


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




namespace registry
{

void  ObjectService::registerService( ::fwData::Object::sptr object, ::fwServices::IService::sptr service)
{
    OSLM_ASSERT("Sorry, this service "<< service->getClassname()
                << " is not valid for object " << object->getClassname(),
                ServiceFactory::getDefault()->checkServiceValidity(object->getClassname(), service->getClassname())
                );

    OSLM_ASSERT("Service "<< service->getID()<<" already registered",
            m_container.right.find(service) == m_container.right.end());

    OSLM_ASSERT("Service "<< service->getID()<<" has already a valid associated object" ,
            service->m_associatedObject.expired() || object != service->m_associatedObject.lock());

    service->m_associatedObject = object;
    m_container.insert( ServiceContainerType::value_type( object->getOSRKey()->getLogicStamp(), service ) );
}

//------------------------------------------------------------------------------

void ObjectService::swapService( ::fwData::Object::sptr objSrc,
                                 ::fwData::Object::sptr objDst, ::fwServices::IService::sptr service )
{
    OSLM_ASSERT("Object "<< objSrc->getID()<<" is not registered in OSR",
                m_container.left.find(objSrc->getOSRKey()->getLogicStamp()) != m_container.left.end());

    OSLM_ASSERT("Service "<< service->getID()<<" is not registered in OSR",
                    m_container.right.find(service) != m_container.right.end());

    m_container.right.erase(service);
    this->registerService(objDst, service);
}

//------------------------------------------------------------------------------

void ObjectService::unregisterService( ::fwServices::IService::sptr service )
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT( "Sorry, the service ( "<< service->getID() <<" ) must be stop before unregister it.",
                 service->isStopped() );
    // TODO verify that there are no com channel on this service.
    this->removeFromContainer( service );
}

//------------------------------------------------------------------------------

void ObjectService::removeFromContainer( ::fwServices::IService::sptr service )
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT("Unknown service "<<service->getID()<<" in OSR",
            m_container.right.find(service) != m_container.right.end());
    m_container.right.erase(service);
}

//------------------------------------------------------------------------------

ObjectService::ObjectVectorType ObjectService::getObjects() const
{
    ObjectVectorType lobjects;
    const ServiceContainerType::right_map & right = m_container.right;
    BOOST_FOREACH( const ServiceContainerType::right_map::value_type &elt, right)
    {
        if ( std::find(lobjects.begin(), lobjects.end(), elt.first->getObject()) == lobjects.end() )
        {
            lobjects.push_back( elt.first->getObject() ) ;
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", lobjects.empty() );
    return lobjects;
}

//------------------------------------------------------------------------------

const ObjectService::ServiceContainerType  & ObjectService::getServiceContainer() const
{
    return m_container;
}

//------------------------------------------------------------------------------

std::string ObjectService::getRegistryInformation() const
{
    std::stringstream info;
    ::fwCore::LogicStamp::LogicStampType previousKey = -1;

    BOOST_FOREACH( const ServiceContainerType::left_map::value_type &objSrvMap, m_container.left)
    {
        // TODO FIXME getObject() fail if there are expired object in OSR
        ::fwCore::LogicStamp::LogicStampType key = objSrvMap.first;

        if ( previousKey != key )
        {
            info    << "Object ( key = "<<key<<" ) has "
                    << m_container.left.count(key) <<" services." << std::endl;
            previousKey = key;
        }
        ::fwServices::IService::sptr service = objSrvMap.second;
        info << "    srv : uid = "<< service->getID() <<" , classname = "<< service->getClassname()
                         <<" , service is stopped = "<< ( service->isStopped() ? "yes" : "no" ) << std::endl;
    }
    return info.str();
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices( ::fwData::Object::sptr obj,
                                                             const std::string & serviceType ) const
{
    ServiceVectorType allServices = this->getServices(obj);
    ServiceVectorType services ;

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

ObjectService::ServiceVectorType ObjectService::getServices( const std::string & serviceType ) const
{
    ServiceVectorType services;
    const ServiceContainerType::right_map right = this->getServiceContainer().right;
    BOOST_FOREACH( ServiceContainerType::right_map::value_type elt, right)
    {
        ::fwServices::IService::sptr service = elt.first;
        if ( service->isA(serviceType) )
        {
            services.push_back( service ) ;
        }
    }
    SLM_DEBUG_IF("No service registered", services.empty());
    return services;
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices( ::fwData::Object::sptr obj ) const
{
    ServiceVectorType services;
    if(m_container.left.find(obj->getOSRKey()->getLogicStamp()) != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ServiceContainerType::left_map::const_iterator firstElement = m_container.left.find(key);
        ServiceContainerType::left_map::const_iterator lastElement  = m_container.left.upper_bound(key);
        for (iter = firstElement ; iter != lastElement ; ++iter)
        {
            services.push_back( iter->second ) ;
        }
    }
    return services;
}

//------------------------------------------------------------------------------

bool ObjectService::has( ::fwData::Object::sptr obj , const std::string & srvType) const
{
    bool hasServices = false;
    if( m_container.left.find(obj->getOSRKey()->getLogicStamp()) != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ServiceContainerType::left_map::const_iterator firstElement = m_container.left.find(key);
        ServiceContainerType::left_map::const_iterator lastElement = m_container.left.upper_bound(key);
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

bool ObjectService::hasKey( ::fwCore::LogicStamp::csptr key ) const
{
    return (m_container.left.find(key->getLogicStamp()) != m_container.left.end());
}

//------------------------------------------------------------------------------

} // namespace registry
} // namespace fwServices
