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

#include "plugin.hpp"

#include <service/extension/AppConfig.hpp>
#include <service/extension/AppConfigParameters.hpp>

namespace sight::module::appXml
{

SIGHT_REGISTER_PLUGIN("sight::module::appXml::plugin");

//------------------------------------------------------------------------------

plugin::plugin() noexcept =
    default;

//------------------------------------------------------------------------------

plugin::~plugin() noexcept =
    default;

//------------------------------------------------------------------------------

void plugin::start()
{
    auto worker = core::thread::get_default_worker();
    worker->post([this](auto&& ...){run();});
}

//------------------------------------------------------------------------------

void plugin::run()
{
    SIGHT_FATAL_IF("Module appXml, missing param config in profile", !this->get_module()->has_parameter("config"));
    m_configurationName = this->get_module()->get_parameter_value("config");
    if(this->get_module()->has_parameter("parameters"))
    {
        m_parametersName = this->get_module()->get_parameter_value("parameters");
    }

    SIGHT_ASSERT("The OSR is already initialized.", !m_appConfigMng);
    SIGHT_ASSERT("The configuration name parameter is not initialized.", !m_configurationName.empty());

    m_appConfigMng = service::app_config_manager::make();

    if(m_parametersName.empty())
    {
        const service::FieldAdaptorType fields;
        m_appConfigMng->setConfig(m_configurationName, fields);
    }
    else
    {
        const service::FieldAdaptorType& fields =
            service::extension::AppConfigParameters::getDefault()->getParameters(m_parametersName);
        m_appConfigMng->setConfig(m_configurationName, fields);
    }

    m_appConfigMng->launch();
}

//------------------------------------------------------------------------------

void plugin::stop() noexcept
{
    SIGHT_ASSERT("The OSR is not initialized.", m_appConfigMng);
    m_appConfigMng->stopAndDestroy();
    m_appConfigMng.reset();
}

//------------------------------------------------------------------------------

} // namespace sight::module::appXml
