/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/registry/ObjectService.hpp"

#include "fwServices/IService.hpp"
#include "fwServices/registry/AppConfig.hpp"
#include "fwServices/registry/ServiceConfig.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

#include <fwCore/LogicStamp.hpp>
#include <fwCore/util/LazyInstantiator.hpp>

#include <fwTools/fwID.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <sstream>
#include <utility>

namespace fwServices
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OSR
{

//------------------------------------------------------------------------------

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

::fwServices::registry::ObjectService::ServiceVectorType getServices( const std::string& serviceType )
{
    return ::fwServices::OSR::get()->getServices(serviceType);
}

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::ServiceVectorType getServices( ::fwData::Object::sptr obj,
                                                                      const std::string& serviceType )
{
    return ::fwServices::OSR::get()->getServices(obj, serviceType);
}

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::ServiceVectorType getServices( ::fwData::Object::sptr obj )
{
    return ::fwServices::OSR::get()->getServices(obj);
}

//------------------------------------------------------------------------------

bool has( ::fwData::Object::sptr obj, const std::string& srvType)
{
    return ::fwServices::OSR::get()->has(obj, srvType);
}

//------------------------------------------------------------------------------

void registerService( ::fwData::Object::sptr obj, ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->registerService(obj, service);
}

//------------------------------------------------------------------------------

void registerService( ::fwData::Object::sptr obj, const ::fwServices::IService::KeyType& objKey,
                      ::fwServices::IService::AccessType access, ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->registerService(obj, objKey, access, service);
}

//------------------------------------------------------------------------------

void registerServiceInput( ::fwData::Object::csptr obj, const ::fwServices::IService::KeyType& objKey,
                           ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->registerServiceInput(obj, objKey, service);
}

//------------------------------------------------------------------------------

void registerServiceOutput( ::fwData::Object::sptr obj, const ::fwServices::IService::KeyType& objKey,
                            ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->registerServiceOutput(obj, objKey, service);
}

//------------------------------------------------------------------------------

void swapService( ::fwData::Object::sptr objDst, ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->swapService(objDst, service);
}

//------------------------------------------------------------------------------

void unregisterService(  ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->unregisterService(service);
}

//------------------------------------------------------------------------------

void unregisterService(const ::fwServices::IService::KeyType& objKey, ::fwServices::IService::AccessType access,
                       IService::sptr service)
{
    ::fwServices::OSR::get()->unregisterService(objKey, access, service);
}

//------------------------------------------------------------------------------

void unregisterServiceOutput(const ::fwServices::IService::KeyType& objKey,  IService::sptr service)
{
    ::fwServices::OSR::get()->unregisterServiceOutput(objKey, service);
}

//------------------------------------------------------------------------------

bool isRegistered(const ::fwServices::IService::KeyType& objKey,
                  ::fwServices::IService::AccessType access, IService::sptr service)
{
    return ::fwServices::OSR::get()->isRegistered(objKey, access, service);
}

//------------------------------------------------------------------------------

} //namespace OSR

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace registry
{

const ::fwCom::Signals::SignalKeyType ObjectService::s_REGISTERED_SIG   = "registered";
const ::fwCom::Signals::SignalKeyType ObjectService::s_UNREGISTERED_SIG = "unregistered";

//------------------------------------------------------------------------------

ObjectService::ObjectService()
{
    newSignal<RegisterSignalType>(s_REGISTERED_SIG);
    newSignal<RegisterSignalType>(s_UNREGISTERED_SIG);
}

//------------------------------------------------------------------------------

std::string ObjectService::getRegistryInformation() const
{
    std::stringstream info;
    ::fwCore::LogicStamp::LogicStampType previousKey = ~0ul;
    ::fwCore::mt::ReadLock lock(m_containerMutex);
    for( const ServiceContainerType::left_map::value_type& objSrvMap :  m_container.left)
    {
        // TODO FIXME getObject() fail if there are expired object in OSR
        ::fwCore::LogicStamp::LogicStampType key = objSrvMap.first;

        if ( previousKey != key )
        {
            info << "Object ( key = "<<key<<" ) has "
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

void ObjectService::registerService( ::fwData::Object::sptr object, ::fwServices::IService::sptr service )
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);
    this->internalRegisterService(object, service, "", ::fwServices::IService::AccessType::INOUT);
}

//------------------------------------------------------------------------------

void ObjectService::registerService( ::fwData::Object::sptr object, const ::fwServices::IService::KeyType& objKey,
                                     ::fwServices::IService::AccessType access, ::fwServices::IService::sptr service)
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);
    this->internalRegisterService(object, service, objKey, access);

}

//------------------------------------------------------------------------------

void ObjectService::registerServiceInput( const ::fwData::Object::csptr& object,
                                          const ::fwServices::IService::KeyType& objKey,
                                          const ::fwServices::IService::sptr& service)
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);
    this->internalRegisterServiceInput(object, service, objKey);

}

