/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/app/config.hpp>

#include "core/com/helper/proxy_connections.hpp"

#include "service/manager.hpp"
#include "service/object_parser.hpp"

#include <core/base.hpp>
#include <core/com/signals.hpp>
#include <core/com/slots.hpp>
#include <core/object.hpp>
#include <core/runtime/types.hpp>

namespace sight::core
{

class object;

} // namespace sight::core

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
struct object_serviceconfig
{
    /// Object key name, may contains a suffix #N with the number of the key if it is part of a group
    std::string m_key;

    /// Object identifier
    std::string m_uid;

    /// Object access (INPUT, INOUT, OUTPUT)
    data::access m_access {data::access::inout};

    /// True if the service is autoConnected this object according to the auto-connection map
    bool m_auto_connect {false};

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
    bool m_global_auto_connect {false};

    /// Service worker
    std::string m_worker;

    /// list of required objects information (inputs, inouts and outputs), indexed by key name and index
    std::map<std::pair<std::string, std::optional<std::size_t> >, object_serviceconfig> m_objects;

    /// list of required object groups information (inputs, inouts and outputs), indexed by key name
    std::map<std::string, object_serviceconfig> m_groups;

    /// list of required properties, indexed by key name and index
    std::map<std::string, object_serviceconfig> m_properties;

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

    using signal_info_t         = std::pair<std::string, core::com::signals::key_t>;
    using slot_info_t           = std::pair<std::string, core::com::slots::key_t>;
    using slot_info_container_t = std::vector<slot_info_t>;

    using config_attribute_t       = std::pair<std::string, bool>;
    using object_id_t              = std::string;
    using proxy_connections_vect_t = std::vector<core::com::helper::proxy_connections>;
    using proxy_connections_map_t  = std::map<object_id_t, proxy_connections_vect_t>;
    using objects_set_t            = std::set<std::string>;

    /**
     * @brief Parses "<connect>" tags from given configuration and return a structure containing the signal and
     *        slots informations.
     *
     * @param _connection_cfg configuration element containing "<connect>" tags
     */
    SIGHT_APP_API static core::com::helper::proxy_connections parse_connections(
        const core::runtime::config_t& _connection_cfg,
        const std::string& _err_msg_head,
        std::function<std::string()> _generate_channel_name_fn
    );

    /** Parse an object configuration, creates it and its subobjects recursively.
     * @param _cfg input configuration of the object
     * @param _objects found objects
     */
    SIGHT_APP_API static void parse_object(
        const boost::property_tree::ptree& _cfg,
        service::object_parser::objects_t& _objects
    );

    static data::object::sptr get_new_object(config_attribute_t _type, config_attribute_t _uid);

    static data::object::sptr get_object(config_attribute_t _type, const std::string& _uid);

    /// Parse a service and return a service configuration
    SIGHT_APP_API static app::detail::service_config parse_service(
        const boost::property_tree::ptree& _srv_elem,
        const std::string& _err_msg_head,
        const service::object_parser::objects_set_t& _objects
    );

    SIGHT_APP_API static std::pair<bool, bool> get_object_key_attrs(
        const std::string& _service_type,
        const std::string& _key
    );

    SIGHT_APP_API static void clear_props();
};

} // namespace sight::app::helper
