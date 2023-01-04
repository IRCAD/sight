/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service/config.hpp"
#include "service/helper/ProxyConnections.hpp"
#include "service/IService.hpp"

#include <core/base.hpp>
#include <core/com/Signals.hpp>
#include <core/com/Slots.hpp>
#include <core/runtime/types.hpp>
#include <core/tools/Object.hpp>

namespace sight::core::tools
{

class Object;

} // namespace sight::core::tools

namespace sight::data
{

class Object;

} // namespace sight::data

namespace sight::core::com::helper
{

class SigSlotConnection;

} // namespace sight::core::com::helper

namespace sight::service::detail
{

/// Used to store object configuration in a service.
struct ObjectServiceConfig
{
    /// Object key name, may contains a suffix #N with the number of the key if it is part of a group
    std::string m_key;

    /// Object identifier
    std::string m_uid;

    /// Object access (INPUT, INOUT, OUTPUT)
    data::Access m_access {data::Access::INOUT};

    /// True if the service is autoConnected this object according to the auto-connection map
    bool m_autoConnect {false};

    /// True if the object is optional (i.e. the service can start even if the object is not present)
    bool m_optional {false};
};
/// Used to store a service configuration.
struct ServiceConfig
{
    /// Service uid
    std::string m_uid;

    /// Service implementation
    std::string m_type;

    /// True if the service is autoConnected to all of its inputs/inouts according to the auto-connection map
    bool m_globalAutoConnect {false};

    /// Service worker
    std::string m_worker;

    /// list of required objects information (inputs, inouts and outputs), indexed by key name and index
    std::map<std::pair<std::string, std::optional<std::size_t> >, ObjectServiceConfig> m_objects;

    /// list of required object groups information (inputs, inouts and outputs), indexed by key name
    std::map<std::string, ObjectServiceConfig> m_groups;

    /// Service configuration (only used with XML config)
    IService::config_t m_config;
};

}

namespace sight::service::helper
{

/**
 * @brief Provides configuration element parsing tools.
 */
class Config
{
public:

    typedef std::pair<std::string, core::com::Signals::SignalKeyType> SignalInfoType;
    typedef std::pair<std::string, core::com::Slots::SlotKeyType> SlotInfoType;
    typedef std::vector<SlotInfoType> SlotInfoContainerType;

    typedef std::string ObjectIdType;
    typedef std::vector<ProxyConnections> ProxyConnectionsVectType;
    typedef std::map<ObjectIdType, ProxyConnectionsVectType> ProxyConnectionsMapType;
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
    SERVICE_API static ConnectionInfo parseConnections(
        const core::runtime::config_t& cfg,
        const CSPTR(core::tools::Object)& obj =
        CSPTR(core::tools::Object)()
    );

    /**
     * @brief Parses "<connect>" tags from given configuration and return a structure containing the signal and
     *        slots informations.
     *
     * @param cfg configuration element containing "<connect>" tags
     */
    SERVICE_API static ProxyConnections parseConnections2(
        const core::runtime::config_t& connectionCfg,
        const std::string& errMsgHead,
        std::function<std::string()> generateChannelNameFn
    );

    /**
     * @brief Parses "<connect>" tags from given configuration to connect signals and slots using given helper.
     *
     * @param cfg configuration element containing "<connect>" tags
     * @param helper SigSlotConnection helper to connect signals and slots
     * @param obj optional object used to retrieve signal if uid is not defined [deprecated]
     */
    SERVICE_API static void createConnections(
        const core::runtime::config_t& cfg,
        core::com::helper::SigSlotConnection& helper,
        const CSPTR(core::tools::Object)& obj = CSPTR(core::tools::Object)()
    );

    /// Disconnects all proxies associated to objectKey
    SERVICE_API static void disconnectProxies(
        const std::string& objectKey,
        Config::ProxyConnectionsMapType& proxyMap
    );

    /// Parse a service and return a service configuration
    SERVICE_API static service::detail::ServiceConfig parseService(
        const boost::property_tree::ptree& srvElem,
        const std::string& errMsgHead
    );

    SERVICE_API static std::pair<bool, bool> getKeyProps(
        const std::string& serviceType,
        const std::string& key
    );

    SERVICE_API static void clearProps();
};

} // namespace sight::service::helper
