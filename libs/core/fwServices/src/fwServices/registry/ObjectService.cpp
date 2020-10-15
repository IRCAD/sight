/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <filesystem>
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

    for (const auto& service : m_services )
    {
        info << "Service : uid = "<< service->getID() <<" , classname = "<< service->getClassname()
             <<" , service is stopped = "<< ( service->isStopped() ? "yes" : "no" ) << std::endl;
        for (const auto& obj : service->m_inputsMap)
        {
            ::fwData::Object::csptr object = obj.second.lock().get_shared();
            if (object)
            {
                info << "    input: key = " << obj.first << ", classname = " << object->getClassname() << std::endl;
            }
        }
        for (const auto& obj : service->m_inOutsMap)
        {
            ::fwData::Object::sptr object = obj.second.lock().get_shared();
            if (object)
            {
                info << "    inout: key = " << obj.first << ", classname = " << object->getClassname() << std::endl;
            }
        }
        for (const auto& obj : service->m_outputsMap)
        {
            ::fwData::Object::sptr object = obj.second.get_shared();
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
    SLM_DEBUG_IF("No output is defined for '" + objKey + "', the object is not emitted to the configuration", !hasID);
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

    // check if the service manage outputs that are not maintained by someone else.
    if (!service->m_outputsMap.empty())
    {
        std::string objectKeys;
        for (const auto& obj: service->m_outputsMap)
        {
            const ::fwData::Object::wptr output = obj.second.get_shared();
            if (output.use_count() == 1)
            {
                if (!objectKeys.empty())
                {
                    objectKeys += ", ";
                }
                objectKeys += "'" + obj.first + "'(nbRef: " + std::to_string(output.use_count()) + ")";
            }
        }
        SLM_WARN_IF(
            "Service "+ service->getID() + " still contains registered outputs: " + objectKeys + ". They will no "
            "longer be maintained. You should call setOutput(key, nullptr) before stopping the service to inform "
            "AppManager and other services that the object will be destroyed.", !objectKeys.empty());
    }

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
    ::fwCore::mt::WriteLock writeLock(m_containerMutex);

    ::fwData::Object::wptr obj = service->m_outputsMap[objKey].get_shared();

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
    ::fwCore::mt::ReadLock readLock(m_containerMutex);

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
    ::fwCore::mt::ReadLock readLock(m_containerMutex);

    if(access == ::fwServices::IService::AccessType::INPUT)
    {
        auto it = service->m_inputsMap.find(objKey);
        if(it != service->m_inputsMap.end())
        {
            return it->second.getShared();
        }
    }
    else if(access == ::fwServices::IService::AccessType::INOUT)
    {
        auto it = service->m_inOutsMap.find(objKey);
        if(it != service->m_inOutsMap.end())
        {
            return it->second.getShared();
        }
    }
    else
    {
        auto it = service->m_outputsMap.find(objKey);
        if(it != service->m_outputsMap.end())
        {
            return it->second.get_shared();
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

    SLM_FATAL_IF("object key is not defined", objKey.empty());

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

    m_services.insert(service);
}

//------------------------------------------------------------------------------

void ObjectService::removeFromContainer( ::fwServices::IService::sptr service )
{
    SLM_TRACE_FUNC();

    auto it = m_services.find(service);
    FW_RAISE_IF("service '" + service->getID() + "' is not found in the OSR", it == m_services.end());
    m_services.erase(it);
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices( const std::string& serviceType ) const
{
    ServiceVectorType services;
    ::fwCore::mt::ReadLock lock(m_containerMutex);

    for (const auto& srv : m_services)
    {
        if (srv->isA(serviceType))
        {
            services.insert( srv );
        }
    }
    return services;
}
} // namespace registry

//------------------------------------------------------------------------------

} // namespace fwServices
