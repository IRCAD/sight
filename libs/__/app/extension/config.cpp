/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/id.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/module.hpp>
#include <core/runtime/runtime.hpp>

#include <data/map.hpp>
#include <data/string.hpp>

#include <boost/algorithm/string.hpp>

#include <regex>

namespace sight::app::extension
{

std::string config::s_mandatory_parameter_identifier = "@mandatory@";

config::uid_definition_t config::s_uid_definition_dictionary = {{"object", "uid"},
    {"service", "uid"},
    {"view", "sid"},
    {"view", "wid"},
    {"editor", "wid"},
    {"slideView", "wid"},
    {"connect", "channel"},
    {"menuItem", "sid"},
    {"channel", "uid"},
};
static const std::regex IS_VARIABLE(R"(\$\{.*\}.*)");

//-----------------------------------------------------------------------------

config::sptr config::get()
{
    static config::sptr s_current_app_config = std::make_shared<config>();
    return s_current_app_config;
}

//-----------------------------------------------------------------------------

void config::parse_plugin_infos()
{
    auto extensions = core::runtime::get_all_extensions_for_point("sight::app::extension::config");
    for(const auto& ext : extensions)
    {
        const auto& app_config = ext->get_config();

        const auto config_id = app_config.get<std::string>("id");
        const auto group     = app_config.get<std::string>("group", "");
        const auto desc      = app_config.get<std::string>("desc", "No description available");

        app_info::parameters_t parameters;
        app_info::objects_t objects;

        if(const auto parameters_cfg = app_config.get_child_optional("parameters"); parameters_cfg.has_value())
        {
            for(const auto& param : boost::make_iterator_range(parameters_cfg->equal_range("param")))
            {
                const auto name = param.second.get<std::string>("<xmlattr>.name");

                SIGHT_ASSERT("Parameter " << std::quoted(name) << " already declared", !parameters.contains(name));
                SIGHT_ASSERT(
                    "Parameter " << std::quoted(name) << " already declared as object",
                    !objects.contains(name)
                );
                parameters[name] = param.second.get<std::string>("<xmlattr>.default", s_mandatory_parameter_identifier);
            }

            for(const auto& object : boost::make_iterator_range(parameters_cfg->equal_range("object")))
            {
                const auto uid = object.second.get<std::string>("<xmlattr>.uid");

                SIGHT_ASSERT("Object " << std::quoted(uid) << " already declared", !objects.contains(uid));
                SIGHT_ASSERT(
                    "Object " << std::quoted(uid) << " already declared as parameter",
                    !parameters.contains(uid)
                );

                objects[uid] = {
                    object.second.get<std::string>("<xmlattr>.type"),
                    object.second.get<bool>("<xmlattr>.optional", false)
                };
            }
        }

        // Get config
        const auto config = app_config.get_child("config");

        // Get module
        std::shared_ptr<core::runtime::module> module = ext->get_module();
        std::string module_id                         = module->identifier();

        // Add app info
        this->add_app_info(config_id, group, desc, parameters, objects, config, module_id);
    }
}

//-----------------------------------------------------------------------------

void config::add_app_info(
    const std::string& _config_id,
    const std::string& _group,
    const std::string& _desc,
    const app_info::parameters_t& _parameters,
    const app_info::objects_t& _objects,
    const core::runtime::config_t& _config,
    const std::string& _module_id
)
{
    core::mt::write_lock lock(m_registry_mutex);

    SIGHT_DEBUG("New app config registering : configId = " + _config_id);
    SIGHT_ASSERT(
        "The app config with the id = " << _config_id << " already exist.",
        m_reg.find(_config_id) == m_reg.end()
    );

    app_info::sptr info = std::make_shared<app_info>();
    info->group       = _group;
    info->desc        = _desc;
    info->config      = _config;
    info->parameters  = _parameters;
    info->objects     = _objects;
    info->module_id   = _module_id;
    m_reg[_config_id] = info;
}

//-----------------------------------------------------------------------------

void config::clear_registry()
{
    core::mt::write_lock lock(m_registry_mutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

core::runtime::config_t config::get_adapted_template_config(
    const std::string& _config_id,
    const field_adaptor_t _field_adaptors,
    bool _auto_prefix_id
)
{
    core::mt::read_lock lock(m_registry_mutex);
    // Get config template
    auto iter = m_reg.find(_config_id);
    SIGHT_ASSERT(
        "The id " << _config_id << " is not found in the application configuration registry",
        iter != m_reg.end()
    );

    // Adapt config
    core::runtime::config_t new_config = iter->second->config;

    field_adaptor_t fields;
    const auto parse_key = [](const std::string& _s){return "\\$\\{" + _s + "\\}";};

    for(const auto& param : iter->second->parameters)
    {
        auto iter_field       = _field_adaptors.find(param.first);
        const std::string key = parse_key(param.first);
        if(iter_field != _field_adaptors.end())
        {
            fields[key] = iter_field->second;
        }
        else if(param.second != s_mandatory_parameter_identifier)
        {
            fields[key] = param.second;
        }
        else
        {
            SIGHT_THROW("[" << _config_id << "] parameter : " << std::quoted(param.first) << " is needed");
        }
    }

    for(const auto& object : iter->second->objects)
    {
        auto iter_field       = _field_adaptors.find(object.first);
        const std::string key = parse_key(object.first);
        if(iter_field != _field_adaptors.end())
        {
            fields[key] = iter_field->second;
        }
        else
        {
            if(object.second.second)
            {
                SIGHT_INFO(
                    "[" << _config_id << "] optional parameter " << std::quoted(object.first)
                    << " not provided"
                );
            }
            else
            {
                SIGHT_THROW("[" << _config_id << "] parameter : " << std::quoted(object.first) << " is needed");
            }
        }

        core::runtime::config_t object_ref_cfg;
        object_ref_cfg.put("<xmlattr>.uid", "${" + object.first + "}");
        object_ref_cfg.put("<xmlattr>.type", object.second.first);
        object_ref_cfg.put("<xmlattr>.src", object.second.second ? "deferred" : "ref");
        new_config.add_child("object", object_ref_cfg);
    }

    auto object_cfgs = new_config.equal_range("object");

    std::string auto_prefix_name;
    if(_auto_prefix_id)
    {
        auto_prefix_name = sight::app::extension::config::get_unique_identifier(_config_id);

        for(auto it = object_cfgs.first ; it != object_cfgs.second ; ++it)
        {
            if(const auto& attributes = it->second.get_child_optional("<xmlattr>"); attributes.has_value())
            {
                for(const auto& attribute : *attributes)
                {
                    const auto attr_value = attribute.second.get_value<std::string>();
                    if("uid" == attribute.first && !std::regex_match(attr_value, IS_VARIABLE))
                    {
                        fields[parse_key(attr_value)] = core::id::join(auto_prefix_name, attr_value);
                    }
                }
            }
        }
    }

    uid_parameter_replace_t parameter_replace_adaptors;
    sight::app::extension::config::collect_uid_for_parameter_replace(
        "config",
        new_config,
        parameter_replace_adaptors
    );
    new_config = sight::app::extension::config::adapt_config(
        new_config,
        fields,
        parameter_replace_adaptors,
        auto_prefix_name
    );

    return new_config;
}

//-----------------------------------------------------------------------------

std::shared_ptr<core::runtime::module> config::get_module(const std::string& _config_id)
{
    auto iter = m_reg.find(_config_id);
    SIGHT_ASSERT(
        "The id " << _config_id << " is not found in the application configuration registry",
        iter != m_reg.end()
    );

    auto module = core::runtime::find_module(iter->second->module_id);

    return module;
}

//-----------------------------------------------------------------------------

std::vector<std::string> config::get_all_configs() const
{
    core::mt::read_lock lock(m_registry_mutex);
    std::vector<std::string> ids;
    for(const auto& elem : m_reg)
    {
        ids.push_back(elem.first);
    }

    return ids;
}

//-----------------------------------------------------------------------------

std::vector<std::string> config::get_configs_from_group(const std::string& _group) const
{
    core::mt::read_lock lock(m_registry_mutex);
    std::vector<std::string> ids;
    for(const auto& elem : m_reg)
    {
        app_info::sptr info = elem.second;
        if(info->group == _group)
        {
            ids.push_back(elem.first);
        }
    }

    return ids;
}

//-----------------------------------------------------------------------------

field_adaptor_t config::map_to_field_adaptor(data::map::csptr _field_adaptors)
{
    field_adaptor_t fields;
    for(const auto& elem : *_field_adaptors)
    {
        fields[elem.first] = std::dynamic_pointer_cast<data::string>(elem.second)->value();
    }

    return fields;
}

//-----------------------------------------------------------------------------

std::string config::get_unique_identifier(const std::string& _service_uid)
{
    static std::mutex s_id_mutex;
    std::unique_lock lock(s_id_mutex);

    static unsigned int srv_cpt = 1;

    return core::id::join(
        _service_uid.empty() ? "config_manager" : _service_uid,
        srv_cpt++
    );
}

//-----------------------------------------------------------------------------

void config::collect_uid_for_parameter_replace(
    const std::string& _name,
    const core::runtime::config_t& _cfg_elem,
    uid_parameter_replace_t& _replace_map
)
{
    const auto& attributes = _cfg_elem.get_child_optional("<xmlattr>");

    if(attributes)
    {
        for(const auto& attribute : *attributes)
        {
            auto range = s_uid_definition_dictionary.equal_range(_name);

            for(auto it = range.first ; it != range.second ; ++it)
            {
                const auto attr_value = attribute.second.get_value<std::string>();
                if(it->second == attribute.first && !std::regex_match(attr_value, IS_VARIABLE))
                {
                    _replace_map.insert(attr_value);
                }
            }
        }
    }

    // Check if a service if used only on signal/slot
    for(const auto& sub_elem : _cfg_elem)
    {
        if(sub_elem.first == "signal" || sub_elem.first == "slot")
        {
            std::vector<std::string> tokens;
            boost::split(tokens, sub_elem.second.get_value<std::string>(), boost::is_any_of("/"));
            _replace_map.insert(tokens[0]);
        }

        collect_uid_for_parameter_replace(sub_elem.first, sub_elem.second, _replace_map);
    }
}

//-----------------------------------------------------------------------------

core::runtime::config_t config::adapt_config(
    const core::runtime::config_t& _cfg_elem,
    const field_adaptor_t& _field_adaptors,
    const uid_parameter_replace_t& _uid_parameter_replace,
    const std::string& _auto_prefix_id
)
{
    core::runtime::config_t result;
    result.put_value<std::string>(subst_var(_cfg_elem.get_value<std::string>(), _field_adaptors));

    if(const auto& attributes = _cfg_elem.get_child_optional("<xmlattr>"); attributes.has_value())
    {
        for(const auto& attribute : *attributes)
        {
            const auto attribute_value = attribute.second.get_value<std::string>();

            // Add the config prefix for unique identifiers
            if(!_auto_prefix_id.empty())
            {
                if(attribute.first == "uid"
                   || attribute.first == "sid"
                   || attribute.first == "wid"
                   || attribute.first == "channel")
                {
                    // Detect if we have a variable name
                    if(!std::regex_match(attribute_value, IS_VARIABLE))
                    {
                        // This is not a variable, add the prefix
                        result.put("<xmlattr>." + attribute.first, core::id::join(_auto_prefix_id, attribute_value));
                        continue;
                    }
                }
                // Special case for <parameter replace="..." by="..." />
                else if(attribute.first == "by")
                {
                    // Detect if we have a variable name
                    if(!std::regex_match(attribute_value, IS_VARIABLE))
                    {
                        // Look inside the map of potential replacements
                        auto it_param = _uid_parameter_replace.find(attribute_value);
                        if(it_param != _uid_parameter_replace.end())
                        {
                            result.put(
                                "<xmlattr>." + attribute.first,
                                core::id::join(_auto_prefix_id, attribute_value)
                            );
                            continue;
                        }
                    }
                }
            }

            result.put("<xmlattr>." + attribute.first, subst_var(attribute_value, _field_adaptors));
        }
    }

    for(const auto& sub_elem : _cfg_elem)
    {
        // Add the config prefix for unique identifiers in signal and slot sources
        if(!_auto_prefix_id.empty() && (sub_elem.first == "signal" || sub_elem.first == "slot"))
        {
            // Detect if we have a variable name
            if(!std::regex_match(sub_elem.second.get_value<std::string>(), IS_VARIABLE))
            {
                // This is not a variable, add the prefix
                core::runtime::config_t elt;
                elt.put_value(core::id::join(_auto_prefix_id, sub_elem.second.get_value<std::string>()));

                const auto& sub_attributes = _cfg_elem.get_child_optional("<xmlattr>");

                if(sub_attributes)
                {
                    for(const auto& attribute : *sub_attributes)
                    {
                        elt.put("<xmlattr>." + attribute.first, attribute.second.get_value<std::string>());
                    }
                }

                result.add_child(sub_elem.first, elt);
                continue;
            }
        }

        result.add_child(
            sub_elem.first,
            adapt_config(sub_elem.second, _field_adaptors, _uid_parameter_replace, _auto_prefix_id)
        );
    }

    return result;
}

//-----------------------------------------------------------------------------

std::string config::subst_var(const std::string& _str, const field_adaptor_t& _variables_map)
{
    std::string new_str = _str;
    if(!_str.empty())
    {
        // Discriminate first variable expressions only, looking through all keys of the replace map is not for free
        // However we look inside the whole string instead of only at the beginning because we want  to replace "inner"
        // variables as well, i.e. not only ${uid} but also uid${suffix}
        if(std::regex_search(_str, IS_VARIABLE))
        {
            // Iterate over all variables
            for(const auto& field_adaptor : _variables_map)
            {
                const std::regex var_regex("(.*)" + field_adaptor.first + "(.*)");
                if(std::regex_match(new_str, var_regex))
                {
                    const std::string var_replace("\\1" + field_adaptor.second + "\\2");
                    new_str = std::regex_replace(
                        new_str,
                        var_regex,
                        var_replace,
                        std::regex_constants::match_default
                        | std::regex_constants::format_sed
                    );
                }

                // Do not break to allow multiple replacements if there are other variables in the string
            }
        }
    }

    return new_str;
}

//-----------------------------------------------------------------------------

} // namespace sight::app::extension
