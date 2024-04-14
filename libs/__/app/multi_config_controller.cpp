/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <boost/range/iterator_range.hpp>

namespace sight::app
{

//------------------------------------------------------------------------------

multi_config_controller::multi_config_controller() noexcept :
    m_config_launcher(std::make_unique<app::helper::config_launcher>())
{
    new_signal<signals::string_signal_t>(signals::CONFIG_STARTED);
    new_slot(slots::SET_CONFIG, &multi_config_controller::set_config, this);
}

//------------------------------------------------------------------------------

void multi_config_controller::configuring(const config_t& _config)
{
    m_config_launcher->parse_config(_config, this->get_sptr());

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
    m_config_launcher->stop_config();
}

//------------------------------------------------------------------------------

void multi_config_controller::updating()
{
    // Start the configuration if it is not running
    bool start = !m_config_launcher->config_is_running();

    if(const auto& found = m_config_parameters_map.find(m_key); found != m_config_parameters_map.end())
    {
        const auto& new_config = found->second;

        // If the configuration is different from the current one
        if(m_config_launcher->config() != new_config)
        {
            // Set the new configuration to start
            m_config_launcher->set_config(new_config);

            // Stop the current configuration and force restart
            if(!start)
            {
                start = true;
                m_config_launcher->stop_config();
            }
        }
    }

    if(start)
    {
        m_config_launcher->start_config(this->get_sptr(), m_config_parameters_map);

        auto sig = signal<signals::string_signal_t>(signals::CONFIG_STARTED);
        sig->async_emit(m_config_launcher->config());
    }
}

//------------------------------------------------------------------------------
void multi_config_controller::set_config(sight::ui::parameter_t _val, std::string _key)
{
    if(std::holds_alternative<std::string>(_val))
    {
        m_config_parameters_map[_key] = std::get<std::string>(_val);
    }
}

//------------------------------------------------------------------------------

void multi_config_controller::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::app
