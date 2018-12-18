/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "appXml/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/AppConfigParameters.hpp>

namespace appXml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::appXml::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept :
    m_configurationName(""),
    m_parametersName("")
{
}

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    SLM_FATAL_IF("Bundle appXml, missing param config in profile", !this->getBundle()->hasParameter("config"));
    m_configurationName = this->getBundle()->getParameterValue("config");
    if( this->getBundle()->hasParameter("parameters") )
    {
        m_parametersName = this->getBundle()->getParameterValue("parameters");
    }
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SLM_ASSERT("The OSR is already initialized.", !m_appConfigMng );
    SLM_ASSERT("The configuration name parameter is not initialized.", !m_configurationName.empty());

    m_appConfigMng = ::fwServices::AppConfigManager::New();

    if( m_parametersName.empty() )
    {
        const ::fwServices::registry::FieldAdaptorType fields;
        m_appConfigMng->setConfig( m_configurationName, fields );
    }
    else
    {
        const ::fwServices::registry::FieldAdaptorType& fields =
            ::fwServices::registry::AppConfigParameters::getDefault()->getParameters( m_parametersName );
        m_appConfigMng->setConfig( m_configurationName, fields );
    }

    m_appConfigMng->launch();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{
    SLM_ASSERT("The OSR is not initialized.", m_appConfigMng );
    m_appConfigMng->stopAndDestroy();
    m_appConfigMng.reset();
}

//------------------------------------------------------------------------------

} // namespace appXml
