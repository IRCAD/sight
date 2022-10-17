/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/IAction.hpp"

#include "ui/base/dialog/IMessageDialog.hpp"
#include "ui/base/dialog/MessageDialog.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/helper.hpp>

namespace sight::ui::base
{

const core::com::Slots::SlotKeyType IAction::s_SET_CHECKED_SLOT = "setChecked";
const core::com::Slots::SlotKeyType IAction::s_CHECK_SLOT       = "check";
const core::com::Slots::SlotKeyType IAction::s_UNCHECK_SLOT     = "uncheck";
const core::com::Slots::SlotKeyType IAction::s_SET_VISIBLE_SLOT = "setVisible";
const core::com::Slots::SlotKeyType IAction::s_SHOW_SLOT        = "show";
const core::com::Slots::SlotKeyType IAction::s_HIDE_SLOT        = "hide";
const core::com::Slots::SlotKeyType IAction::s_SET_ENABLED_SLOT = "setEnabled";
const core::com::Slots::SlotKeyType IAction::s_ENABLE_SLOT      = "enable";
const core::com::Slots::SlotKeyType IAction::s_DISABLE_SLOT     = "disable";

const core::com::Signals::SignalKeyType IAction::s_IS_ENABLED_SIG = "isEnabled";
const core::com::Signals::SignalKeyType IAction::s_ENABLED_SIG    = "enabled";
const core::com::Signals::SignalKeyType IAction::s_DISABLED_SIG   = "disabled";
const core::com::Signals::SignalKeyType IAction::s_IS_CHECKED_SIG = "isChecked";
const core::com::Signals::SignalKeyType IAction::s_CHECKED_SIG    = "checked";
const core::com::Signals::SignalKeyType IAction::s_UNCHECKED_SIG  = "unchecked";

// Deprecated in Sight 22.0 and removed in Sight 23.0
const core::com::Slots::SlotKeyType IAction::s_SET_IS_ACTIVE_SLOT = "setIsActive";
const core::com::Slots::SlotKeyType IAction::s_ACTIVATE_SLOT      = "activate";
const core::com::Slots::SlotKeyType IAction::s_DEACTIVATE_SLOT    = "deactivate";

const core::com::Slots::SlotKeyType IAction::s_SET_IS_EXECUTABLE_SLOT = "setIsExecutable";
const core::com::Slots::SlotKeyType IAction::s_SET_EXECUTABLE_SLOT    = "setExecutable";
const core::com::Slots::SlotKeyType IAction::s_SET_INEXECUTABLE_SLOT  = "setInexecutable";

IAction::IAction()
{
    newSlot(s_SET_CHECKED_SLOT, &IAction::setChecked, this);
    newSlot(s_CHECK_SLOT, [this](){this->setChecked(true);});
    newSlot(s_UNCHECK_SLOT, [this](){this->setChecked(false);});

    newSlot(s_SET_ENABLED_SLOT, &IAction::setEnabled, this);
    newSlot(s_ENABLE_SLOT, [this](){this->setEnabled(true);});
    newSlot(s_DISABLE_SLOT, [this](){this->setEnabled(false);});

    newSlot(s_SET_VISIBLE_SLOT, &IAction::setVisible, this);
    newSlot(s_SHOW_SLOT, [this](){this->setVisible(true);});
    newSlot(s_HIDE_SLOT, [this](){this->setVisible(false);});

    newSlot(s_SET_IS_ACTIVE_SLOT, &IAction::setIsActive, this);
    newSlot(s_ACTIVATE_SLOT, [this](){this->setIsActive(true);});
    newSlot(s_DEACTIVATE_SLOT, [this](){this->setIsActive(false);});

    newSlot(s_SET_IS_EXECUTABLE_SLOT, &IAction::setIsExecutable, this);
    newSlot(s_SET_EXECUTABLE_SLOT, [this](){this->setIsExecutable(true);});
    newSlot(s_SET_INEXECUTABLE_SLOT, [this](){this->setIsExecutable(false);});

    newSignal<bool_signal_t>(s_IS_ENABLED_SIG);
    newSignal<void_signal_t>(s_ENABLED_SIG);
    newSignal<void_signal_t>(s_DISABLED_SIG);
    newSignal<bool_signal_t>(s_IS_CHECKED_SIG);
    newSignal<void_signal_t>(s_CHECKED_SIG);
    newSignal<void_signal_t>(s_UNCHECKED_SIG);
}

//-----------------------------------------------------------------------------

IAction::~IAction()
= default;

//-----------------------------------------------------------------------------

void IAction::initialize()
{
    m_registry = ui::base::registry::Action::New(this->getID());

    auto config = this->getConfigTree();

    if(config.get_child_optional("state.<xmlattr>.active").has_value())
    {
        SIGHT_WARN(
            "'<state active=" "/> option is deprecated and will be removed in Sight 23.0,"
                              " please use '<state checked=" ">' instead."
        );
        m_checked = core::runtime::get_ptree_value(config, "state.<xmlattr>.active", m_checked);
    }
    else
    {
        m_checked = core::runtime::get_ptree_value(config, "state.<xmlattr>.checked", m_checked);
    }

    if(config.get_child_optional("state.<xmlattr>.executable").has_value())
    {
        SIGHT_WARN(
            "'<state executable=" "/> option is deprecated and will be removed in Sight 23.0,"
                                  " please use '<state enabled=" ">' instead."
        );
        m_enabled = core::runtime::get_ptree_value(config, "state.<xmlattr>.executable", m_enabled);
    }
    else
    {
        m_enabled = core::runtime::get_ptree_value(config, "state.<xmlattr>.enabled", m_enabled);
    }

    m_inverted = core::runtime::get_ptree_value(config, "state.<xmlattr>.inverse", m_inverted);
    m_visible  = core::runtime::get_ptree_value(config, "state.<xmlattr>.visible", m_visible);

    m_confirmAction  = config.get_child_optional("confirmation").has_value();
    m_confirmMessage = config.get<std::string>("confirmation.<xmlattr>.message", "");
    m_defaultButton  = core::runtime::get_ptree_value(config, "confirmation.<xmlattr>.defaultButton", m_defaultButton);
}

//-----------------------------------------------------------------------------

void IAction::actionServiceStopping()
{
    this->m_registry->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void IAction::actionServiceStarting()
{
    this->m_registry->actionServiceStarting();
    this->setChecked(m_checked);
}

//-----------------------------------------------------------------------------

void IAction::setChecked(bool checked)
{
    m_checked = checked;

    if(this->confirmAction())
    {
        this->m_registry->actionServiceSetChecked(m_checked);
        if(m_checked)
        {
            auto sig = this->signal<void_signal_t>(s_CHECKED_SIG);
            sig->asyncEmit();
        }
        else
        {
            auto sig = this->signal<void_signal_t>(s_UNCHECKED_SIG);
            sig->asyncEmit();
        }

        auto sig = this->signal<bool_signal_t>(s_IS_CHECKED_SIG);
        sig->asyncEmit(m_checked);
    }
}

//-----------------------------------------------------------------------------

bool IAction::checked() const
{
    return m_checked;
}

//-----------------------------------------------------------------------------

void IAction::setIsActive(bool isActive)
{
    SIGHT_WARN(
        "'setIsActive/activate/deactivate' slots are deprecated and will be removed in Sight 23.0,"
        " please use 'setChecked/check/uncheck' instead."
    );

    this->setChecked(isActive);
}

//-----------------------------------------------------------------------------

bool IAction::getIsActive() const
{
    return this->checked();
}

//-----------------------------------------------------------------------------

void IAction::setEnabled(bool enabled)
{
    m_enabled = enabled;

    this->m_registry->actionServiceSetEnabled(m_enabled);
    if(m_enabled)
    {
        auto sig = this->signal<void_signal_t>(s_ENABLED_SIG);
        sig->asyncEmit();
    }
    else
    {
        auto sig = this->signal<void_signal_t>(s_DISABLED_SIG);
        sig->asyncEmit();
    }

    auto sig = this->signal<bool_signal_t>(s_IS_ENABLED_SIG);
    sig->asyncEmit(m_enabled);
}

//-----------------------------------------------------------------------------

bool IAction::enabled() const
{
    return m_enabled;
}

//-----------------------------------------------------------------------------

void IAction::setIsExecutable(bool isExecutable)
{
    SIGHT_WARN(
        "'setIsExecutable/setExecutable/setInexecutable' slots are deprecated and will be removed in Sight 23.0,"
        " please use 'setEnabled/enable/disable' instead."
    );
    this->setEnabled(isExecutable);
}

//-----------------------------------------------------------------------------

bool IAction::getIsExecutable() const
{
    return this->enabled();
}

//-----------------------------------------------------------------------------

void IAction::setVisible(bool visible)
{
    m_visible = visible;
    this->m_registry->actionServiceSetVisible(visible);
}

//-----------------------------------------------------------------------------

void IAction::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void IAction::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

bool IAction::visible() const
{
    return m_visible;
}

//-----------------------------------------------------------------------------

bool IAction::isVisible() const
{
    return visible();
}

//-----------------------------------------------------------------------------

bool IAction::inverted() const
{
    return m_inverted;
}

//-----------------------------------------------------------------------------

bool IAction::isInverted() const
{
    return inverted();
}

//-----------------------------------------------------------------------------

bool IAction::confirmAction()
{
    bool actionIsConfirmed = true;

    if(m_confirmAction && this->getStatus() == IService::STARTED)
    {
        ui::base::dialog::MessageDialog dialog;
        dialog.setTitle("Confirmation");
        std::string message = m_confirmMessage;
        if(message.empty())
        {
            message = "Do you really want to execute this action ?";
        }

        dialog.setMessage(message);

        if(m_defaultButton)
        {
            dialog.setDefaultButton(ui::base::dialog::IMessageDialog::YES);
        }
        else
        {
            dialog.setDefaultButton(ui::base::dialog::IMessageDialog::NO);
        }

        dialog.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        dialog.addButton(ui::base::dialog::IMessageDialog::YES_NO);
        ui::base::dialog::IMessageDialog::Buttons button = dialog.show();

        actionIsConfirmed = (button == ui::base::dialog::IMessageDialog::YES);
    }

    return actionIsConfirmed;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
