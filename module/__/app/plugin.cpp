/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
    sight::app::extension::config::get()->parse_plugin_infos();
    sight::app::extension::parameters::get_default()->parse_plugin_infos();

    if(this->get_module()->has_parameter("config"))
    {
        auto worker = core::thread::get_default_worker();
        worker->post([this](auto&& ...){run();});
    }
    else
    {
        SIGHT_DEBUG("Module app, missing param config in profile");
    }
}

//------------------------------------------------------------------------------

void plugin::run()
{
    const auto module = this->get_module();
    m_configuration_name = module->get_parameter_value("config");
    if(module->has_parameter("parameters"))
    {
        m_parameters_name = module->get_parameter_value("parameters");
    }

    SIGHT_ASSERT("The OSR is already initialized.", !m_app_config_mng);
    SIGHT_ASSERT("The configuration name parameter is not initialized.", !m_configuration_name.empty());

    m_app_config_mng = sight::app::config_manager::make();

    if(m_parameters_name.empty())
    {
        const sight::app::field_adaptor_t fields;
        m_app_config_mng->set_config(m_configuration_name, fields);
    }
    else
    {
        const sight::app::field_adaptor_t& fields =
            sight::app::extension::parameters::get_default()->get_parameters(m_parameters_name);
        m_app_config_mng->set_config(m_configuration_name, fields);
    }

    m_app_config_mng->launch();
}

//------------------------------------------------------------------------------

void plugin::stop() noexcept
{
    if(m_app_config_mng)
    {
        m_app_config_mng->stop_and_destroy();
        m_app_config_mng.reset();
    }

    // Clear all app configuration
    sight::app::extension::config::get()->clear_registry();

    // Clear all app configuration parameters
    sight::app::extension::parameters::get_default()->clear_registry();
}

//------------------------------------------------------------------------------

} // namespace sight::module::app
