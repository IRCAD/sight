/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "config.hpp"

#include <core/runtime/runtime.hpp>

namespace sight::data::extension
{

config::sptr config::s_data_config = std::make_shared<config>();

//-----------------------------------------------------------------------------

config::sptr config::get()
{
    return s_data_config;
}

//-----------------------------------------------------------------------------

void config::parse_plugin_infos()
{
    using extension_t = std::shared_ptr<core::runtime::extension>;

    const auto ext_elements = core::runtime::get_all_extensions_for_point("sight::data::extension::config");

    for(const extension_t& ext : ext_elements)
    {
        const auto& cfg = ext->get_config();

        const auto config_id = cfg.get<std::string>("id");
        const auto desc      = cfg.get<std::string>("desc", "No description available");
        const auto object    = cfg.get<std::string>("object", "");

        // Get config
        const auto config = cfg.get_child("config");

        // Add data config info
        this->add_data_config_info(config_id, object, desc, config);
    }
}

//-----------------------------------------------------------------------------

void config::add_data_config_info
(
    const std::string& _config_id,
    const std::string& _data,
    const std::string& _desc,
    const boost::property_tree::ptree& _config
)
{
    std::unique_lock lock(m_registry_mutex);

    SIGHT_DEBUG(
        "New data config registering : "
        << " configId = " << _config_id
        << " data = " << _data
        << " desc = " << _desc
    );

    SIGHT_ASSERT(
        "The data config with the id " << _config_id << " already exists.",
        m_registry.find(_config_id) == m_registry.end()
    );

    data_config_info info {
        .data   = sight::core::runtime::filter_id(_data),
        .desc   = _desc,
        .config = _config,
    };
    m_registry[_config_id] = info;
}

//-----------------------------------------------------------------------------

void config::clear_registry()
{
    std::unique_lock lock(m_registry_mutex);
    m_registry.clear();
}

//-----------------------------------------------------------------------------

boost::property_tree::ptree config::get_data_config(
    const std::string& _config_id,
    const std::string& _data_impl
) const
{
#ifndef _DEBUG
    SIGHT_NOT_USED(_data_impl);
#else
    const std::string data_impl = core::runtime::filter_id(_data_impl);
#endif
    std::shared_lock lock(m_registry_mutex);
    auto iter = m_registry.find(_config_id);
    SIGHT_ASSERT(
        "The id " << _config_id << " is not found in the application configuration registry",
        iter != m_registry.end()
    );
    SIGHT_ASSERT(
        "The id " << _config_id << " is not allowed for this data " << data_impl,
        data_impl.empty() || iter->second.data.empty() || iter->second.data == data_impl
    );
    return iter->second.config;
}

//-----------------------------------------------------------------------------

const std::string& config::get_config_desc(const std::string& _config_id) const
{
    std::shared_lock lock(m_registry_mutex);
    auto iter = m_registry.find(_config_id);
    SIGHT_ASSERT(
        "The id " << _config_id << " is not found in the application configuration registry",
        iter != m_registry.end()
    );
    return iter->second.desc;
}

//-----------------------------------------------------------------------------

std::vector<std::string> config::get_all_config_for_data(std::string _data_impl, bool _matching_only) const
{
    const std::string data_impl = core::runtime::filter_id(_data_impl);
    std::shared_lock lock(m_registry_mutex);
    std::vector<std::string> configs;

    for(const auto& srv_cfg : m_registry)
    {
        const data_config_info& info = srv_cfg.second;
        if((info.data.empty() && !_matching_only) || info.data == data_impl)
        {
            configs.push_back(srv_cfg.first);
        }
    }

    return configs;
}

} // namespace sight::data::extension
