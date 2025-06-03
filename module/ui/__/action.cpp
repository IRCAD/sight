/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "action.hpp"

#include <core/com/signal.hxx>
#include <core/runtime/helper.hpp>
namespace sight::module::ui
{

//------------------------------------------------------------------------------

void action::configuring()
{
    this->initialize();

    const auto config = this->get_config();
    m_sync = core::runtime::get_ptree_value(config, "sync", m_sync);

    if(const auto& parameter = config.get_child_optional("parameter"); parameter)
    {
        // Convert to std::optional and check that the key is present
        const auto& key = parameter->get_optional<std::string>("<xmlattr>.key");
        SIGHT_THROW_IF("Parameter key is mandatory if a action must emit a parameterChanged signal.", !key);
        m_key = std::make_optional(*key);

        // Convert to std::optional.
        const auto& clicked = parameter->get_optional<std::string>("<xmlattr>.clicked");
        m_clicked = clicked ? std::make_optional(*clicked) : std::nullopt;

        const auto& checked = parameter->get_optional<std::string>("<xmlattr>.checked");
        m_checked = checked ? std::make_optional(*checked) : std::nullopt;

        const auto& unchecked = parameter->get_optional<std::string>("<xmlattr>.unchecked");
        m_unchecked = unchecked ? std::make_optional(*unchecked) : std::nullopt;
    }

    m_joystick_alias = sight::io::joystick::interactor::to_joystick(config.get<std::string>("joystick", ""));
}

//-----------------------------------------------------------------------------

void action::starting()
{
    this->action_service_starting();

    if(m_joystick_alias != sight::io::joystick::joystick_t::unknown)
    {
        this->start_listening_joystick();
    }
}

//-----------------------------------------------------------------------------

void action::stopping()
{
    this->action_service_stopping();

    if(m_joystick_alias != sight::io::joystick::joystick_t::unknown)
    {
        this->stop_listening_joystick();
    }
}

//-----------------------------------------------------------------------------

void action::updating()
{
    if(this->confirm_action())
    {
        if(m_sync)
        {
            m_clicked_sig->emit();
        }
        else
        {
            m_clicked_sig->async_emit();
        }

        if(m_key)
        {
            const bool is_checked = this->checked();
            if(m_sync)
            {
                m_parameter_changed_sig->emit(
                    m_clicked
                    ? *m_clicked
                    : is_checked && m_checked
                    ? *m_checked
                    : !is_checked && m_unchecked
                    ? *m_unchecked
                    : sight::ui::parameter_t(is_checked),
                    *m_key
                );
            }
            else
            {
                m_parameter_changed_sig->async_emit(
                    m_clicked
                    ? *m_clicked
                    : is_checked && m_checked
                    ? *m_checked
                    : !is_checked && m_unchecked
                    ? *m_unchecked
                    : sight::ui::parameter_t(is_checked),
                    *m_key
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void action::joystick_axis_direction_event(const sight::io::joystick::axis_direction_event& _event)
{
    if(_event.device->alias == m_joystick_alias)
    {
        if(_event.axis_alias == sight::io::joystick::axis_t::tz and _event.value
           == sight::io::joystick::axis_direction_event::direction_t::backward)
        {
            this->updating();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui
