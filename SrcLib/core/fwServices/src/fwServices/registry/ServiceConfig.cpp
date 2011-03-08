/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/registry/ServiceConfig.hpp"

namespace fwServices
{
namespace registry
{

const std::string ServiceConfig::CONFIG_EXT_POINT = "::fwServices::registry::ServiceConfig";
//-----------------------------------------------------------------------------

ServiceConfig::sptr ServiceConfig::getDefault()
{
    SLM_TRACE_FUNC();
    static ServiceConfig::sptr m_instance = ServiceConfig::New();
    return m_instance;
}

//-----------------------------------------------------------------------------

ServiceConfig::~ServiceConfig()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ServiceConfig::parseBundleInformation()
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef ::boost::shared_ptr< ::fwRuntime::Extension > ExtensionType;

    std::vector< ExtensionType >  extElements;
    extElements = ::fwRuntime::getAllExtensionsForPoint(CONFIG_EXT_POINT);
    BOOST_FOREACH(ExtensionType ext , extElements)
    {
        // Get id
        SLM_ASSERT("Missing id element", ext->hasConfigurationElement("id"));
        std::string id = ext->findConfigurationElement("id")->getValue();

        // Get service
        std::string service ="";
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
(   const std::string & configId,
    const std::string & service,
    const std::string & desc,
    ::fwRuntime::ConfigurationElement::csptr config)
{
    SLM_TRACE_FUNC();
    OSLM_DEBUG( "New service config registring : "
            << " configId = " << configId
            << " service = " << service
            << " desc = " << desc
    );

    SLM_ASSERT("Sorry, service config id = "<< configId <<" already exist.", m_reg.find( configId ) == m_reg.end() );

    ServiceConfigInfo::NewSptr info;
    info->service = service;
    info->desc = desc;
    info->config =  config;
    m_reg[configId] = info;
}

//-----------------------------------------------------------------------------

ServiceConfig::ServiceConfig()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ServiceConfig::clearRegistry()
{
    SLM_TRACE_FUNC();
    m_reg.clear();
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr ServiceConfig::getServiceConfig( const std::string & configId, const std::string &serviceImpl ) const
{
    SLM_TRACE_FUNC();
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("Sorry, the id " <<  configId << " is not found in the application configuration registry", iter != m_reg.end());
    SLM_ASSERT("Sorry, the id " <<  configId << " is not allowed for this service " << serviceImpl,
               serviceImpl.empty() || iter->second->service.empty() || iter->second->service == serviceImpl);
    return iter->second->config;
}

//-----------------------------------------------------------------------------

std::vector< std::string > ServiceConfig::getAllConfigForService( std::string serviceImpl ) const
{
    std::vector< std::string > configs;

    BOOST_FOREACH(Registry::value_type srvCfg, m_reg)
    {
        ServiceConfigInfo::sptr info = srvCfg.second;
        if (info->service.empty() || info->service == serviceImpl)
        {
            configs.push_back(srvCfg.first);
        }
    }

    return configs;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices

