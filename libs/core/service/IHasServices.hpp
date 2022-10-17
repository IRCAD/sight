/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <core/macros.hpp>

#include <service/op/Add.hpp>

#include <vector>

namespace sight::service
{

class IService;

/**
 * @brief   Interface for objects that need to manage services.
 */
class SERVICE_CLASS_API IHasServices
{
public:

    typedef std::vector<WPTR(service::IService)> ServiceVector;

    /**
     * @brief Get all subservices linked to this service
     * @return The vector of linked services
     */
    [[nodiscard]] const ServiceVector& getRegisteredServices() const;

    /**
     * @brief Return a specific registered service
     * @param[in] id Identifier of the service
     */
    [[nodiscard]] SERVICE_API CSPTR(service::IService) getRegisteredService(const core::tools::fwID::IDType& _id) const;

protected:

    /// Constructor
    SERVICE_API IHasServices() noexcept;

    /// Destructor
    SERVICE_API virtual ~IHasServices() noexcept;

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    SERVICE_API SPTR(service::IService) registerService(
        const std::string& _implType,
        const std::string& _id = ""
    );

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    template<class T>
    SPTR(T) registerService(const std::string& _implType, const std::string& _id = "");

    /**
     * @brief Unregister a specific service
     * @param id Identifier of the service
     */
    SERVICE_API void unregisterService(const core::tools::fwID::IDType& _id);

    /**
     * @brief Unregister a specific service
     * @param service Pointer to the service
     */
    SERVICE_API void unregisterService(const service::IService::sptr& _service);

    /**
     * @brief Unregister all services linked to this service, optionally matches only a given type of services
     * @param _implType Optional type of the services to unregister
     */
    SERVICE_API void unregisterServices(const std::string& _implType = "");

private:

    /// Sub services linked to this service
    ServiceVector m_subServices;
};

//------------------------------------------------------------------------------

inline const IHasServices::ServiceVector& IHasServices::getRegisteredServices() const
{
    return m_subServices;
}

//------------------------------------------------------------------------------

template<class T>
SPTR(T) IHasServices::registerService(const std::string& _implType, const std::string& _id)
{
    auto srv = service::add<T>(_implType, _id);
    m_subServices.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

} //namespace sight::service
