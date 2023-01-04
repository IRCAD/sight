/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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
#include "service/helper/ProxyConnections.hpp"

namespace sight::service
{

class IService;

} // namespace sight::service

namespace sight::service::detail
{

/**
 * @brief   Private implementation of the public IService interface, which is the base class for all services.
 *
 * This class defines the API to use and declare services.
 * The service state aims to impose the execution order (i.e. configure(), start(), update() or, stop()).
 */
class ServiceConnection
{
public:

    /// Connect the service with configuration services and objects
    void connect(const service::IService& _service);

    /// Disconnect the service from configuration services and objects
    void disconnect(const service::IService& _service);

    /// Add a known connection from the appConfig
    void add(const helper::ProxyConnections& proxy);

    /// Proxies configurations, connected at start, and disconnected at stop
    std::map<std::string, helper::ProxyConnections> m_proxies;
};

} // namespace sight::service::detail
