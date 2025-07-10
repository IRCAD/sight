/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/__/action.hpp"
#include "ui/__/detail/registry/action.hpp"

#include "ui/__/dialog/message.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/ptree.hpp>

namespace sight::ui
{

action::action()
{
    new_slot(slots::SET_CHECKED, &action::set_checked, this);
    new_slot(slots::CHECK, [this](){this->set_checked(true);});
    new_slot(slots::UNCHECK, [this](){this->set_checked(false);});
    new_slot(slots::APPLY_CHECKED, [this](){this->set_checked(*m_checked);});
    new_slot(slots::SET_ENABLED, &action::set_enabled, this);
    new_slot(slots::SET_DISABLED, [this](bool _disabled){this->set_enabled(!_disabled);});
    new_slot(slots::ENABLE, [this](){this->set_enabled(true);});
    new_slot(slots::DISABLE, [this](){this->set_enabled(false);});
    new_slot(slots::APPLY_ENABLED, [this](){this->set_enabled(*m_enabled);});

    new_slot(slots::SET_VISIBLE, &action::set_visible, this);
    new_slot(slots::SET_HIDDEN, [this](bool _hidden){this->set_visible(!_hidden);});
    new_slot(slots::SHOW, [this](){this->set_visible(true);});
    new_slot(slots::HIDE, [this](){this->set_visible(false);});
    new_slot(slots::TOGGLE_VISIBILITY, [this]{this->set_visible(!*m_visible);});
    new_slot(slots::APPLY_VISIBLE, [this](){this->set_visible(*m_visible);});

    new_signal<signals::bool_t>(signals::IS_ENABLED);
    new_signal<signals::void_t>(signals::ENABLED);
    new_signal<signals::void_t>(signals::DISABLED);
    new_signal<signals::bool_t>(signals::IS_CHECKED);
    new_signal<signals::void_t>(signals::CHECKED);
    new_signal<signals::void_t>(signals::UNCHECKED);
    new_signal<signals::bool_t>(signals::IS_VISIBLE);
}

//-----------------------------------------------------------------------------

void action::initialize()
{
    m_registry = ui::detail::registry::action::make(this->get_id());

    auto config = this->get_config();

    if(auto properties = config.get_child_optional("properties"); not properties.has_value())
    {
        const auto checked = m_checked.lock();
        *checked = core::ptree::get_value(config, "state.<xmlattr>.checked", false);
        const auto enabled = m_enabled.lock();
        *enabled = core::ptree::get_value(config, "state.<xmlattr>.enabled", true);

        const auto inverse = m_inverse.lock();
        *inverse = core::ptree::get_value(config, "state.<xmlattr>.inverse", false);
        const auto visible = m_visible.lock();
        *visible = core::ptree::get_value(config, "state.<xmlattr>.visible", true);
    }
    else
    {
        SIGHT_ASSERT("Do not mix properties with pure signals", !config.get_child_optional("state").has_value());
    }

    m_confirm_action  = config.get_child_optional("confirmation").has_value();
    m_confirm_message = config.get<std::string>("confirmation.<xmlattr>.message", "");
    m_default_button  =
        core::ptree::get_value(config, "confirmation.<xmlattr>.defaultButton", m_default_button);
}

//-----------------------------------------------------------------------------

void action::action_service_stopping()
{
    this->m_registry->action_service_stopping();
}

//-----------------------------------------------------------------------------

void action::action_service_starting()
{
    this->m_registry->action_service_starting();
    this->set_checked(*m_checked);
}

//-----------------------------------------------------------------------------

void action::set_checked(bool _checked)
{
    if(this->confirm_action())
    {
        {
            const auto checked = m_checked.lock();

            if(_checked != checked->value())
            {
                *checked = _checked;
                checked->async_emit(this, data::object::MODIFIED_SIG);
            }
        }
        this->m_registry->action_service_set_checked(_checked);

        if(!m_prev_checked.has_value() || *m_prev_checked != _checked)
        {
            m_prev_checked = _checked;
            if(_checked)
            {
                this->async_emit(signals::CHECKED);
            }
            else
            {
                this->async_emit(signals::UNCHECKED);
            }

            this->async_emit(signals::IS_CHECKED, _checked);
        }
    }
}

//-----------------------------------------------------------------------------

bool action::checked() const
{
    return *m_checked;
}

//-----------------------------------------------------------------------------

void action::set_enabled(bool _enabled)
{
    {
        const auto enabled = m_enabled.lock();

        if(_enabled != enabled->value())
        {
            *enabled = _enabled;
            enabled->async_emit(this, data::object::MODIFIED_SIG);
        }
    }

    this->m_registry->action_service_set_enabled(_enabled);

    if(!m_prev_enabled.has_value() || *m_prev_enabled != _enabled)
    {
        m_prev_enabled = _enabled;

        if(_enabled)
        {
            this->async_emit(signals::ENABLED);
        }
        else
        {
            this->async_emit(signals::DISABLED);
        }

        this->async_emit(signals::IS_ENABLED, _enabled);
    }
}

//-----------------------------------------------------------------------------

bool action::enabled() const
{
    return *m_enabled;
}

//-----------------------------------------------------------------------------

void action::set_visible(bool _visible)
{
    {
        const auto visible = m_visible.lock();

        if(_visible != visible->value())
        {
            *visible = _visible;
            visible->async_emit(this, data::object::MODIFIED_SIG);
        }
    }
    this->m_registry->action_service_set_visible(_visible);

    if(!m_prev_visible.has_value() || *m_prev_visible != _visible)
    {
        m_prev_visible = _visible;

        this->async_emit(signals::IS_VISIBLE, _visible);
    }
}

//-----------------------------------------------------------------------------

void action::show()
{
    this->set_visible(true);
}

//-----------------------------------------------------------------------------

void action::hide()
{
    this->set_visible(false);
}

//-----------------------------------------------------------------------------

bool action::visible() const
{
    return *m_visible;
}

//-----------------------------------------------------------------------------

bool action::inverted() const
{
    return *m_inverse;
}

//-----------------------------------------------------------------------------

bool action::confirm_action()
{
    bool action_is_confirmed = true;

    if(m_confirm_action && this->status() == base::global_status::started)
    {
        ui::dialog::message dialog;
        dialog.set_title("Confirmation");
        std::string message = m_confirm_message;
        if(message.empty())
        {
            message = "Do you really want to execute this action ?";
        }

        dialog.set_message(message);

        if(m_default_button)
        {
            dialog.set_default_button(ui::dialog::message::yes);
        }
        else
        {
            dialog.set_default_button(ui::dialog::message::no);
        }

        dialog.set_icon(ui::dialog::message::question);
        dialog.add_button(ui::dialog::message::yes_no);
        ui::dialog::message::buttons button = dialog.show();

        action_is_confirmed = (button == ui::dialog::message::yes);
    }

    return action_is_confirmed;
}

//-----------------------------------------------------------------------------

service::connections_t action::auto_connections() const
{
    return {
        {m_checked, sight::data::object::MODIFIED_SIG, slots::APPLY_CHECKED},
        {m_enabled, sight::data::object::MODIFIED_SIG, slots::APPLY_ENABLED},
        {m_visible, sight::data::object::MODIFIED_SIG, slots::APPLY_VISIBLE}
    };
}

} // namespace sight::ui
