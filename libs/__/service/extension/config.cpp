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

config::sptr config::s_currentServiceConfig = std::make_shared<config>();

//-----------------------------------------------------------------------------

config::sptr config::getDefault()
{
    return s_currentServiceConfig;
}

//-----------------------------------------------------------------------------

void config::parse_plugin_infos()
{
    using ExtensionType = std::shared_ptr<core::runtime::extension>;

    std::vector<ExtensionType> extElements;
    extElements = core::runtime::get_all_extensions_for_point(CONFIG_EXT_POINT);
    for(const ExtensionType& ext : extElements)
    {
        const auto& cfg = ext->get_config();

        const auto configId = cfg.get<std::string>("id");
        const auto desc     = cfg.get<std::string>("desc", "No description available");
        const auto service  = cfg.get<std::string>("service", "");

        // Get config
        const auto config = cfg.get_child("config");

        // Add service config info
        this->addServiceConfigInfo(configId, service, desc, config);
    }
}

//-----------------------------------------------------------------------------

void config::addServiceConfigInfo
(
    const std::string& configId,
    const std::string& service,
    const std::string& desc,
    const boost::property_tree::ptree& config
)
{
    core::mt::write_lock lock(m_registryMutex);

    SIGHT_DEBUG(
        "New service config registering : "
        << " configId = " << configId
        << " service = " << service
        << " desc = " << desc
    );

    SIGHT_ASSERT(
        "The service config with the id " << configId << " already exists.",
        m_reg.find(configId) == m_reg.end()
    );

    ServiceConfigInfo::sptr info = std::make_shared<ServiceConfigInfo>();
    info->service   = sight::core::runtime::filter_id(service);
    info->desc      = desc;
    info->config    = config;
    m_reg[configId] = info;
}

//-----------------------------------------------------------------------------

void config::clear_registry()
{
    core::mt::write_lock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

boost::property_tree::ptree config::get_service_config(
    const std::string& configId,
    const std::string& _serviceImpl
) const
{
#ifndef _DEBUG
    SIGHT_NOT_USED(_serviceImpl);
#else
    const std::string serviceImpl = core::runtime::filter_id(_serviceImpl);
#endif
    core::mt::read_lock lock(m_registryMutex);
    auto iter = m_reg.find(configId);
    SIGHT_ASSERT(
        "The id " << configId << " is not found in the application configuration registry",
        iter != m_reg.end()
    );
    SIGHT_ASSERT(
        "The id " << configId << " is not allowed for this service " << serviceImpl,
        serviceImpl.empty() || iter->second->service.empty() || iter->second->service == serviceImpl
    );
    return iter->second->config;
}

//-----------------------------------------------------------------------------

const std::string& config::getConfigDesc(const std::string& configId) const
{
    core::mt::read_lock lock(m_registryMutex);
    auto iter = m_reg.find(configId);
    SIGHT_ASSERT(
        "The id " << configId << " is not found in the application configuration registry",
        iter != m_reg.end()
    );
    return iter->second->desc;
}

//-----------------------------------------------------------------------------

std::vector<std::string> config::getAllConfigForService(std::string _serviceImpl, bool matchingOnly) const
{
    const std::string serviceImpl = core::runtime::filter_id(_serviceImpl);
    core::mt::read_lock lock(m_registryMutex);
    std::vector<std::string> configs;

    for(const Registry::value_type& srvCfg : m_reg)
    {
        ServiceConfigInfo::sptr info = srvCfg.second;
        if((info->service.empty() && !matchingOnly) || info->service == serviceImpl)
        {
            configs.push_back(srvCfg.first);
        }
    }

    return configs;
}

//-----------------------------------------------------------------------------

} // namespace sight::service::extension
