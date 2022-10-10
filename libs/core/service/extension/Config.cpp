/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "service/extension/Config.hpp"

#include <core/runtime/Convert.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/Runtime.hpp>

namespace sight::service::extension
{

const std::string Config::CONFIG_EXT_POINT = "sight::service::extension::Config";

Config::sptr Config::s_currentServiceConfig = Config::New();

//-----------------------------------------------------------------------------

Config::sptr Config::getDefault()
{
    return s_currentServiceConfig;
}

//-----------------------------------------------------------------------------

Config::~Config()
= default;

//-----------------------------------------------------------------------------

void Config::parseBundleInformation()
{
    using ExtensionType = std::shared_ptr<core::runtime::Extension>;

    std::vector<ExtensionType> extElements;
    extElements = core::runtime::getAllExtensionsForPoint(CONFIG_EXT_POINT);
    for(const ExtensionType& ext : extElements)
    {
        // Get id
        SIGHT_ASSERT("Missing id element", ext->hasConfigurationElement("id"));
        std::string id = ext->findConfigurationElement("id")->getValue();

        // Get service
        std::string service;
        if(ext->hasConfigurationElement("service"))
        {
            service = ext->findConfigurationElement("service")->getValue();
        }

        // Get desc
        std::string desc = "No description available";
        if(ext->hasConfigurationElement("desc"))
        {
            desc = ext->findConfigurationElement("desc")->getValue();
        }

        // Get config
        core::runtime::ConfigurationElement::csptr config = ext->findConfigurationElement("config");

        // Add service config info
        this->addServiceConfigInfo(
            id,
            service,
            desc,
            core::runtime::Convert::toPropertyTree(config).get_child("config")
        );
    }
}

//-----------------------------------------------------------------------------

void Config::addServiceConfigInfo
(
    const std::string& configId,
    const std::string& service,
    const std::string& desc,
    const boost::property_tree::ptree& config
)
{
    core::mt::WriteLock lock(m_registryMutex);

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

    ServiceConfigInfo::sptr info = ServiceConfigInfo::New();
    info->service   = core::runtime::filterID(service);
    info->desc      = desc;
    info->config    = config;
    m_reg[configId] = info;
}

//-----------------------------------------------------------------------------

Config::Config()
= default;

//-----------------------------------------------------------------------------

void Config::clearRegistry()
{
    core::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

boost::property_tree::ptree Config::getServiceConfig(
    const std::string& configId,
    const std::string& _serviceImpl
) const
{
#ifndef _DEBUG
    SIGHT_NOT_USED(_serviceImpl);
#else
    const std::string serviceImpl = core::runtime::filterID(_serviceImpl);
#endif
    core::mt::ReadLock lock(m_registryMutex);
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

const std::string& Config::getConfigDesc(const std::string& configId) const
{
    core::mt::ReadLock lock(m_registryMutex);
    auto iter = m_reg.find(configId);
    SIGHT_ASSERT(
        "The id " << configId << " is not found in the application configuration registry",
        iter != m_reg.end()
    );
    return iter->second->desc;
}

//-----------------------------------------------------------------------------

std::vector<std::string> Config::getAllConfigForService(std::string _serviceImpl, bool matchingOnly) const
{
    const std::string serviceImpl = core::runtime::filterID(_serviceImpl);
    core::mt::ReadLock lock(m_registryMutex);
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
