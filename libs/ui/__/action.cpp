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

const core::com::slots::key_t action::SET_CHECKED_SLOT       = "setChecked";
const core::com::slots::key_t action::CHECK_SLOT             = "check";
const core::com::slots::key_t action::UNCHECK_SLOT           = "uncheck";
const core::com::slots::key_t action::SET_VISIBLE_SLOT       = "setVisible";
const core::com::slots::key_t action::SET_HIDDEN_SLOT        = "setHidden";
const core::com::slots::key_t action::SHOW_SLOT              = "show";
const core::com::slots::key_t action::HIDE_SLOT              = "hide";
const core::com::slots::key_t action::TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const core::com::slots::key_t action::SET_ENABLED_SLOT       = "setEnabled";
const core::com::slots::key_t action::SET_DISABLED_SLOT      = "setDisabled";
const core::com::slots::key_t action::ENABLE_SLOT            = "enable";
const core::com::slots::key_t action::DISABLE_SLOT           = "disable";

const core::com::signals::key_t action::IS_ENABLED_SIG = "isEnabled";
const core::com::signals::key_t action::ENABLED_SIG    = "enabled";
const core::com::signals::key_t action::DISABLED_SIG   = "disabled";
const core::com::signals::key_t action::IS_CHECKED_SIG = "isChecked";
const core::com::signals::key_t action::CHECKED_SIG    = "checked";
const core::com::signals::key_t action::UNCHECKED_SIG  = "unchecked";
const core::com::signals::key_t action::IS_VISIBLE_SIG = "isVisible";

action::action()
{
    new_slot(SET_CHECKED_SLOT, &action::setChecked, this);
    new_slot(CHECK_SLOT, [this](){this->setChecked(true);});
    new_slot(UNCHECK_SLOT, [this](){this->setChecked(false);});

    new_slot(SET_ENABLED_SLOT, &action::setEnabled, this);
    new_slot(SET_DISABLED_SLOT, [this](bool _disabled){this->setEnabled(!_disabled);});
    new_slot(ENABLE_SLOT, [this](){this->setEnabled(true);});
    new_slot(DISABLE_SLOT, [this](){this->setEnabled(false);});

    new_slot(SET_VISIBLE_SLOT, &action::setVisible, this);
    new_slot(SET_HIDDEN_SLOT, [this](bool _hidden){this->setVisible(!_hidden);});
    new_slot(SHOW_SLOT, [this](){this->setVisible(true);});
    new_slot(HIDE_SLOT, [this](){this->setVisible(false);});
    new_slot(TOGGLE_VISIBILITY_SLOT, [this]{this->setVisible(!m_visible);});

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

    m_confirmAction  = config.get_child_optional("confirmation").has_value();
    m_confirmMessage = config.get<std::string>("confirmation.<xmlattr>.message", "");
    m_defaultButton  = core::runtime::get_ptree_value(config, "confirmation.<xmlattr>.defaultButton", m_defaultButton);
}

//-----------------------------------------------------------------------------

void action::actionServiceStopping()
{
    this->m_registry->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void action::actionServiceStarting()
{
    this->m_registry->actionServiceStarting();
    this->setChecked(m_checked);
}

//-----------------------------------------------------------------------------

void action::setChecked(bool _checked)
{
    m_checked = _checked;

    if(this->confirmAction())
    {
        this->m_registry->actionServiceSetChecked(m_checked);
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

void action::setEnabled(bool _enabled)
{
    m_enabled = _enabled;

    this->m_registry->actionServiceSetEnabled(m_enabled);
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

void action::setVisible(bool _visible)
{
    m_visible = _visible;
    this->m_registry->actionServiceSetVisible(_visible);
    signal<bool_signal_t>(IS_VISIBLE_SIG)->async_emit(_visible);
}

//-----------------------------------------------------------------------------

void action::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void action::hide()
{
    this->setVisible(false);
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

bool action::confirmAction()
{
    bool action_is_confirmed = true;

    if(m_confirmAction && this->status() == base::STARTED)
    {
        ui::dialog::message dialog;
        dialog.setTitle("Confirmation");
        std::string message = m_confirmMessage;
        if(message.empty())
        {
            message = "Do you really want to execute this action ?";
        }

        dialog.setMessage(message);

        if(m_defaultButton)
        {
            dialog.setDefaultButton(ui::dialog::message::YES);
        }
        else
        {
            dialog.setDefaultButton(ui::dialog::message::NO);
        }

        dialog.setIcon(ui::dialog::message::QUESTION);
        dialog.addButton(ui::dialog::message::YES_NO);
        ui::dialog::message::Buttons button = dialog.show();

        action_is_confirmed = (button == ui::dialog::message::YES);
    }

    return action_is_confirmed;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
