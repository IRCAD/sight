/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwServices/registry/ServiceConfig.hpp"

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Runtime.hpp>

namespace fwServices
{
namespace registry
{

const std::string ServiceConfig::CONFIG_EXT_POINT = "::fwServices::registry::ServiceConfig";

ServiceConfig::sptr ServiceConfig::s_currentServiceConfig = ServiceConfig::New();

//-----------------------------------------------------------------------------

ServiceConfig::sptr ServiceConfig::getDefault()
{
    return s_currentServiceConfig;
}

//-----------------------------------------------------------------------------

ServiceConfig::~ServiceConfig()
{
}

//-----------------------------------------------------------------------------

void ServiceConfig::parseBundleInformation()
{
    typedef std::shared_ptr< ::fwRuntime::Extension > ExtensionType;

    std::vector< ExtensionType >  extElements;
    extElements = ::fwRuntime::getAllExtensionsForPoint(CONFIG_EXT_POINT);
    for(ExtensionType ext :  extElements)
    {
        // Get id
        SLM_ASSERT("Missing id element", ext->hasConfigurationElement("id"));
        std::string id = ext->findConfigurationElement("id")->getValue();

        // Get service
        std::string service = "";
        if ( ext->hasConfigurationElement("service") )
        {
            service = ext->findConfigurationElement("service")->getValue();
        }

        // Get desc
        std::string desc = "No description available";
        if ( ext->hasConfigurationElement("desc") )
        {
            desc = ext->findConfigurationElement("desc")->getValue();
        }

        // Get config
        ::fwRuntime::ConfigurationElement::csptr config = ext->findConfigurationElement("config");

        // Add service config info
        this->addServiceConfigInfo(id, service, desc, config);
    }
}

//-----------------------------------------------------------------------------

void ServiceConfig::addServiceConfigInfo
    (   const std::string& configId,
    const std::string& service,
    const std::string& desc,
    ::fwRuntime::ConfigurationElement::csptr config)
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);

    OSLM_DEBUG( "New service config registering : "
                << " configId = " << configId
                << " service = " << service
                << " desc = " << desc
                );

    SLM_ASSERT("The service config with the id "<< configId <<" already exists.",
               m_reg.find( configId ) == m_reg.end() );

    ServiceConfigInfo::sptr info = ServiceConfigInfo::New();
    info->service   = service;
    info->desc      = desc;
    info->config    = config;
    m_reg[configId] = info;
}

//-----------------------------------------------------------------------------

ServiceConfig::ServiceConfig()
{
}

//-----------------------------------------------------------------------------

void ServiceConfig::clearRegistry()
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr ServiceConfig::getServiceConfig( const std::string& configId,
                                                                          const std::string& serviceImpl ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("The id " <<  configId << " is not found in the application configuration registry",
               iter != m_reg.end());
    SLM_ASSERT("The id " <<  configId << " is not allowed for this service " << serviceImpl,
               serviceImpl.empty() || iter->second->service.empty() || iter->second->service == serviceImpl);
    return iter->second->config;
}

//-----------------------------------------------------------------------------

const std::string& ServiceConfig::getConfigDesc( const std::string& configId ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("The id " <<  configId << " is not found in the application configuration registry",
               iter != m_reg.end());
    return iter->second->desc;
}

//-----------------------------------------------------------------------------

std::vector< std::string > ServiceConfig::getAllConfigForService( std::string serviceImpl, bool matchingOnly ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    std::vector< std::string > configs;

    for(Registry::value_type srvCfg :  m_reg)
    {
        ServiceConfigInfo::sptr info = srvCfg.second;
        if ( (info->service.empty() && !matchingOnly ) || info->service == serviceImpl)
        {
            configs.push_back(srvCfg.first);
        }
    }

    return configs;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices
