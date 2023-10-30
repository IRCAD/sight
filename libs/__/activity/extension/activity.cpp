/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "activity.hpp"

#include <core/runtime/module.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/activity.hpp>
#include <data/vector.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <limits>
#include <regex>

namespace sight::activity::extension
{

activity_config_param::activity_config_param(const config_t& _config) :
    replace(_config.get<std::string>("<xmlattr>.replace")),
    // @deprecated This is no longer necessary to use "uid" to get the prefix replacement, since
    // this is now done in config. However we keep that code for a while for backward compatibility
    by(_config.get_optional<std::string>("<xmlattr>.uid").get_value_or(""))
{
    if(by.empty())
    {
        by = _config.get<std::string>("<xmlattr>.by");
    }
}

//-----------------------------------------------------------------------------

activity_config::activity_config(const config_t& _config) :
    id(_config.get<std::string>("<xmlattr>.id"))
{
    if(_config.count("parameters") == 1)
    {
        const config_t& config_parameters = _config.get_child("parameters");

        for(const auto& v : boost::make_iterator_range(config_parameters.equal_range("parameter")))
        {
            activity_config_param parameter(v.second);
            parameters.push_back(parameter);
        }
    }

    SIGHT_ASSERT("At most 1 <parameters> tag is allowed", _config.count("parameters") < 2);
}

//-----------------------------------------------------------------------------

activity_requirement_key::activity_requirement_key(const config_t& _config) :
    key(_config.get_value<std::string>())
{
}

//-----------------------------------------------------------------------------

activity_requirement::activity_requirement(const config_t& _config) :
    name(_config.get<std::string>("<xmlattr>.name")),
    type(_config.get<std::string>("<xmlattr>.type")),
    container(_config.get<std::string>("<xmlattr>.container", "")),
    description(_config.get<std::string>("desc", "")),
    validator(_config.get<std::string>("validator", "")),
    min_occurs(_config.get<unsigned int>("<xmlattr>.minOccurs", 1)),
    max_occurs(_config.get<unsigned int>("<xmlattr>.maxOccurs", 1)),
    object_config(_config.get_child("config", config_t()))
{
    for(const auto& v : boost::make_iterator_range(_config.equal_range("key")))
    {
        keys.emplace_back(v.second);
    }

    if(_config.get<std::string>("<xmlattr>.maxOccurs", "") == "*")
    {
        this->max_occurs = std::numeric_limits<unsigned int>::max();
    }

    const std::string& create_str = _config.get<std::string>("<xmlattr>.create", "false");
    SIGHT_ASSERT("'create' attribute must be 'true' or 'false'", create_str == "true" || create_str == "false");
    create = (create_str == "true");
    SIGHT_ASSERT(
        "Create option is only available if minOccurs = 0 and maxOccurs = 1",
        !create || (min_occurs == 0 && max_occurs == 1)
    );

    SIGHT_ASSERT(
        "minOccurs value shall be equal or greater than 0 and lower or equal to maxOccurs (" << max_occurs << ")",
        min_occurs <= max_occurs
    );

    const std::string& reset_string = _config.get<std::string>("<xmlattr>.reset", "false");
    SIGHT_ASSERT("'reset' attribute must be 'true' or 'false'", reset_string == "true" || reset_string == "false");

    reset = (reset_string == "true");

    SIGHT_ASSERT(
        "'Reset' option is only available if 'create' = 'true' or 'minOccurs' = '0'",
        !reset || create || min_occurs == 0
    );
}

//-----------------------------------------------------------------------------

activity_info::activity_info(const SPTR(core::runtime::extension)& _ext) :
    id(_ext->get_config().get<std::string>("id")),
    title(_ext->get_config().get<std::string>("title")),
    description(_ext->get_config().get<std::string>("desc")),
    icon(core::runtime::get_module_resource_file_path(_ext->get_config().get<std::string>("icon")).string()),
    tab_info(title),
    bundle_id(_ext->get_module()->identifier()),
    app_config(_ext->get_config().get_child("appConfig"))
{
    const auto& config = _ext->get_config();
    tab_info = config.get<std::string>("tabinfo", tab_info);

    if(const auto& requirements_cfg = config.get_child_optional("requirements"); requirements_cfg.has_value())
    {
        for(const auto& elem : boost::make_iterator_range(requirements_cfg->equal_range("requirement")))
        {
            activity_requirement requirement(elem.second);
            requirements.push_back(requirement);

            min_max_t& min_max = m_requirement_count[requirement.type];

            min_max.first += requirement.min_occurs;

            if(requirement.max_occurs < (std::numeric_limits<unsigned int>::max() - min_max.second))
            {
                min_max.second += requirement.max_occurs;
            }
            else
            {
                min_max.second = std::numeric_limits<unsigned int>::max();
            }
        }
    }

    builder_impl = config.get<std::string>("builder", "sight::activity::builder::Activity");

    // backward compatibility
    if(const auto& validator_cfg = config.get_optional<std::string>("validator"); validator_cfg.has_value())
    {
        const auto& validator_impl_str = validator_cfg.value();
        if(!validator_impl_str.empty())
        {
            validators_impl.push_back(validator_impl_str);
        }
    }

    if(const auto& validators = config.get_child_optional("validators"); validators.has_value())
    {
        for(const auto& validator : boost::make_iterator_range(validators->equal_range("validator")))
        {
            validators_impl.push_back(validator.second.get_value<std::string>());
        }
    }

    // Set Default validator if none is defined
    if(validators_impl.empty())
    {
        validators_impl.emplace_back("sight::activity::validator::default_activity");
    }
}

//-----------------------------------------------------------------------------

bool activity_info::usable_with(data_count_t _data_counts) const
{
    bool ok = _data_counts.size() <= m_requirement_count.size();

    if(ok)
    {
        for(const auto& req_count : m_requirement_count)
        {
            const min_max_t& req_min_max = req_count.second;
            auto iter                    = _data_counts.find(req_count.first);
            if(iter != _data_counts.end())
            {
                unsigned int data_count = iter->second;
                ok = (data_count != 0U) && req_min_max.first <= data_count && data_count <= req_min_max.second;
                _data_counts.erase(iter);
            }
            else
            {
                ok = (req_min_max.first == 0);
            }

            if(!ok)
            {
                break;
            }
        }

        if(ok)
        {
            for(const auto& data_count : _data_counts)
            {
                if(m_requirement_count.find(data_count.first) == m_requirement_count.end())
                {
                    ok = false;
                    break;
                }
            }
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

activity::sptr activity::get_default()
{
    static auto s_activity = std::make_shared<sight::activity::extension::activity>();
    return s_activity;
}

//-----------------------------------------------------------------------------

void activity::parse_plugin_infos()
{
    std::vector<SPTR(core::runtime::extension)> extensions =
        core::runtime::get_all_extensions_for_point("sight::activity::extension");

    for(const SPTR(core::runtime::extension) & ext : extensions)
    {
        SIGHT_DEBUG("Parsing <" << ext->get_module()->identifier() << "> Activity");
        activity_info info(ext);

        core::mt::write_lock lock(m_registry_mutex);
        SIGHT_ASSERT(
            "The id " << info.id << "(" << info.title << ")"
            << " already exists in the Activity registry",
            m_reg.find(info.id) == m_reg.end()
        );
        m_reg.insert(registry::value_type(info.id, info));
    }
}

//-----------------------------------------------------------------------------

void activity::clear_registry()
{
    core::mt::write_lock lock(m_registry_mutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

bool activity::has_info(const std::string& _extension_id) const
{
    core::mt::read_lock lock(m_registry_mutex);
    auto iter = m_reg.find(_extension_id);
    return iter != m_reg.end();
}

//-----------------------------------------------------------------------------

std::vector<activity_info> activity::get_infos() const
{
    std::vector<activity_info> infos;

    core::mt::read_lock lock(m_registry_mutex);

    for(const auto& val : m_reg)
    {
        infos.push_back(val.second);
    }

    return infos;
}

//-----------------------------------------------------------------------------

activity_info::data_count_t activity::get_data_count(const data::vector::csptr& _data)
{
    activity_info::data_count_t data_count;

    for(const data::object::csptr obj : *_data)
    {
        ++data_count[obj->get_classname()];
    }

    return data_count;
}

//-----------------------------------------------------------------------------

std::vector<activity_info> activity::get_infos(const data::vector::csptr& _data) const
{
    activity_info::data_count_t data_count = this->get_data_count(_data);
    std::vector<activity_info> infos;

    core::mt::read_lock lock(m_registry_mutex);

    for(const auto& reg_value : m_reg)
    {
        const activity_info& activity = reg_value.second;
        if(activity.usable_with(data_count))
        {
            infos.push_back(activity);
        }
    }

    return infos;
}

//-----------------------------------------------------------------------------

std::vector<std::string> activity::get_keys() const
{
    std::vector<std::string> keys;

    core::mt::read_lock lock(m_registry_mutex);

    for(const auto& val : m_reg)
    {
        keys.push_back(val.first);
    }

    return keys;
}

//-----------------------------------------------------------------------------

activity_info activity::get_info(const std::string& _extension_id) const
{
    core::mt::read_lock lock(m_registry_mutex);
    auto iter = m_reg.find(_extension_id);
    SIGHT_ASSERT(
        "The id " << _extension_id << " is not found in the application configuration parameter registry",
        iter != m_reg.end()
    );
    return iter->second;
}

//-----------------------------------------------------------------------------

std::tuple<activity_info, std::map<std::string, std::string> > activity::get_info_and_replacement_map(
    const data::activity& _activity,
    const activity_config_params_type& _parameters
) const
{
    // Retrieve the activity informations
    const auto& info = get_info(_activity.get_activity_config_id());

    return {info, get_replacement_map(_activity, info, _parameters)};
}

//------------------------------------------------------------------------------

std::map<std::string, std::string> activity::get_replacement_map(
    const data::activity& _activity,
    const activity_info& _info,
    const activity_config_params_type& _parameters
)
{
    std::map<std::string, std::string> replacement_map;

    // First, use requirements to populate replacement map with an object from the root composite
    for(const auto& requirement : _info.requirements)
    {
        // Use the name as "key" for the config parameter
        const auto& it = _activity.find(requirement.name);

        if(it != _activity.end() && it->second)
        {
            replacement_map[requirement.name] = it->second->get_id();
        }
    }

    const auto add_parameters =
        [&replacement_map](const auto& _parameters)
        {
            for(const auto& parameter : _parameters)
            {
                // @TODO This should be removed later when legacy xml code is cleaned.
                SIGHT_FATAL_IF(
                    "Cannot replace '" << parameter.replace << "' by CAMP path '" << parameter.by << "'.",
                    parameter.is_object_path()
                );

                replacement_map[parameter.replace] = parameter.by;
            }
        };

    // Then, use config parameters
    // Parameters will override already present requirements with the same name
    add_parameters(_info.app_config.parameters);

    // Finally, use additional parameters...
    add_parameters(_parameters);

    // Store the activity UID
    replacement_map["AS_UID"] = _activity.get_id();

    return replacement_map;
}

} // namespace sight::activity::extension
