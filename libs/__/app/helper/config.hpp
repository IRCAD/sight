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

#include "app/config.hpp"

#include "core/com/helper/proxy_connections.hpp"

#include "service/manager.hpp"

#include <core/base.hpp>
#include <core/com/signals.hpp>
#include <core/com/slots.hpp>
#include <core/runtime/types.hpp>
#include <core/tools/object.hpp>

namespace sight::core::tools
{

class object;

} // namespace sight::core::tools

namespace sight::data
{

class object;

} // namespace sight::data

namespace sight::core::com::helper
{

class sig_slot_connection;

} // namespace sight::core::com::helper

namespace sight::app::detail
{

/// Used to store object configuration in a service.
struct ObjectServiceconfig
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
struct service_config
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
    std::map<std::pair<std::string, std::optional<std::size_t> >, ObjectServiceconfig> m_objects;

    /// list of required object groups information (inputs, inouts and outputs), indexed by key name
    std::map<std::string, ObjectServiceconfig> m_groups;

    /// Service configuration (only used with XML config)
    service::config_t m_config;
};

}

namespace sight::app::helper
{

/**
 * @brief Provides configuration element parsing tools.
 */
class config : public sight::service::manager
{
public:

    typedef std::pair<std::string, core::com::signals::key_t> SignalInfoType;
    typedef std::pair<std::string, core::com::slots::key_t> SlotInfoType;
    typedef std::vector<SlotInfoType> SlotInfoContainerType;

    typedef std::string ObjectIdType;
    typedef std::vector<core::com::helper::proxy_connections> proxy_connectionsVectType;
    typedef std::map<ObjectIdType, proxy_connectionsVectType> proxy_connections_map_t;
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
    APP_API static ConnectionInfo parseConnections(
        const core::runtime::config_t& cfg,
        const CSPTR(core::tools::object)& obj =
        CSPTR(core::tools::object)()
    );

    /**
     * @brief Parses "<connect>" tags from given configuration and return a structure containing the signal and
     *        slots informations.
     *
     * @param cfg configuration element containing "<connect>" tags
     */
    APP_API static core::com::helper::proxy_connections parseConnections2(
        const core::runtime::config_t& connectionCfg,
        const std::string& errMsgHead,
        std::function<std::string()> generateChannelNameFn
    );

    /**
     * @brief Parses "<connect>" tags from given configuration to connect signals and slots using given helper.
     *
     * @param cfg configuration element containing "<connect>" tags
     * @param helper sig_slot_connection helper to connect signals and slots
     * @param obj optional object used to retrieve signal if uid is not defined [deprecated]
     */
    APP_API static void createConnections(
        const core::runtime::config_t& cfg,
        core::com::helper::sig_slot_connection& helper,
        const CSPTR(core::tools::object)& obj = CSPTR(core::tools::object)()
    );

    /// Disconnects all proxies associated to objectKey
    APP_API static void disconnectProxies(
        const std::string& objectKey,
        config::proxy_connections_map_t& proxyMap
    );

    /// Parse a service and return a service configuration
    APP_API static app::detail::service_config parseService(
        const boost::property_tree::ptree& srvElem,
        const std::string& errMsgHead
    );

    APP_API static std::pair<bool, bool> get_object_key_attrs(
        const std::string& serviceType,
        const std::string& key
    );

    APP_API static void clearProps();
};

} // namespace sight::app::helper
