/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/registry/AppConfig.hpp"

namespace fwServices
{
namespace registry
{

//-----------------------------------------------------------------------------

AppConfig::sptr AppConfig::getDefault()
{
    SLM_TRACE_FUNC();
    static AppConfig::sptr m_instance = AppConfig::New();
    return m_instance;
}

//-----------------------------------------------------------------------------

AppConfig::~AppConfig()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void AppConfig::parseBundleInformation()
{}

//-----------------------------------------------------------------------------

void AppConfig::addAppInfo
(   const std::string & configId,
    AppInfo::ConfigType type,
    const std::string & desc,
    ::fwRuntime::ConfigurationElement::csptr config)
{
    SLM_TRACE_FUNC();
    OSLM_DEBUG( "New app config registring : "
            << " configId =" << configId
            << " type=" << type
    );

    SLM_ASSERT("Sorry, app config id = "<< configId <<" already exist.", m_reg.find( configId ) == m_reg.end() );

    AppInfo::NewSptr info;
    info->type = type;
    info->desc = desc;
    info->config =  config;
    m_reg[configId] = info;
}

//-----------------------------------------------------------------------------

AppConfig::AppConfig()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void AppConfig::clearRegistry()
{
    SLM_TRACE_FUNC();
    m_reg.clear();
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig::getStandardConfig( const std::string & configId ) const
{
    SLM_TRACE_FUNC();
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("Sorry, the id " <<  configId << " is not found in the application configuration registry", iter != m_reg.end());
    return iter->second->config;
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig::getAdaptedTemplateConfig( const std::string & configId, const std::map< std::string, std::string > & replaceFields ) const
{
    // Get config template
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("Sorry, the id " <<  configId << " is not found in the application configuration registry", iter != m_reg.end());
    iter->second->config;

    // Adapt config
    ::fwRuntime::ConfigurationElement::csptr newConfig;
    return newConfig;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices

