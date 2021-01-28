/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "Plugin.hpp"

#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <services/registry/AppConfig.hpp>
#include <services/registry/AppConfigParameters.hpp>

namespace sight::module::appXml
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::sight::module::appXml::Plugin");

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
    SLM_FATAL_IF("Module appXml, missing param config in profile", !this->getModule()->hasParameter("config"));
    m_configurationName = this->getModule()->getParameterValue("config");
    if( this->getModule()->hasParameter("parameters") )
    {
        m_parametersName = this->getModule()->getParameterValue("parameters");
    }
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SLM_ASSERT("The OSR is already initialized.", !m_appConfigMng );
    SLM_ASSERT("The configuration name parameter is not initialized.", !m_configurationName.empty());

    m_appConfigMng = services::AppConfigManager::New();

    if( m_parametersName.empty() )
    {
        const services::registry::FieldAdaptorType fields;
        m_appConfigMng->setConfig( m_configurationName, fields );
    }
    else
    {
        const services::registry::FieldAdaptorType& fields =
            services::registry::AppConfigParameters::getDefault()->getParameters( m_parametersName );
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

} // namespace sight::module::appXml
