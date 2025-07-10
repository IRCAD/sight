/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

namespace sight::service
{

class base;

} // namespace sight::service

namespace sight::service::detail
{

/**
 * @brief   Utility object used to isolate the code managing signal/slots connections in services.
 *
 * In a regular life cycle, the connections configured in a service are plugged in after start. Thus the connection
 * with the start slot, which allows other events to start a service, requires a specific handling, performed before
 * the start of the service.
 */
class service_connection
{
public:

    /// Connect the service with configuration services and objects
    void connect(const sight::service::base& _service);

    /// Disconnect the service from configuration services and objects
    void disconnect(const sight::service::base& _service);

    /// Connect only the start slot of the service with other services and objects
    void connect_start_slot(const sight::service::base& _service);

    /// Disconnect only the start slot of service from other services and objects
    void disconnect_start_slot(const sight::service::base& _service);

    /// Add a known connection from the appConfig
    void add(const core::com::helper::proxy_connections& _proxy);

    /// Proxies configurations, connected at start, and disconnected at stop
    std::map<std::string, core::com::helper::proxy_connections> m_proxies;
};

} // namespace sight::service::detail