//------------------------------------------------------------------------------

void ObjectService::registerServiceOutput(::fwData::Object::sptr object, const ::fwServices::IService::KeyType& objKey,
                                          ::fwServices::IService::sptr service)
{
    const auto id = service->getObjectId(objKey);

    this->signal<RegisterSignalType>(s_REGISTERED_SIG)->asyncEmit(object, id);
}

//------------------------------------------------------------------------------

void ObjectService::swapService( ::fwData::Object::sptr objDst, ::fwServices::IService::sptr service )
{
    ::fwCore::mt::WriteLock lock(m_containerMutex);
    OSLM_ASSERT("Object "<< service->getObject()->getID()<<" is not registered in OSR",
                m_container.left.find(service->getObject()->getOSRKey()->getLogicStamp()) != m_container.left.end());

    OSLM_ASSERT("Service "<< service->getID()<<" is not registered in OSR",
                m_container.right.find(service) != m_container.right.end());

    // TODO: swap pour chaque objet !
    m_container.right.erase(service);
    this->internalRegisterService(objDst, service, "", ::fwServices::IService::AccessType::INOUT);
}

//------------------------------------------------------------------------------

void ObjectService::unregisterService( ::fwServices::IService::sptr service )
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);

    SLM_ASSERT( "The service ( " + service->getID() + " ) must be stop before being unregistered.",
                service->isStopped() );

    this->removeFromContainer( service );
    service->m_inputsMap.clear();
    service->m_inOutsMap.clear();
    service->m_outputsMap.clear();
}

//------------------------------------------------------------------------------

