/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include "appXml/Plugin.hpp"

namespace appXml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::appXml::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() throw() : m_configurationName("")
{}

//------------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//------------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
    SLM_FATAL_IF("Bundle appXml, missing param config in profile", !this->getBundle()->hasParameter("config"));
    m_configurationName =  this->getBundle()->getParameterValue("config") ;
}

//------------------------------------------------------------------------------

void Plugin::initialize() throw( ::fwRuntime::RuntimeException )
{
    SLM_ASSERT("Sorry, the OSR is already initialized.", ! m_appConfigMng );
    SLM_ASSERT("Sorry, configuration name parameter is not initialized.", !m_configurationName.empty());

    m_appConfigMng = ::fwServices::AppConfigManager::New();
    ::fwRuntime::ConfigurationElement::csptr config = ::fwServices::registry::AppConfig::getDefault()->getStandardConfig( m_configurationName );
    m_appConfigMng->setConfig( ::fwRuntime::ConfigurationElement::constCast( config ) );
    m_appConfigMng->launch();
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//------------------------------------------------------------------------------

void Plugin::uninitialize() throw()
{
    SLM_ASSERT("Sorry, the OSR is not initialized.", m_appConfigMng );
    m_appConfigMng->stopAndDestroy();
}

//------------------------------------------------------------------------------

} // namespace appXml
