/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service/config.hpp"
#include "service/IService.hpp"

#include <set>

namespace sight::service
{

using service_vector_t = std::set<SPTR(service::IService)>;

/// Returns a list of all registered services
SERVICE_API std::string getRegistryInformation();

/// Returns a container with all registered services of a given type
SERVICE_API const service::service_vector_t& getServices();

/// Returns a container with all registered services of a given type
SERVICE_API service_vector_t getServices(const std::string& serviceType);

/// Returns a container with all registered services of a given template type
template<class SERVICE>
std::set<SPTR(SERVICE)> getServices();

/**
 * @brief Register the service alone
 *
 * @param service Service to add to the OSR
 */
SERVICE_API void registerService(SPTR(service::IService) service);

/**
 * @brief Remove the service (service) from the m_container
 *
 * @param service Service whose key should be removed
 */
SERVICE_API void unregisterService(SPTR(service::IService) service);

//------------------------------------------------------------------------------

template<class SERVICE>
std::set<SPTR(SERVICE)> getServices()
{
    const auto allServices = getServices();
    std::set<SPTR(SERVICE)> services;

    for(const auto& srv : allServices)
    {
        SPTR(SERVICE) service = std::dynamic_pointer_cast<SERVICE>(srv);
        if(service)
        {
            services.insert(service);
        }
    }

    return services;
}

} // namespace sight::service
