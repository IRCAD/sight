/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
#include "services/helper/ProxyConnections.hpp"
#include "services/IService.hpp"

#include <core/base.hpp>
#include <core/com/Signals.hpp>
#include <core/com/Slots.hpp>
#include <core/tools/Object.hpp>

namespace fwTools
{
class Object;
}

namespace sight::data
{
class Object;
}

namespace fwRuntime
{
struct ConfigurationElement;
}

namespace sight::services
{

namespace helper
{

class SigSlotConnection;

/**
 * @brief Provides configuration element parsing tools.
 */
class Config
{

public:

    typedef std::pair< std::string, core::com::Signals::SignalKeyType > SignalInfoType;
    typedef std::pair< std::string, core::com::Slots::SlotKeyType > SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    typedef std::string ObjectIdType;
    typedef std::vector<ProxyConnections> ProxyConnectionsVectType;
    typedef std::map< ObjectIdType, ProxyConnectionsVectType > ProxyConnectionsMapType;

    struct ConnectionInfo
    {
        SignalInfoType m_signal;
        SlotInfoContainerType m_slots;
    };

    /**
     * @brief Parses "<connect>" tags from given configuration and return a structure containing the signal and
     *        slots informations.
     *
     * @param cfg configuration element containing "<connect>" tags
     * @param obj optional object used to retrieve signal if uid is not defined [deprecated]
     */
    SERVICES_API static ConnectionInfo parseConnections(const CSPTR(core::runtime::ConfigurationElement)& cfg,
                                                        const CSPTR(core::tools::Object)& obj =
                                                            CSPTR(core::tools::Object)());

    /**
     * @brief Parses "<connect>" tags from given configuration and return a structure containing the signal and
     *        slots informations.
     *
     * @param cfg configuration element containing "<connect>" tags
     */
    SERVICES_API static ProxyConnections parseConnections2(
        const CSPTR(core::runtime::ConfigurationElement)& connectionCfg,
        const std::string& errMsgHead,
        std::function<std::string()> generateChannelNameFn);

    /**
     * @brief Parses "<connect>" tags from given configuration to connect signals and slots using given helper.
     *
     * @param cfg configuration element containing "<connect>" tags
     * @param helper SigSlotConnection helper to connect signals and slots
     * @param obj optional object used to retrieve signal if uid is not defined [deprecated]
     */
    SERVICES_API static void createConnections(
        const CSPTR(core::runtime::ConfigurationElement)& cfg,
        core::com::helper::SigSlotConnection& helper,
        const CSPTR(core::tools::Object)& obj = CSPTR(core::tools::Object)());

    /**
     * @brief Parses "<proxy>" tags from given configuration to connect signals and slots using proxies.
     *
     * @param objectKey Id of the object
     * @param cfg configuration element containing "<proxy>" tags
     * @param proxyMap map containing the proxy's signals and slots connections
     * @param obj optional object used to retrieve signal if uid is not defined
     */
    SERVICES_API static void createProxy(
        const std::string& objectKey,
        const CSPTR(core::runtime::ConfigurationElement)& cfg,
        ProxyConnectionsMapType& proxyMap,
        const CSPTR(data::Object)& obj = CSPTR(data::Object)());

    /// Disconnects all proxies associated to objectKey
    SERVICES_API static void disconnectProxies(const std::string& objectKey,
                                               Config::ProxyConnectionsMapType& proxyMap);

    /// Parse a service and return a service configuration
    SERVICES_API static services::IService::Config parseService(
        const CSPTR(core::runtime::ConfigurationElement)& srvElem,
        const std::string& errMsgHead);
};

} // namespace helper

} // namespace sight::services
