/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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
#include <core/ptree.hpp>
#include <core/runtime/runtime.hpp>

#include <data/extension/config.hpp>
#include <data/object.hpp>

#include <array>
#include <regex>
#include <string>
#include <vector>

namespace sight::app::helper
{

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

//------------------------------------------------------------------------------

void config::parse_object(
    const boost::property_tree::ptree& _config,
    service::object_parser::objects_t& _objects
)
{
    const boost::property_tree::ptree config = _config.get_child("object", _config);

    // Id
    config_attribute_t id("", false);
    if(const auto uid = config.get_optional<std::string>("<xmlattr>.uid"); uid.has_value())
    {
        id.first = uid.value();
        SIGHT_THROW_IF("\"uid\" attribute is empty.", id.first.empty());
        id.second = true;
    }

    // Type
    config_attribute_t type("", false);
    if(const auto type_cfg = config.get_optional<std::string>("<xmlattr>.type"); type_cfg.has_value())
    {
        type.first = type_cfg.value();
        SIGHT_THROW_IF("\"type\" attribute is empty.", type.first.empty());
        type.second = true;
    }

    enum modifier_t : std::uint8_t
    {
        none,
        deferred,
        reference,
        preference
    } modifier = none;

    if(const auto deferred_cfg = config.get_optional<bool>("<xmlattr>.deferred"); deferred_cfg.has_value())
    {
        modifier = deferred;
    }

    if(const auto ref_cfg = config.get_optional<bool>("<xmlattr>.reference"); ref_cfg.has_value())
    {
        SIGHT_ASSERT("Deferred object cannot also be a reference", modifier == none);
        modifier = reference;
    }

    if(const auto preference_cfg = config.get_optional<bool>("<xmlattr>.preference"); preference_cfg.has_value())
    {
        SIGHT_ASSERT("Deferred and reference objects cannot be set as preference", modifier == none);
        modifier = preference;
    }

    if(const auto modifier_cfg = config.get_optional<std::string>("<xmlattr>.src"); modifier_cfg.has_value())
    {
        SIGHT_ASSERT(
            "Do not use obsolete 'src' attribute with 'deferred', 'reference' or 'preference' attribute.",
            modifier == none
        );

        modifier =
            *modifier_cfg == "deferred" ? deferred
                                        : (*modifier_cfg == "ref") ? reference
                                                                   : (*modifier_cfg
                                                                      == "preference") ? preference : none;
        SIGHT_THROW_IF(
            "\"src\" attribute is empty or invalid: " << std::quoted(*modifier_cfg) << ". "
            << "Must be \"new\", \"deferred\", \"ref\", or \"preference\".",
            modifier == none
        );

        FW_DEPRECATED_MSG(
            "'src' attribute for <object> will be removed. For deferred objects, please use deferred=\"true\" "
            "instead of src=\"deferred\", for preferences use preference=\"true\" and to pass objects as src=\"ref\", "
            "just pass them as parameters, it is no longer required to declare them twice.",
            "26.0"
        );
    }

    if(modifier == deferred)
    {
        SIGHT_THROW_IF("Missing attribute \"id\".", !id.second);
        [[maybe_unused]] const auto ret = _objects.deferred.insert(id.first);
        SIGHT_ASSERT("Object '" + id.first + "' already exists in this config.", ret.second);
    }
    else
    {
        // Creation of a new object
        data::object::sptr obj;

        // Create new or get the referenced object
        if(modifier == reference)
        {
            SIGHT_THROW_IF("Missing attribute \"id\".", !id.second);
            obj = get_object(type, id.first);
        }
        else
        {
            obj = get_new_object(type, id);

            // Get the object parser associated with the object type
            const auto srv_factory = service::extension::factory::get();

            std::string srv_impl = srv_factory->get_default_implementation_id_from_object_and_type(
                obj->get_classname(),
                "sight::service::object_parser"
            );

            service::base::sptr srv                 = srv_factory->create(srv_impl);
            service::object_parser::sptr obj_parser = std::dynamic_pointer_cast<service::object_parser>(srv);

            // If we have an extension config, use it to parse the object
            if(const auto data_config_id = config.get_optional<std::string>("<xmlattr>.config"); data_config_id)
            {
                const auto data_config = data::extension::config::get()->get_data_config(
                    *data_config_id,
                    type.second ? type.first : ""
                );

                obj_parser->parse(data_config, obj, _objects);
            }
            else
            {
                obj_parser->parse(config, obj, _objects);
            }
        }

        // If there is no uid defined in the config, we use the one generated from get_id()
        const auto real_id = id.second ? id.first : obj->get_id();
        _objects.created[real_id] = obj;

        if(modifier == preference)
        {
            SIGHT_THROW_IF("Missing attribute \"id\".", !id.second);
            [[maybe_unused]] const auto ret = _objects.prefs.insert(id.first);
            SIGHT_ASSERT("Object '" + id.first + "' already exists in this config.", ret.second);
        }
    }
}

// ------------------------------------------------------------------------

data::object::sptr config::get_new_object(config_attribute_t _type, config_attribute_t _uid)
{
    // Building object structure
    SPTR(core::runtime::extension) ext = core::runtime::find_extension(_type.first);
    if(ext)
    {
        const std::string class_name = core::get_classname<data::object>();
        SIGHT_ASSERT(
            "Extension and classname are different.",
            ext->point() == class_name
        );

        // Start dll to retrieve proxy and register object
        ext->get_module()->start();
    }

    data::object::sptr obj = data::factory::make(_type.first);
    SIGHT_ASSERT("factory failed to build object : " + _type.first, obj);

    if(_uid.second)
    {
        SIGHT_ASSERT("Object already has an UID.", !obj->has_id());
        SIGHT_ASSERT("UID " << _uid.first << " already exists.", !core::id::exist(_uid.first));
        obj->set_id(_uid.first);
    }

    return obj;
}

// ------------------------------------------------------------------------

data::object::sptr config::get_object(config_attribute_t _type, const std::string& _uid)
{
    SIGHT_THROW_IF("Object with UID \"" + _uid + "\" doesn't exist.", !core::id::exist(_uid));
    auto obj = std::dynamic_pointer_cast<data::object>(core::id::get_object(_uid));

    SIGHT_THROW_IF("The UID '" + _uid + "' does not reference any object.", !obj);

    if(_type.second)
    {
        SIGHT_THROW_IF(
            "Object with UID \"" + _uid
            + "\" has a different type (\"" + obj->get_classname() + "\" != \"" + _type.first + "\").",
            !obj->is_a(_type.first)
        );
    }

    return obj;
}

//-----------------------------------------------------------------------------

app::detail::service_config config::parse_service(
    const boost::property_tree::ptree& _srv_elem,
    const std::string& _err_msg_head,
    const objects_set_t& _objects
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
    srvconfig.m_global_auto_connect = core::ptree::get_value(_srv_elem, "<xmlattr>.auto_connect", true);

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
    for(const auto& data_keyword : {"in", "out", "inout"})
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
            auto key_cfgs                   = cfg.second.equal_range("key");
            const std::string& key          = group.value();
            const auto default_optional_cfg = is_key_optional(srvconfig.m_type, key);

            objconfig.m_auto_connect = cfg.second.get_optional<bool>("<xmlattr>.auto_connect");

            // Optional is global to all keys in the group
            if(objconfig.m_access != data::access::out)
            {
                objconfig.m_optional = core::ptree::get_value(
                    cfg.second,
                    "<xmlattr>.optional",
                    default_optional_cfg
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
                const auto auto_connect = group_cfg->second.get_optional<bool>("<xmlattr>.auto_connect");
                group_objconfig.m_auto_connect = auto_connect.has_value() ? auto_connect : objconfig.m_auto_connect;

                // Optional can be overriden by element in the group
                if(group_objconfig.m_access != data::access::out)
                {
                    group_objconfig.m_optional = core::ptree::get_value(
                        group_cfg->second,
                        "<xmlattr>.optional",
                        objconfig.m_optional
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

            const auto default_optional_cfg = is_key_optional(srvconfig.m_type, objconfig.m_key);

            // AutoConnect
            objconfig.m_auto_connect = cfg.second.get_optional<bool>("<xmlattr>.auto_connect");

            // Optional
            if(objconfig.m_access != data::access::out)
            {
                objconfig.m_optional = core::ptree::get_value(
                    cfg.second,
                    "<xmlattr>.optional",
                    default_optional_cfg
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

    // Collect all properties configurations
    std::vector<std::pair<std::string, std::string> > properties_cfgs;
    if(const auto& properties = srvconfig.m_config.get_child_optional("properties"); properties.has_value())
    {
        if(const auto& attributes = properties->get_child_optional("<xmlattr>"); attributes.has_value())
        {
            for(const auto& attribute : *attributes)
            {
                properties_cfgs.emplace_back(attribute.first, attribute.second.get_value<std::string>());
            }
        }

        auto properties_attrs = properties->equal_range("property");
        for(auto it_prop = properties_attrs.first ; it_prop != properties_attrs.second ; ++it_prop)
        {
            if(auto obj_cfg = it_prop->second.get_child_optional("<xmlattr>"); obj_cfg.has_value())
            {
                // We take only the first element
                auto first_element = *obj_cfg->begin();
                properties_cfgs.emplace_back(first_element.first, first_element.second.get_value<std::string>());
            }
        }
    }

    const auto parse_object_property =
        [&_objects](const auto& _uid) -> std::optional<std::string>
        {
            std::vector<std::string> tokens;
            boost::split(tokens, _uid, boost::is_any_of("."));

            if(auto it = _objects.find(tokens[0]); it != _objects.end())
            {
                if(tokens.size() > 1)
                {
                    if(auto map = std::dynamic_pointer_cast<sight::data::map>(it->second); map != nullptr)
                    {
                        return (*map)[tokens[1]]->get_id();
                    }
                }

                return _uid;
            }

            return {};
        };

    for(auto&& [key, value] : properties_cfgs)
    {
        const auto uid = parse_object_property(value);
        if(uid.has_value())
        {
            app::detail::object_serviceconfig objconfig
            {
                .m_key          = key,
                .m_uid          = *uid,
                .m_access       = data::access::inout,
                .m_auto_connect = key != "from",
                .m_optional     = false
            };

            srvconfig.m_objects[{objconfig.m_key, std::nullopt}] = objconfig;
        }
    }

    return srvconfig;
}

//------------------------------------------------------------------------------

bool config::is_key_optional(const std::string& _service_type, const std::string& _key)
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

    return service::manager::is_key_optional(srv, _key);
}

// ----------------------------------------------------------------------------

void config::clear_props()
{
    std::lock_guard guard(s_services_props_mutex);
    s_services_props.clear();
}

// ----------------------------------------------------------------------------

} // namespace sight::app::helper