void ObjectService::unregisterService(const ::fwServices::IService::KeyType& objKey,
                                      ::fwServices::IService::AccessType access,
                                      ::fwServices::IService::sptr service )
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);

    ::fwData::Object::cwptr obj;

    if(access == ::fwServices::IService::AccessType::INPUT)
    {
        obj = service->m_inputsMap[objKey];
    }
    else if(access == ::fwServices::IService::AccessType::INOUT)
    {
        obj = service->m_inOutsMap[objKey];
    }
    else
    {
        obj = service->m_outputsMap[objKey];
    }

    SLM_ASSERT("Object key '" + objKey + "' not found in service '" + service->getID() + "'", !obj.expired());

    // Remove from the left part
    {
        auto range = m_container.left.equal_range(obj.lock()->getOSRKey()->getLogicStamp());
        for(auto it = range.first; it != range.second; )
        {
            if(it->second == service)
            {
                it = m_container.left.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // Remove from the right part
    {
        auto range = m_container.right.equal_range(service);
        for(auto it = range.first; it != range.second; )
        {
            if(it->second == obj.lock()->getOSRKey()->getLogicStamp())
            {
                it = m_container.right.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    if(access == ::fwServices::IService::AccessType::INPUT)
    {
        service->m_inputsMap.erase(objKey);
    }
    else if(access == ::fwServices::IService::AccessType::INOUT)
    {
        service->m_inOutsMap.erase(objKey);
    }
    else
    {
        service->m_outputsMap.erase(objKey);
    }
}

//------------------------------------------------------------------------------

void ObjectService::unregisterServiceOutput( const ::fwServices::IService::KeyType& objKey,
                                             ::fwServices::IService::sptr service )
{
    const auto id = service->getObjectId(objKey);
    ::fwData::Object::wptr obj = service->m_outputsMap[objKey];

    this->signal<RegisterSignalType>(s_UNREGISTERED_SIG)->asyncEmit(obj.lock(), id);
}

//------------------------------------------------------------------------------

bool ObjectService::isRegistered(const ::fwServices::IService::KeyType& objKey,
                                 ::fwServices::IService::AccessType access, IService::sptr service)
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);

    if(access == ::fwServices::IService::AccessType::INPUT)
    {
        return service->m_inputsMap.find(objKey) != service->m_inputsMap.end();
    }
    else if(access == ::fwServices::IService::AccessType::INOUT)
    {
        return service->m_inOutsMap.find(objKey) != service->m_inOutsMap.end();
    }
    else
    {
        return service->m_outputsMap.find(objKey) != service->m_outputsMap.end();
    }
}

//------------------------------------------------------------------------------

void ObjectService::internalRegisterService(::fwData::Object::sptr object, ::fwServices::IService::sptr service,
                                            const ::fwServices::IService::KeyType& objKey,
                                            ::fwServices::IService::AccessType access)
{
    SLM_ASSERT("Can't register a null service in OSR.", service);
    SLM_ASSERT("Can't register a null object in OSR.", object);

    if(objKey.empty())
    {
        OSLM_ASSERT("Service "<< service->getID()<<" has already a valid associated object",
                    service->m_associatedObject.expired() || object != service->m_associatedObject.lock());

        // Old behavior with 1 object -> N Services
        OSLM_ASSERT("This service "<< service->getClassname()
                                   << " is not valid for the object " << object->getClassname(),
                    ServiceFactory::getDefault()->checkServiceValidity(object->getClassname(), service->getClassname())
                    );

        OSLM_ASSERT("Service "<< service->getID()<<" already registered",
                    m_container.right.find(service) == m_container.right.end());

        service->m_associatedObject = object;
    }
    else
    {
        if(service->m_inputsMap.empty() && service->m_inOutsMap.empty() && service->m_outputsMap.empty())
        {
            // If we have an appXml2 but with an old service implementation,
            // we consider that the primary object is the first one we added
            service->m_associatedObject = object;
        }
        // new behavior with N objects -> N Services
        if(access == ::fwServices::IService::AccessType::INPUT)
        {
            service->m_inputsMap[objKey] = object;
        }
        else if(access == ::fwServices::IService::AccessType::INOUT)
        {
            service->m_inOutsMap[objKey] = object;
        }
        else
        {
            service->m_outputsMap[objKey] = object;
        }
    }
    m_container.insert( ServiceContainerType::value_type( object->getOSRKey()->getLogicStamp(), service ) );
}

//------------------------------------------------------------------------------

void ObjectService::internalRegisterServiceInput(const fwData::Object::csptr& object, const IService::sptr& service,
                                                 const ::fwServices::IService::KeyType& objKey)
{
    SLM_ASSERT("Can't register a null service in OSR.", service);
    SLM_ASSERT("Can't register a null object in OSR.", object);
    SLM_ASSERT("Can't register an input object without key.", !objKey.empty());

    service->m_inputsMap[objKey] = object;

    m_container.insert( ServiceContainerType::value_type( object->getOSRKey()->getLogicStamp(), service ) );
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
    ObjectVectorType objects;
    ::fwCore::mt::ReadLock lock(m_containerMutex);

    for( const ServiceContainerType::right_map::value_type& elt : m_container.right)
    {
        auto srvObjects = elt.first->getObjects();
        std::move(srvObjects.begin(), srvObjects.end(), std::inserter(objects, objects.begin()));
    }
    SLM_WARN_IF( "No object registered for the requested type of service", objects.empty() );
    return objects;
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices( ::fwData::Object::sptr obj,
                                                             const std::string& serviceType ) const
{
    ServiceVectorType allServices = this->getServices(obj);
    ServiceVectorType services;

    // Search should be optimized
    for(::fwServices::IService::sptr srv :  allServices)
    {
        if( srv->isA(serviceType) )
        {
            services.insert( srv );
        }
    }
    return services;
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices( const std::string& serviceType ) const
{
    ServiceVectorType services;
    ::fwCore::mt::ReadLock lock(m_containerMutex);
    const ServiceContainerType::right_map right = m_container.right;
    for( ServiceContainerType::right_map::value_type elt :  right)
    {
        ::fwServices::IService::sptr service = elt.first;
        if ( service->isA(serviceType) )
        {
            services.insert( service );
        }
    }
    SLM_DEBUG_IF("No service registered", services.empty());
    return services;
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices( ::fwData::Object::sptr obj ) const
{
    ServiceVectorType services;
    ::fwCore::mt::ReadLock lock(m_containerMutex);
    if(m_container.left.find(obj->getOSRKey()->getLogicStamp()) != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ServiceContainerType::left_map::const_iterator firstElement = m_container.left.find(key);
        ServiceContainerType::left_map::const_iterator lastElement  = m_container.left.upper_bound(key);
        for (iter = firstElement; iter != lastElement; ++iter)
        {
            services.insert( iter->second );
        }
    }
    return services;
}

//------------------------------------------------------------------------------

bool ObjectService::has( ::fwData::Object::sptr obj, const std::string& srvType) const
{
    bool hasServices = false;
    ::fwCore::mt::ReadLock lock(m_containerMutex);
    if( m_container.left.find(obj->getOSRKey()->getLogicStamp()) != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ServiceContainerType::left_map::const_iterator firstElement = m_container.left.find(key);
        ServiceContainerType::left_map::const_iterator lastElement  = m_container.left.upper_bound(key);
        for (iter = firstElement; iter != lastElement; ++iter)
        {
            if( iter->second->isA(srvType))
            {
                hasServices = true;
                break;
            }
        }
    }
    return hasServices;
}

//------------------------------------------------------------------------------

bool ObjectService::hasKey( ::fwCore::LogicStamp::csptr key ) const
{
    ::fwCore::mt::ReadLock lock(m_containerMutex);
    return (m_container.left.find(key->getLogicStamp()) != m_container.left.end());
}

//------------------------------------------------------------------------------

} // namespace registry
} // namespace fwServices
