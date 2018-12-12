/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

namespace OSR
{

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::sptr get()
{
    return ::fwCore::util::LazyInstantiator< ::fwServices::registry::ObjectService >::getInstance();
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

void registerService( ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->registerService(service);
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

::fwData::Object::csptr getRegistered(const ::fwServices::IService::KeyType& objKey,
                                      ::fwServices::IService::AccessType access, IService::sptr service)
{
    return ::fwServices::OSR::get()->getRegistered(objKey, access, service);
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
    ::fwData::Object::csptr previousObj;
    ::fwCore::mt::ReadLock lock(m_containerMutex);
#ifndef REMOVE_DEPRECATED
    for( const ServiceContainerType::left_map::value_type& objSrvMap :  m_container.left)
    {
        ::fwData::Object::csptr obj = objSrvMap.first.lock();

        if ( obj && previousObj != obj )
        {
            info << "Object ( id = "<<obj->getID()<<" ) has "
                 << m_container.left.count(obj) <<" services." << std::endl;
            previousObj = obj;
        }
        ::fwServices::IService::sptr service = objSrvMap.second;
        info << "    srv : uid = "<< service->getID() <<" , classname = "<< service->getClassname()
             <<" , service is stopped = "<< ( service->isStopped() ? "yes" : "no" ) << std::endl;
    }
#endif
    for (const auto& service : m_services )
    {
        info << "Service : uid = "<< service->getID() <<" , classname = "<< service->getClassname()
             <<" , service is stopped = "<< ( service->isStopped() ? "yes" : "no" ) << std::endl;
        for (const auto& obj : service->m_inputsMap)
        {
            ::fwData::Object::csptr object = obj.second.lock();
            if (object)
            {
                info << "    input: key = " << obj.first << ", classname = " << object->getClassname() << std::endl;
            }
        }
        for (const auto& obj : service->m_inOutsMap)
        {
            ::fwData::Object::sptr object = obj.second.lock();
            if (object)
            {
                info << "    inout: key = " << obj.first << ", classname = " << object->getClassname() << std::endl;
            }
        }
        for (const auto& obj : service->m_outputsMap)
        {
            ::fwData::Object::sptr object = obj.second;
            if (object)
            {
                info << "    output: key = " << obj.first << ", classname = " << object->getClassname() << std::endl;
            }
        }
    }
    return info.str();
}

//------------------------------------------------------------------------------

void ObjectService::registerService( ::fwServices::IService::sptr service )
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);
    m_services.insert(service);
#ifndef REMOVE_DEPRECATED
    m_container.insert( ServiceContainerType::value_type( ::fwData::Object::wptr(), service ) );
#endif
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

    ::fwCore::mt::WriteLock writeLock(m_containerMutex);
    this->internalRegisterService(object, service, objKey, ::fwServices::IService::AccessType::OUTPUT);

    const bool hasID = service->hasObjectId(objKey);
    OSLM_DEBUG_IF("No output is defined for '" + objKey + "', the object is not emitted to the configuration", !hasID);
    if (hasID)
    {
        const auto id = service->getObjectId(objKey);
        auto sig      = this->signal<RegisterSignalType>(s_REGISTERED_SIG);
        sig->asyncEmit(object, id);
    }
}

//------------------------------------------------------------------------------

void ObjectService::unregisterService( ::fwServices::IService::sptr service )
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);

    SLM_ASSERT( "The service ( " + service->getID() + " ) must be stopped before being unregistered.",
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

#ifndef REMOVE_DEPRECATED
    ::fwData::Object::cwptr obj;

    if(access == ::fwServices::IService::AccessType::INPUT)
    {
        obj = service->m_inputsMap[objKey];
    }
    else if(access == ::fwServices::IService::AccessType::INOUT)
    {
        obj = service->m_inOutsMap.at(objKey);
    }
    else
    {
        obj = service->m_outputsMap.at(objKey);
    }

    SLM_ASSERT("Object key '" + objKey + "' not found in service '" + service->getID() + "'", !obj.expired());

    // Remove from the left part
    {
        auto range = m_container.left.equal_range(obj);
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
            if(it->second.lock() == obj.lock())
            {
                it = m_container.right.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
#endif

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
    ::fwData::Object::wptr obj = service->m_outputsMap[objKey];

    if (service->hasObjectId(objKey))
    {
        const auto id = service->getObjectId(objKey);
        auto sig      = this->signal<RegisterSignalType>(s_UNREGISTERED_SIG);
        sig->asyncEmit(obj.lock(), id);
    }
    service->m_outputsMap.erase(objKey);
}

//------------------------------------------------------------------------------

bool ObjectService::isRegistered(const ::fwServices::IService::KeyType& objKey,
                                 ::fwServices::IService::AccessType access, IService::sptr service) const
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

::fwData::Object::csptr ObjectService::getRegistered(const ::fwServices::IService::KeyType& objKey,
                                                     ::fwServices::IService::AccessType access,
                                                     IService::sptr service) const
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);

    if(access == ::fwServices::IService::AccessType::INPUT)
    {
        auto it = service->m_inputsMap.find(objKey);
        if(it != service->m_inputsMap.end())
        {
            return it->second.lock();
        }
    }
    else if(access == ::fwServices::IService::AccessType::INOUT)
    {
        auto it = service->m_inOutsMap.find(objKey);
        if(it != service->m_inOutsMap.end())
        {
            return it->second.lock();
        }
    }
    else
    {
        auto it = service->m_outputsMap.find(objKey);
        if(it != service->m_outputsMap.end())
        {
            return it->second;
        }
    }
    return nullptr;
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
#ifndef REMOVE_DEPRECATED
        OSLM_ASSERT("Service "<< service->getID()<<" has already a valid associated object",
                    service->m_associatedObject.expired() || object != service->m_associatedObject.lock());

        // Old behavior with 1 object -> N Services
        OSLM_ASSERT("This service "<< service->getClassname()
                                   << " is not valid for the object " << object->getClassname(),
                    ServiceFactory::getDefault()->checkServiceValidity(object->getClassname(), service->getClassname())
                    );

        OSLM_ASSERT("Service "<< service->getID()<<" already registered",
                    m_container.right.find(service) == m_container.right.end());
#else
        OSLM_FATAL("object key is not defined");
#endif
        service->m_associatedObject = object;
    }
    else
    {
        if(service->m_inputsMap.empty() && service->m_inOutsMap.empty() && service->m_outputsMap.empty())
        {
            // If we have an appXml but with an old service implementation,
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
#ifndef REMOVE_DEPRECATED
    m_container.insert( ServiceContainerType::value_type( object, service ) );
#endif
    m_services.insert(service);
}

//------------------------------------------------------------------------------

void ObjectService::internalRegisterServiceInput(const fwData::Object::csptr& object, const IService::sptr& service,
                                                 const ::fwServices::IService::KeyType& objKey)
{
    SLM_ASSERT("Can't register a null service in OSR.", service);
    SLM_ASSERT("Can't register a null object in OSR.", object);
    SLM_ASSERT("Can't register an input object without key.", !objKey.empty());

    service->m_inputsMap[objKey] = object;

#ifndef REMOVE_DEPRECATED
    m_container.insert( ServiceContainerType::value_type( object, service ) );
#endif
    m_services.insert(service);
}

//------------------------------------------------------------------------------

void ObjectService::removeFromContainer( ::fwServices::IService::sptr service )
{
    SLM_TRACE_FUNC();
#ifndef REMOVE_DEPRECATED
    OSLM_ASSERT("Unknown service "<<service->getID()<<" in OSR",
                m_container.right.find(service) != m_container.right.end());
    m_container.right.erase(service);
#endif
    auto it = m_services.find(service);
    FW_RAISE_IF("service '" + service->getID() + "' is not found in the OSR", it == m_services.end());
    m_services.erase(it);
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices( const std::string& serviceType ) const
{
    ServiceVectorType services;
    ::fwCore::mt::ReadLock lock(m_containerMutex);
#ifndef REMOVE_DEPRECATED
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
#else
    for (const auto& srv : m_services)
    {
        if (srv->isA(serviceType))
        {
            services.insert( srv );
        }
    }
#endif
    return services;
}
} // namespace registry

#ifndef REMOVE_DEPRECATED

namespace OSR
{

//------------------------------------------------------------------------------

::fwServices::registry::ObjectService::ObjectVectorType getObjects()
{
    return ::fwServices::OSR::get()->getObjects();
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

void swapService( ::fwData::Object::sptr objDst, ::fwServices::IService::sptr service )
{
    ::fwServices::OSR::get()->swapService(objDst, service);
}
} //namespace OSR

namespace registry
{
//------------------------------------------------------------------------------

void ObjectService::registerService( ::fwData::Object::sptr object, ::fwServices::IService::sptr service )
{
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);
    FW_DEPRECATED_MSG("'ObjectService::registerSerice(object, service)' is deprecated. Use "
                      "'ObjectService::registerService(object, key, INOUT, service)' or "
                      "service->registerInOut(object, key)'.", "20.0");
    this->internalRegisterService(object, service, "", ::fwServices::IService::AccessType::INOUT);
}

//------------------------------------------------------------------------------

void ObjectService::swapService( ::fwData::Object::sptr objDst, ::fwServices::IService::sptr service )
{
    FW_DEPRECATED_MSG("'ObjectService::swapService(object, service)' is deprecated. Use "
                      "'service->swapKey(key, object)'.", "20.0");
    ::fwCore::mt::WriteLock lock(m_containerMutex);
    OSLM_ASSERT("Object "<< service->getObject()->getID()<<" is not registered in OSR",
                m_container.left.find(service->getObject()) != m_container.left.end());

    OSLM_ASSERT("Service "<< service->getID()<<" is not registered in OSR",
                m_container.right.find(service) != m_container.right.end());

    // TODO: swap pour chaque objet !
    m_container.right.erase(service);
    this->internalRegisterService(objDst, service, "", ::fwServices::IService::AccessType::INOUT);
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
    FW_DEPRECATED_MSG("'ObjectService::getServices(object, srvType)' is deprecated.", "20.0");

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

ObjectService::ServiceVectorType ObjectService::getServices( ::fwData::Object::sptr obj ) const
{
    FW_DEPRECATED_MSG("'ObjectService::getServices(object)' is deprecated.", "20.0");
    ServiceVectorType services;
    ::fwCore::mt::ReadLock lock(m_containerMutex);

    ServiceContainerType::left_map::const_iterator firstElement = m_container.left.find(obj);
    if(firstElement != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator lastElement = m_container.left.upper_bound(obj);
        for (auto iter = firstElement; iter != lastElement; ++iter)
        {
            services.insert( iter->second );
        }
    }
    return services;
}

//------------------------------------------------------------------------------

bool ObjectService::has( ::fwData::Object::sptr obj, const std::string& srvType) const
{
    FW_DEPRECATED_MSG("'ObjectService::has(object, srvType)' is deprecated.", "20.0");
    bool hasServices = false;
    ::fwCore::mt::ReadLock lock(m_containerMutex);
    ServiceContainerType::left_map::const_iterator firstElement = m_container.left.find(obj);
    if( firstElement != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator lastElement = m_container.left.upper_bound(obj);
        for (auto iter = firstElement; iter != lastElement; ++iter)
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
} // namespace registry
#endif

//------------------------------------------------------------------------------

} // namespace fwServices
