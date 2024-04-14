/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <service/macros.hpp>
#include <service/registry.hpp>

namespace sight::service
{

/**
 * @name Methods for creating and attaching services to object
 */
//@{
/**
 * @brief Create a service of type serviceType
 * @return a pointer to the new service
 */
SIGHT_SERVICE_API service::base::sptr add(const std::string& _impl_type, const std::string& _id = "");

/**
 * @brief Create a service of type serviceType
 * @return a pointer to the new service with the given template type
 */
template<class SERVICE>
SPTR(SERVICE) add(const std::string& _impl_type, const std::string& _id = "");

//@}

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) add(const std::string& _impl_type, const std::string& _id)
{
    service::base::sptr generic_srv = service::add(_impl_type, _id);
    auto srv                        = std::dynamic_pointer_cast<SERVICE>(generic_srv);
    SIGHT_THROW_IF(
        "Failed to cast service from factory type '" + _impl_type + "' into '"
        + core::type_demangler<SERVICE>().get_classname() + "'",
        !srv
    );

    return srv;
}

/**
 * @brief Remove an existing service.
 * This provides a symmetric function to sight::service::add().
 * @return a pointer to the new service
 */
inline void remove(const SPTR(service::base)& _srv)
{
    service::unregister_service(_srv);
}

/**
 * @brief Return a registered base having uid as unique universal identifier , its an alias on
 * data::object::get_id(...) method
 *

 */
SIGHT_SERVICE_API service::base::sptr get(std::string _uid);

//------------------------------------------------------------------------------

} // namespace sight::service
