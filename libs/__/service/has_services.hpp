/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include <service/op.hpp>

#include <vector>

namespace sight::service
{

class base;

/**
 * @brief   Interface for objects that need to manage services.
 */
class SERVICE_CLASS_API has_services
{
public:

    using service_vector_t = std::vector<std::weak_ptr<service::base> >;

    /**
     * @brief Get all subservices linked to this service
     * @return The vector of linked services
     */
    [[nodiscard]] const service_vector_t& get_registered_services() const;

    /**
     * @brief Return a specific registered service
     * @param[in] id Identifier of the service
     */
    [[nodiscard]] SERVICE_API CSPTR(service::base) get_registered_service(const core::tools::id::type& _id) const;

protected:

    /// Constructor
    SERVICE_API has_services() noexcept;

    /// Destructor
    SERVICE_API virtual ~has_services() noexcept;

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    SERVICE_API SPTR(service::base) register_service(
        const std::string& _impl_type,
        const std::string& _id = ""
    );

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    template<class T>
    SPTR(T) register_service(const std::string& _impl_type, const std::string& _id = "");

    /**
     * @brief Unregister a specific service
     * @param id Identifier of the service
     */
    SERVICE_API void unregister_service(const core::tools::id::type& _id);

    /**
     * @brief Unregister a specific service
     * @param service Pointer to the service
     */
    SERVICE_API void unregister_service(const service::base::sptr& _service);

    /**
     * @brief Unregister all services linked to this service, optionally matches only a given type of services
     * @param _implType Optional type of the services to unregister
     */
    SERVICE_API void unregister_services(const std::string& _impl_type = "");

private:

    /// Sub services linked to this service
    service_vector_t m_sub_services;
};

//------------------------------------------------------------------------------

inline const has_services::service_vector_t& has_services::get_registered_services() const
{
    return m_sub_services;
}

//------------------------------------------------------------------------------

template<class T>
SPTR(T) has_services::register_service(const std::string& _impl_type, const std::string& _id)
{
    auto srv = service::add<T>(_impl_type, _id);
    m_sub_services.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

} //namespace sight::service
