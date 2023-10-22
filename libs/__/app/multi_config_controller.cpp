/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "app/multi_config_controller.hpp"

#include <core/com/slots.hxx>

namespace sight::app
{

//------------------------------------------------------------------------------

multi_config_controller::multi_config_controller() noexcept :
    m_configLauncher(std::make_unique<app::helper::config_launcher>())
{
    new_slot(slots::SET_CONFIG, &multi_config_controller::setConfig, this);
}

//------------------------------------------------------------------------------

void multi_config_controller::configuring(const config_t& _config)
{
    m_configLauncher->parseConfig(_config, this->get_sptr());

    const auto& app_config = _config.get_child("appConfig");
    m_key = app_config.get<std::string>("<xmlattr>.key", m_key);
}

//------------------------------------------------------------------------------

void multi_config_controller::starting()
{
}

//------------------------------------------------------------------------------

void multi_config_controller::stopping()
{
    m_configLauncher->stopConfig();
}

//------------------------------------------------------------------------------

void multi_config_controller::updating()
{
    if(m_configLauncher->configIsRunning())
    {
        m_configLauncher->stopConfig();
    }

    m_configLauncher->startConfig(this->get_sptr());
}

//------------------------------------------------------------------------------
void multi_config_controller::setConfig(sight::ui::parameter_t _val, std::string _key)
{
    if(_key == m_key && std::holds_alternative<std::string>(_val))
    {
        m_configLauncher->setConfig(std::get<std::string>(_val));
        this->update();
    }
}

//------------------------------------------------------------------------------

void multi_config_controller::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::app
