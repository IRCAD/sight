/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "services/config.hpp"

#include <core/macros.hpp>

#include <services/op/Add.hpp>

#include <vector>

namespace sight::services
{
class IService;

/**
 * @brief   Interface for objects that need to manage services.
 */
class SERVICES_CLASS_API IHasServices
{
public:

    typedef std::vector < WPTR(services::IService) > ServiceVector;

    /**
     * @brief Get all subservices linked to this service
     * @return The vector of linked services
     */
    const ServiceVector& getRegisteredServices() const;

    /**
     * @brief Return a specific registered service
     * @param[in] id Identifier of the service
     */
    SERVICES_API CSPTR( services::IService) getRegisteredService(const core::tools::fwID::IDType& _id) const;

protected:

    /// Constructor
    SERVICES_API IHasServices() noexcept;

    /// Destructor
    SERVICES_API virtual ~IHasServices() noexcept;

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    SERVICES_API SPTR(services::IService) registerService(const std::string& _implType,
                                                          const std::string& _id = "" );

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    template <class T>
    SPTR(T) registerService(const std::string& _implType, const std::string& _id = "" );

    /**
     * @brief Unregister a specific service
     * @param id Identifier of the service
     */
    SERVICES_API void unregisterService(const core::tools::fwID::IDType& _id);

    /**
     * @brief Unregister a specific service
     * @param service Pointer to the service
     */
    SERVICES_API void unregisterService(const services::IService::sptr& _service);

    /**
     * @brief Unregister all services linked to this service, optionally matches only a given type of services
     * @param _implType Optional type of the services to unregister
     */
    SERVICES_API void unregisterServices(const std::string& _implType = "");

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

template <class T>
SPTR(T) IHasServices::registerService(const std::string& _implType, const std::string& _id)
{
    auto srv = services::add<T>(_implType, _id);
    m_subServices.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

} //namespace sight::services
