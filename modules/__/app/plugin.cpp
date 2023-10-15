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

#include <app/extension/config.hpp>
#include <app/extension/parameters.hpp>

namespace sight::module::app
{

SIGHT_REGISTER_PLUGIN("sight::module::app::plugin");

//------------------------------------------------------------------------------

void plugin::start()
{
    sight::app::extension::config::getDefault()->parse_plugin_infos();
    sight::app::extension::parameters::getDefault()->parse_plugin_infos();

    auto worker = core::thread::get_default_worker();
    worker->post([this](auto&& ...){run();});
}

//------------------------------------------------------------------------------

void plugin::run()
{
    if(this->get_module()->has_parameter("config"))
    {
        m_configurationName = this->get_module()->get_parameter_value("config");
        if(this->get_module()->has_parameter("parameters"))
        {
            m_parametersName = this->get_module()->get_parameter_value("parameters");
        }

        SIGHT_ASSERT("The OSR is already initialized.", !m_appConfigMng);
        SIGHT_ASSERT("The configuration name parameter is not initialized.", !m_configurationName.empty());

        m_appConfigMng = sight::app::config_manager::make();

        if(m_parametersName.empty())
        {
            const sight::app::field_adaptor_t fields;
            m_appConfigMng->setConfig(m_configurationName, fields);
        }
        else
        {
            const sight::app::field_adaptor_t& fields =
                sight::app::extension::parameters::getDefault()->getParameters(m_parametersName);
            m_appConfigMng->setConfig(m_configurationName, fields);
        }

        m_appConfigMng->launch();
    }
    else
    {
        SIGHT_DEBUG("Module app, missing param config in profile");
    }
}

//------------------------------------------------------------------------------

void plugin::stop() noexcept
{
    if(m_appConfigMng)
    {
        m_appConfigMng->stopAndDestroy();
        m_appConfigMng.reset();
    }

    // Clear all app configuration
    sight::app::extension::config::getDefault()->clear_registry();

    // Clear all app configuration parameters
    sight::app::extension::parameters::getDefault()->clear_registry();
}

//------------------------------------------------------------------------------

} // namespace sight::module::app
