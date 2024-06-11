/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "app/extension/config.hpp"
#include "app/helper/config.hpp"

#include "core/com/proxy.hpp"

#include "service/detail/service.hpp"
#include "service/extension/factory.hpp"

#include <core/com/has_signals.hpp>
#include <core/com/has_slots.hpp>
#include <core/com/helper/sig_slot_connection.hpp>
#include <core/object.hpp>
#include <core/runtime/helper.hpp>

#include <data/object.hpp>

#include <array>
#include <regex>
#include <string>
#include <vector>

namespace sight::app::helper
{

/// container for the data keywords for a service configuration
const std::array<std::string, 3> DATA_KEYWORDS = {{"in", "out", "inout"}};

static std::map<std::string, service::base::sptr> s_services_props;
static std::mutex s_services_props_mutex;

//-----------------------------------------------------------------------------

core::com::helper::proxy_connections config::parse_connections(
    const core::runtime::config_t& _connection_cfg,
    const std::string& _err_msg_head,
    std::function<std::string()> _generate_channel_name_fn
)
{
#ifndef _DEBUG
    SIGHT_NOT_USED(_err_msg_head);
#endif
    const std::string channel = _connection_cfg.get<std::string>("<xmlattr>.channel", _generate_channel_name_fn());

    core::com::helper::proxy_connections proxy_cnt(channel);

    for(const auto& elem : _connection_cfg)
    {
        const static std::regex s_RE("(.*)/(.*)");
        std::smatch match;
        std::string uid;
        std::string key;

        const auto src = elem.second.get_value<std::string>();
        if(std::regex_match(src, match, s_RE))
        {
            SIGHT_ASSERT("errMsgHead + Wrong value for attribute src: " << src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SIGHT_ASSERT(
                _err_msg_head + src << " configuration is not correct for " << elem.first,
                !uid.empty() && !key.empty()
            );

            if(elem.first == "signal")
            {
                proxy_cnt.add_signal_connection(uid, key);
            }
            else if(elem.first == "slot")
            {
                proxy_cnt.add_slot_connection(uid, key);
            }
        }
        else if(elem.first != "<xmlattr>")
        {
            SIGHT_ASSERT(
                _err_msg_head + "Signal or slot must be written as <signal>fwID/key</signal> or "
                                "<slot>fwID/key</slot>",
                false
            );
        }
    }

    // This is ok to return like this, thanks to C++11 rvalue there will be no copy of the vectors inside the struct
    return proxy_cnt;
}

//-----------------------------------------------------------------------------

app::detail::service_config config::parse_service(
    const boost::property_tree::ptree& _srv_elem,
    const std::string& _err_msg_head
)
{
#ifndef _DEBUG
    SIGHT_NOT_USED(_err_msg_head);
#endif

    // Get attributes
    app::detail::service_config srvconfig;

    srvconfig.m_uid = _srv_elem.get<std::string>("<xmlattr>.uid");
    SIGHT_ASSERT(_err_msg_head + "'uid' attribute is empty.", !srvconfig.m_uid.empty());

    std::string err_msg_tail = " when parsing service '" + srvconfig.m_uid + "'.";

    // config
    std::string config = _srv_elem.get<std::string>("<xmlattr>.config", "");
    SIGHT_ASSERT(_err_msg_head + "'config' attribute is empty.", !srvconfig.m_uid.empty());

    // Type
    srvconfig.m_type = _srv_elem.get<std::string>("<xmlattr>.type", "");
    SIGHT_ASSERT(
        std::string(_err_msg_head) + "Attribute \"type\" is required " + err_msg_tail,
        !srvconfig.m_type.empty()
    );

    // AutoConnect
    srvconfig.m_global_auto_connect = core::runtime::get_ptree_value(_srv_elem, "<xmlattr>.auto_connect", false);

    // Worker key
    srvconfig.m_worker = _srv_elem.get<std::string>("<xmlattr>.worker", "");

    // Get service configuration
    if(!config.empty())
    {
        const auto srv_cfg_factory = service::extension::config::get_default();
        srvconfig.m_config = srv_cfg_factory->get_service_config(config, srvconfig.m_type);
    }
    else
    {
        srvconfig.m_config = _srv_elem;
    }

    // Check if user did not bind a service to another service
    auto service_cfg = _srv_elem.equal_range("service");
    SIGHT_ASSERT(
        std::string(_err_msg_head) + "Cannot bind a service to another service" + err_msg_tail,
        service_cfg.first == service_cfg.second
    );
    service_cfg = _srv_elem.equal_range("serviceList");
    SIGHT_ASSERT(
        std::string(_err_msg_head) + "Cannot bind a service to another service" + err_msg_tail,
        service_cfg.first == service_cfg.second
    );

    // Check first if we can create this service
    // If there is a missing object in its data list, then it is not possible

    // Collect all input/output configurations
    std::vector<std::pair<std::string, boost::property_tree::ptree> > object_cfgs;
    for(const auto& data_keyword : DATA_KEYWORDS)
    {
        auto obj_cfgs = _srv_elem.equal_range(data_keyword);
        for(auto obj_cfg = obj_cfgs.first ; obj_cfg != obj_cfgs.second ; ++obj_cfg)
        {
            object_cfgs.emplace_back(obj_cfg->first, obj_cfg->second);
        }
    }

    // Parse input/output configurations
    for(const auto& cfg : object_cfgs)
    {
        // Access type
        app::detail::object_serviceconfig objconfig;
        if(cfg.first == "in")
        {
            objconfig.m_access = data::access::in;
        }
        else if(cfg.first == "out")
        {
            objconfig.m_access = data::access::out;
        }
        else if(cfg.first == "inout")
        {
            objconfig.m_access = data::access::inout;
        }
        else
        {
            SIGHT_FATAL("Unreachable code");
        }

        // Do we use groups ?
        const auto group = cfg.second.get_optional<std::string>("<xmlattr>.group");
        if(group)
        {
            auto key_cfgs          = cfg.second.equal_range("key");
            const std::string& key = group.value();
            const auto default_cfg = get_object_key_attrs(srvconfig.m_type, key);

            objconfig.m_auto_connect = core::runtime::get_ptree_value(
                cfg.second,
                "<xmlattr>.auto_connect",
                default_cfg.first
            );

            // Optional is global to all keys in the group
            if(objconfig.m_access != data::access::out)
            {
                objconfig.m_optional = core::runtime::get_ptree_value(
                    cfg.second,
                    "<xmlattr>.optional",
                    default_cfg.second
                );
            }
            else
            {
                objconfig.m_optional = true;
            }

            std::size_t count = 0;
            for(auto group_cfg = key_cfgs.first ; group_cfg != key_cfgs.second ; ++group_cfg)
            {
                app::detail::object_serviceconfig group_objconfig = objconfig;

                // Identifier
                group_objconfig.m_uid = group_cfg->second.get<std::string>("<xmlattr>.uid", "");
                SIGHT_ASSERT(
                    std::string(_err_msg_head) + "\"uid\" attribute is empty" + err_msg_tail,
                    !group_objconfig.m_uid.empty()
                );

                group_objconfig.m_key = key;

                // AutoConnect can be overriden by element in the group
                group_objconfig.m_auto_connect = core::runtime::get_ptree_value(
                    group_cfg->second,
                    "<xmlattr>.auto_connect",
                    group_objconfig.m_auto_connect
                );

                // Optional can be overriden by element in the group
                if(group_objconfig.m_access != data::access::out)
                {
                    group_objconfig.m_optional = core::runtime::get_ptree_value(
                        group_cfg->second,
                        "<xmlattr>.optional",
                        group_objconfig.m_optional
                    );
                }
                else
                {
                    group_objconfig.m_optional = true;
                }

                // Assign the current object config in the service config
                srvconfig.m_objects[{key, count++}] = group_objconfig;
            }
        }
        else
        {
            // Identifier
            objconfig.m_uid = cfg.second.get<std::string>("<xmlattr>.uid", "");
            SIGHT_ASSERT(
                std::string(_err_msg_head) + "\"uid\" attribute is empty" + err_msg_tail,
                !objconfig.m_uid.empty()
            );

            // Key inside the service
            objconfig.m_key = cfg.second.get<std::string>("<xmlattr>.key", "");
            SIGHT_ASSERT(
                std::string(_err_msg_head) + "Missing object attribute 'key'" + err_msg_tail,
                !objconfig.m_key.empty()
            );

            const auto default_cfg = get_object_key_attrs(srvconfig.m_type, objconfig.m_key);

            // AutoConnect
            objconfig.m_auto_connect = core::runtime::get_ptree_value(
                cfg.second,
                "<xmlattr>.auto_connect",
                default_cfg.first
            );

            // Optional
            if(objconfig.m_access != data::access::out)
            {
                objconfig.m_optional = core::runtime::get_ptree_value(
                    cfg.second,
                    "<xmlattr>.optional",
                    default_cfg.second
                );
            }
            else
            {
                objconfig.m_optional = true;
            }

            // Assign the current object config in the service config
            srvconfig.m_objects[{objconfig.m_key, std::nullopt}] = objconfig;
        }
    }

    return srvconfig;
}

//------------------------------------------------------------------------------

std::pair<bool, bool> config::get_object_key_attrs(
    const std::string& _service_type,
    const std::string& _key
)
{
    std::lock_guard guard(s_services_props_mutex);

    service::base::sptr srv;
    auto it = s_services_props.find(_service_type);
    if(it == s_services_props.end())
    {
        auto srv_factory = service::extension::factory::get();
        srv                             = srv_factory->create(_service_type);
        s_services_props[_service_type] = srv;
    }
    else
    {
        srv = it->second;
    }

    return service::manager::get_object_key_attrs(srv, _key);
}

// ----------------------------------------------------------------------------

void config::clear_props()
{
    std::lock_guard guard(s_services_props_mutex);
    s_services_props.clear();
}

// ----------------------------------------------------------------------------

} // namespace sight::app::helper
