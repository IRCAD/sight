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

#include "service/extension/config.hpp"

#include <core/runtime/runtime.hpp>

namespace sight::service::extension
{

const std::string config::CONFIG_EXT_POINT = "sight::service::extension::config";

config::sptr config::s_current_service_config = std::make_shared<config>();

//-----------------------------------------------------------------------------

config::sptr config::get_default()
{
    return s_current_service_config;
}

//-----------------------------------------------------------------------------

void config::parse_plugin_infos()
{
    using extension_t = std::shared_ptr<core::runtime::extension>;

    std::vector<extension_t> ext_elements;
    ext_elements = core::runtime::get_all_extensions_for_point(CONFIG_EXT_POINT);
    for(const extension_t& ext : ext_elements)
    {
        const auto& cfg = ext->get_config();

        const auto config_id = cfg.get<std::string>("id");
        const auto desc      = cfg.get<std::string>("desc", "No description available");
        const auto service   = cfg.get<std::string>("service", "");

        // Get config
        const auto config = cfg.get_child("config");

        // Add service config info
        this->add_service_config_info(config_id, service, desc, config);
    }
}

//-----------------------------------------------------------------------------

void config::add_service_config_info
(
    const std::string& _config_id,
    const std::string& _service,
    const std::string& _desc,
    const boost::property_tree::ptree& _config
)
{
    core::mt::write_lock lock(m_registry_mutex);

    SIGHT_DEBUG(
        "New service config registering : "
        << " configId = " << _config_id
        << " service = " << _service
        << " desc = " << _desc
    );

    SIGHT_ASSERT(
        "The service config with the id " << _config_id << " already exists.",
        m_reg.find(_config_id) == m_reg.end()
    );

    service_config_info::sptr info = std::make_shared<service_config_info>();
    info->service     = sight::core::runtime::filter_id(_service);
    info->desc        = _desc;
    info->config      = _config;
    m_reg[_config_id] = info;
}

//-----------------------------------------------------------------------------

void config::clear_registry()
{
    core::mt::write_lock lock(m_registry_mutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

boost::property_tree::ptree config::get_service_config(
    const std::string& _config_id,
    const std::string& _service_impl
) const
{
#ifndef _DEBUG
    SIGHT_NOT_USED(_service_impl);
#else
    const std::string service_impl = core::runtime::filter_id(_service_impl);
#endif
    core::mt::read_lock lock(m_registry_mutex);
    auto iter = m_reg.find(_config_id);
    SIGHT_ASSERT(
        "The id " << _config_id << " is not found in the application configuration registry",
        iter != m_reg.end()
    );
    SIGHT_ASSERT(
        "The id " << _config_id << " is not allowed for this service " << service_impl,
        service_impl.empty() || iter->second->service.empty() || iter->second->service == service_impl
    );
    return iter->second->config;
}

//-----------------------------------------------------------------------------

const std::string& config::get_config_desc(const std::string& _config_id) const
{
    core::mt::read_lock lock(m_registry_mutex);
    auto iter = m_reg.find(_config_id);
    SIGHT_ASSERT(
        "The id " << _config_id << " is not found in the application configuration registry",
        iter != m_reg.end()
    );
    return iter->second->desc;
}

//-----------------------------------------------------------------------------

std::vector<std::string> config::get_all_config_for_service(std::string _service_impl, bool _matching_only) const
{
    const std::string service_impl = core::runtime::filter_id(_service_impl);
    core::mt::read_lock lock(m_registry_mutex);
    std::vector<std::string> configs;

    for(const auto& srv_cfg : m_reg)
    {
        service_config_info::sptr info = srv_cfg.second;
        if((info->service.empty() && !_matching_only) || info->service == service_impl)
        {
            configs.push_back(srv_cfg.first);
        }
    }

    return configs;
}

//-----------------------------------------------------------------------------

} // namespace sight::service::extension
