/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "service/IService.hpp"

#include <core/com/Signal.hxx>

namespace sight::service
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OSR
{

//------------------------------------------------------------------------------
template<data::Access A>
void registerService(
    typename data::access_traits<A>::value obj,
    const service::IService::KeyType& objKey,
    service::IService::sptr service
)
{
    service::OSR::get()->registerService<A>(obj, objKey, service);
}

//------------------------------------------------------------------------------

template<class SERVICE>
std::set<SPTR(SERVICE)> getServices()
{
    return service::OSR::get()->getServices<SERVICE>();
}

//------------------------------------------------------------------------------

inline SPTR(service::registry::ObjectService::RegisterSignalType) getRegisterSignal()
{
    return service::OSR::get()->signal<service::registry::ObjectService::RegisterSignalType>
               (service::registry::ObjectService::s_REGISTERED_SIG);
}

inline SPTR(service::registry::ObjectService::RegisterSignalType) getUnregisterSignal()
{
    return service::OSR::get()->signal<service::registry::ObjectService::RegisterSignalType>
               (service::registry::ObjectService::s_UNREGISTERED_SIG);
}

} //namespace OSR

//------------------------------------------------------------------------------

namespace registry
{

//------------------------------------------------------------------------------

template<class SERVICE>
std::set<SPTR(SERVICE)> ObjectService::getServices() const
{
    std::set<SPTR(SERVICE)> services;

    for(const auto& srv : m_services)
    {
        SPTR(SERVICE) service = std::dynamic_pointer_cast<SERVICE>(srv);
        if(service)
        {
            services.insert(service);
        }
    }

    return services;
}

//------------------------------------------------------------------------------

template<data::Access A>
void ObjectService::registerService(
    typename data::access_traits<A>::value object,
    const service::IService::KeyType& objKey,
    service::IService::sptr service
)
{
    core::mt::WriteLock writeLock(m_containerMutex);
    this->internalRegisterService<A>(object, service, objKey);
}

//------------------------------------------------------------------------------
template<>
inline void ObjectService::internalRegisterService<data::Access::in>(
    typename data::access_traits<data::Access::in>::value object,
    service::IService::sptr service,
    const service::IService::KeyType& objKey
)
{
    SIGHT_ASSERT("Can't register a null service in OSR.", service);
    SIGHT_ASSERT("Can't register a null object in OSR.", object);

    SIGHT_FATAL_IF("object key is not defined", objKey.empty());

    service->m_inputsMap[objKey] = object;
    m_services.insert(service);
}

//------------------------------------------------------------------------------
template<>
inline void ObjectService::internalRegisterService<data::Access::inout>(
    typename data::access_traits<data::Access::inout>::value object,
    service::IService::sptr service,
    const service::IService::KeyType& objKey
)
{
    SIGHT_ASSERT("Can't register a null service in OSR.", service);
    SIGHT_ASSERT("Can't register a null object in OSR.", object);

    SIGHT_FATAL_IF("object key is not defined", objKey.empty());

    service->m_inOutsMap[objKey] = object;
    m_services.insert(service);
}

//------------------------------------------------------------------------------
template<>
inline void ObjectService::internalRegisterService<data::Access::out>(
    typename data::access_traits<data::Access::out>::value object,
    service::IService::sptr service,
    const service::IService::KeyType& objKey
)
{
    SIGHT_ASSERT("Can't register a null service in OSR.", service);
    SIGHT_ASSERT("Can't register a null object in OSR.", object);

    SIGHT_FATAL_IF("object key is not defined", objKey.empty());

    service->m_outputsMap[objKey] = object;
    m_services.insert(service);
}

//------------------------------------------------------------------------------

} // namespace registry

} // namespace sight::service
