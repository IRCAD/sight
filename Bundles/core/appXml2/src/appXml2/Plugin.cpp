/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/registry/AppConfig2.hpp>
#include <fwServices/registry/AppConfigParameters.hpp>

#include "appXml2/Plugin.hpp"

namespace appXml2
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::appXml2::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() throw() : m_configurationName(""),m_parametersName("")
{
}

//------------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//------------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
    SLM_FATAL_IF("Bundle appXml2, missing param config in profile", !this->getBundle()->hasParameter("config"));
    m_configurationName = this->getBundle()->getParameterValue("config");
    if( this->getBundle()->hasParameter("parameters") )
    {
        m_parametersName = this->getBundle()->getParameterValue("parameters");
    }
}

//------------------------------------------------------------------------------

void Plugin::initialize() throw( ::fwRuntime::RuntimeException )
{
    SLM_ASSERT("The OSR is already initialized.", !m_appConfigMng );
    SLM_ASSERT("The configuration name parameter is not initialized.", !m_configurationName.empty());

    m_appConfigMng = ::fwServices::AppConfigManager2::New();

    if( m_parametersName.empty() )
    {
        const ::fwServices::registry::FieldAdaptorType fields;
        m_appConfigMng->setConfig( m_configurationName, fields );
    }
    else
    {
        const ::fwServices::registry::FieldAdaptorType & fields =
            ::fwServices::registry::AppConfigParameters::getDefault()->getParameters( m_parametersName );
        m_appConfigMng->setConfig( m_configurationName, fields );
    }

    m_appConfigMng->launch();
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() throw()
{
    SLM_ASSERT("The OSR is not initialized.", m_appConfigMng );
    m_appConfigMng->stopAndDestroy();
    m_appConfigMng.reset();
}

//------------------------------------------------------------------------------

} // namespace appXml2
