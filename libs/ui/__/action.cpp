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

#include "ui/__/action.hpp"
#include "ui/__/detail/registry/action.hpp"

#include "ui/__/dialog/message.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/helper.hpp>

namespace sight::ui
{

const core::com::slots::key_t action::SET_CHECKED_SLOT       = "set_checked";
const core::com::slots::key_t action::CHECK_SLOT             = "check";
const core::com::slots::key_t action::UNCHECK_SLOT           = "uncheck";
const core::com::slots::key_t action::SET_VISIBLE_SLOT       = "set_visible";
const core::com::slots::key_t action::SET_HIDDEN_SLOT        = "set_hidden";
const core::com::slots::key_t action::SHOW_SLOT              = "show";
const core::com::slots::key_t action::HIDE_SLOT              = "hide";
const core::com::slots::key_t action::TOGGLE_VISIBILITY_SLOT = "toggle_visibility";
const core::com::slots::key_t action::SET_ENABLED_SLOT       = "set_enabled";
const core::com::slots::key_t action::SET_DISABLED_SLOT      = "set_disabled";
const core::com::slots::key_t action::ENABLE_SLOT            = "enable";
const core::com::slots::key_t action::DISABLE_SLOT           = "disable";

const core::com::signals::key_t action::IS_ENABLED_SIG = "is_enabled";
const core::com::signals::key_t action::ENABLED_SIG    = "enabled";
const core::com::signals::key_t action::DISABLED_SIG   = "disabled";
const core::com::signals::key_t action::IS_CHECKED_SIG = "is_checked";
const core::com::signals::key_t action::CHECKED_SIG    = "checked";
const core::com::signals::key_t action::UNCHECKED_SIG  = "unchecked";
const core::com::signals::key_t action::IS_VISIBLE_SIG = "is_visible";

action::action()
{
    new_slot(SET_CHECKED_SLOT, &action::set_checked, this);
    new_slot(CHECK_SLOT, [this](){this->set_checked(true);});
    new_slot(UNCHECK_SLOT, [this](){this->set_checked(false);});

    new_slot(SET_ENABLED_SLOT, &action::set_enabled, this);
    new_slot(SET_DISABLED_SLOT, [this](bool _disabled){this->set_enabled(!_disabled);});
    new_slot(ENABLE_SLOT, [this](){this->set_enabled(true);});
    new_slot(DISABLE_SLOT, [this](){this->set_enabled(false);});

    new_slot(SET_VISIBLE_SLOT, &action::set_visible, this);
    new_slot(SET_HIDDEN_SLOT, [this](bool _hidden){this->set_visible(!_hidden);});
    new_slot(SHOW_SLOT, [this](){this->set_visible(true);});
    new_slot(HIDE_SLOT, [this](){this->set_visible(false);});
    new_slot(TOGGLE_VISIBILITY_SLOT, [this]{this->set_visible(!m_visible);});

    new_signal<bool_signal_t>(IS_ENABLED_SIG);
    new_signal<void_signal_t>(ENABLED_SIG);
    new_signal<void_signal_t>(DISABLED_SIG);
    new_signal<bool_signal_t>(IS_CHECKED_SIG);
    new_signal<void_signal_t>(CHECKED_SIG);
    new_signal<void_signal_t>(UNCHECKED_SIG);
    new_signal<bool_signal_t>(IS_VISIBLE_SIG);
}

//-----------------------------------------------------------------------------

action::~action()
= default;

//-----------------------------------------------------------------------------

void action::initialize()
{
    m_registry = ui::detail::registry::action::make(this->get_id());

    auto config = this->get_config();

    m_checked = core::runtime::get_ptree_value(config, "state.<xmlattr>.checked", m_checked);
    m_enabled = core::runtime::get_ptree_value(config, "state.<xmlattr>.enabled", m_enabled);

    m_inverted = core::runtime::get_ptree_value(config, "state.<xmlattr>.inverse", m_inverted);
    m_visible  = core::runtime::get_ptree_value(config, "state.<xmlattr>.visible", m_visible);

    m_confirm_action  = config.get_child_optional("confirmation").has_value();
    m_confirm_message = config.get<std::string>("confirmation.<xmlattr>.message", "");
    m_default_button  =
        core::runtime::get_ptree_value(config, "confirmation.<xmlattr>.defaultButton", m_default_button);
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
    this->set_checked(m_checked);
}

//-----------------------------------------------------------------------------

void action::set_checked(bool _checked)
{
    m_checked = _checked;

    if(this->confirm_action())
    {
        this->m_registry->action_service_set_checked(m_checked);
        if(m_checked)
        {
            auto sig = this->signal<void_signal_t>(CHECKED_SIG);
            sig->async_emit();
        }
        else
        {
            auto sig = this->signal<void_signal_t>(UNCHECKED_SIG);
            sig->async_emit();
        }

        auto sig = this->signal<bool_signal_t>(IS_CHECKED_SIG);
        sig->async_emit(m_checked);
    }
}

//-----------------------------------------------------------------------------

bool action::checked() const
{
    return m_checked;
}

//-----------------------------------------------------------------------------

void action::set_enabled(bool _enabled)
{
    m_enabled = _enabled;

    this->m_registry->action_service_set_enabled(m_enabled);
    if(m_enabled)
    {
        auto sig = this->signal<void_signal_t>(ENABLED_SIG);
        sig->async_emit();
    }
    else
    {
        auto sig = this->signal<void_signal_t>(DISABLED_SIG);
        sig->async_emit();
    }

    auto sig = this->signal<bool_signal_t>(IS_ENABLED_SIG);
    sig->async_emit(m_enabled);
}

//-----------------------------------------------------------------------------

bool action::enabled() const
{
    return m_enabled;
}

//-----------------------------------------------------------------------------

void action::set_visible(bool _visible)
{
    m_visible = _visible;
    this->m_registry->action_service_set_visible(_visible);
    signal<bool_signal_t>(IS_VISIBLE_SIG)->async_emit(_visible);
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
    return m_visible;
}

//-----------------------------------------------------------------------------

bool action::inverted() const
{
    return m_inverted;
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

} // namespace sight::ui
