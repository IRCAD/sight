/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <sight/service/config.hpp>

#include "core/com/helper/proxy_connections.hpp"

#include "service/base.hpp"

namespace sight::service
{

/**
 * @brief   Utility class used to give access to internal services mechanism for classes that manage services, like
 * application managers. The methods exposed here should not be visible in the public API.
 */
class SIGHT_SERVICE_CLASS_API manager
{
public:

    /**
     * @brief Set a registered object for a service
     *
     * @param[in] _srv service
     * @param[in] _obj object used by the service
     * @param[in] _key key of the object
     * @param[in] _index index of the data in the group
     * @param[in] _access access to the object (in/inout/out)
     * @param[in] _auto_connect if true, the service will be connected to the object's signals
     * @param[in] _optional if true, the service can be started even if the objet is not present
     */
    SIGHT_SERVICE_API static void set_object(
        service::base::sptr& _srv,
        data::object::sptr _obj,
        std::string_view _key,
        std::optional<std::size_t> _index,
        data::access _access,
        std::optional<bool> _auto_connect,
        const bool _optional
    );

    /**
     * @brief Unset a registered object for a service
     *
     * @param[in] _srv service
     * @param[in] _key key of the object
     * @param[in] _index index of the data in the group
     */
    SIGHT_SERVICE_API static void reset_object(
        service::base::sptr& _srv,
        std::string_view _key,
        std::optional<std::size_t> _index
    );

    /**
     * @brief Set the deferred identifier of a key. This is useful to declare an object that is not present at start of
     * the application and that will be created later.
     * @param[in] _srv service
     * @param[in] _key key of the object
     * @param[in] _id label of the object
     * @param[in] _index index of the data in the group
     */
    SIGHT_SERVICE_API static void set_deferred_id(
        service::base::sptr& _srv,
        std::string_view _key,
        const std::string& _id,
        std::optional<std::size_t> _index
    );

    /// Add connections to a service
    SIGHT_SERVICE_API static void add_connection(
        service::base::sptr& _srv,
        const core::com::helper::proxy_connections& _proxy
    );

    /// Connects the service with its data
    SIGHT_SERVICE_API static void auto_connect(service::base::sptr& _srv);

    /// Disconnects the service from its data
    SIGHT_SERVICE_API static void auto_disconnect(service::base::sptr& _srv);

    /// Connects a slot to the signal used to notify about newly deferred objects
    SIGHT_SERVICE_API static core::com::connection connect_register_out(const core::com::slot_base::sptr& _slot);

    /// Disconnect a slot to the signal used to notify about newly deferred objects
    SIGHT_SERVICE_API static core::com::connection connect_unregister_out(const core::com::slot_base::sptr& _slot);

    /// Notify about a newly deferred object
    SIGHT_SERVICE_API static void notify_register_out(data::object::sptr, const std::string&);

    /// Notify about a destroyed deferred object
    SIGHT_SERVICE_API static void notify_unregister_out(data::object::sptr, const std::string&);

    /// Returns a boolean to indicate if the object is optional
    SIGHT_SERVICE_API static bool is_key_optional(const service::base::sptr& _srv, const std::string& _key);
};

} // namespace sight::service
