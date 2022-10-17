/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "service/registry/ObjectService.hpp"

#include "service/extension/AppConfig.hpp"
#include "service/extension/Config.hpp"
#include "service/extension/Factory.hpp"
#include "service/IService.hpp"

#include <core/LazyInstantiator.hpp>
#include <core/LogicStamp.hpp>
#include <core/runtime/Runtime.hpp>
#include <core/tools/fwID.hpp>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <utility>

namespace sight::service
{

//------------------------------------------------------------------------------

namespace OSR
{

//------------------------------------------------------------------------------

service::registry::ObjectService::sptr get()
{
    return core::LazyInstantiator<service::registry::ObjectService>::getInstance();
}

//------------------------------------------------------------------------------

std::string getRegistryInformation()
{
    return service::OSR::get()->getRegistryInformation();
}

//------------------------------------------------------------------------------

service::registry::ObjectService::ServiceVectorType getServices(const std::string& serviceType)
{
    return service::OSR::get()->getServices(serviceType);
}

//------------------------------------------------------------------------------

void registerService(service::IService::sptr service)
{
    service::OSR::get()->registerService(service);
}

//------------------------------------------------------------------------------

void registerServiceOutput(
    data::Object::sptr obj,
    std::string_view objKey,
    service::IService::sptr service,
    std::optional<std::size_t> index
)
{
    service::OSR::get()->registerServiceOutput(obj, objKey, service, index);
}

//------------------------------------------------------------------------------

void unregisterService(service::IService::sptr service)
{
    service::OSR::get()->unregisterService(service);
}

//------------------------------------------------------------------------------

void unregisterServiceOutput(std::string_view objKey, IService::sptr service, std::optional<std::size_t> index)
{
    service::OSR::get()->unregisterServiceOutput(objKey, service, index);
}

//------------------------------------------------------------------------------

} // namespace OSR

//------------------------------------------------------------------------------

namespace registry
{

const core::com::Signals::SignalKeyType ObjectService::s_REGISTERED_SIG   = "registered";
const core::com::Signals::SignalKeyType ObjectService::s_UNREGISTERED_SIG = "unregistered";

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
    data::Object::csptr previousObj;
    core::mt::ReadLock lock(m_containerMutex);

    for(const auto& service : m_services)
    {
        info << "Service : uid = " << service->getID() << " , classname = " << service->getClassname()
        << " , service is stopped = " << (service->isStopped() ? "true" : "false") << std::endl;
        for(const auto& [key, obj] : service->m_inputsMap)
        {
            data::Object::csptr object = obj.lock().get_shared();
            if(object)
            {
                const auto key_name = KEY_GROUP_NAME(key.first, key.second.has_value() ? key.second.value() : 0);
                info << "    input: key = " << key_name << ", classname = " << object->getClassname() << std::endl;
            }
        }

        for(const auto& [key, obj] : service->m_inOutsMap)
        {
            data::Object::sptr object = obj.lock().get_shared();
            if(object)
            {
                const auto key_name = KEY_GROUP_NAME(key.first, key.second.has_value() ? key.second.value() : 0);
                info << "    inout: key = " << key_name << ", classname = " << object->getClassname() << std::endl;
            }
        }

        for(const auto& [key, obj] : service->m_outputsMap)
        {
            data::Object::sptr object = obj.get_shared();
            if(object)
            {
                const auto key_name = KEY_GROUP_NAME(key.first, key.second.has_value() ? key.second.value() : 0);
                info << "    output: key = " << key_name << ", classname = " << object->getClassname() << std::endl;
            }
        }
    }

    return info.str();
}

//------------------------------------------------------------------------------

void ObjectService::registerService(service::IService::sptr service)
{
    core::mt::WriteLock writeLock(m_containerMutex);
    m_services.insert(service);
}

//------------------------------------------------------------------------------

void ObjectService::registerServiceOutput(
    data::Object::sptr object,
    std::string_view objKey,
    service::IService::sptr service,
    std::optional<std::size_t> index
)
{
    core::mt::WriteLock writeLock(m_containerMutex);
    m_services.insert(service);

    const bool hasID = service->hasObjectId(objKey, index);
    SIGHT_DEBUG_IF(
        "No output is defined for '" << objKey << "', the object is not emitted to the configuration",
        !hasID
    );
    if(hasID)
    {
        const auto id = service->getObjectId(objKey, index);
        auto sig      = this->signal<RegisterSignalType>(s_REGISTERED_SIG);
        sig->asyncEmit(object, id);
    }
}

//------------------------------------------------------------------------------

void ObjectService::unregisterService(service::IService::sptr service)
{
    core::mt::WriteLock writeLock(m_containerMutex);

    SIGHT_ASSERT(
        "The service ( " + service->getID() + " ) must be stopped before being unregistered.",
        service->isStopped()
    );

    this->removeFromContainer(service);

    // check if the service manage outputs that are not maintained by someone else.
    if(!service->m_outputsMap.empty())
    {
        std::string objectKeys;
        for(const auto& [key, obj] : service->m_outputsMap)
        {
            const data::Object::wptr output = obj.get_shared();
            if(output.use_count() == 1)
            {
                if(!objectKeys.empty())
                {
                    objectKeys += ", ";
                }

                objectKeys += "'" + std::string(key.first) + "'(nbRef: " + std::to_string(output.use_count()) + ")";
            }
        }

        SIGHT_WARN_IF(
            "Service " + service->getID() + " still contains registered outputs: " + objectKeys + ". They will no "
                                                                                                  "longer be maintained. You should call setOutput(key, nullptr) before stopping the service to inform "
                                                                                                  "AppManager and other services that the object will be destroyed.",
            !objectKeys.empty()
        );
    }

    service->m_inputsMap.clear();
    service->m_inOutsMap.clear();
    service->m_outputsMap.clear();
}

//------------------------------------------------------------------------------

void ObjectService::unregisterServiceOutput(
    std::string_view objKey,
    service::IService::sptr service,
    std::optional<std::size_t> index
)
{
    core::mt::WriteLock writeLock(m_containerMutex);

    const auto key = std::make_pair(std::string(objKey), index);

    auto obj = service->m_outputsMap[key].get_shared();

    service->m_outputsMap.erase(key);

    if(service->hasObjectId(objKey, index))
    {
        const auto id = service->getObjectId(objKey, index);
        auto sig      = this->signal<RegisterSignalType>(s_UNREGISTERED_SIG);
        sig->asyncEmit(obj, id);
    }
}

//------------------------------------------------------------------------------

void ObjectService::removeFromContainer(service::IService::sptr service)
{
    auto it = m_services.find(service);
    SIGHT_THROW_IF("service '" + service->getID() + "' is not found in the OSR", it == m_services.end());
    m_services.erase(it);
}

//------------------------------------------------------------------------------

ObjectService::ServiceVectorType ObjectService::getServices(const std::string& _serviceType) const
{
    const std::string serviceType = core::runtime::filterID(_serviceType);
    ServiceVectorType services;
    core::mt::ReadLock lock(m_containerMutex);

    for(const auto& srv : m_services)
    {
        if(srv->isA(serviceType))
        {
            services.insert(srv);
        }
    }

    return services;
}

} // namespace registry

//------------------------------------------------------------------------------

} // namespace sight::service
