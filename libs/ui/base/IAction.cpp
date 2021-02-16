/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>

namespace sight::ui::base
{

const core::com::Slots::SlotKeyType IAction::s_SET_IS_ACTIVE_SLOT     = "setIsActive";
const core::com::Slots::SlotKeyType IAction::s_ACTIVATE_SLOT          = "activate";
const core::com::Slots::SlotKeyType IAction::s_DEACTIVATE_SLOT        = "deactivate";
const core::com::Slots::SlotKeyType IAction::s_SET_IS_EXECUTABLE_SLOT = "setIsExecutable";
const core::com::Slots::SlotKeyType IAction::s_SET_EXECUTABLE_SLOT    = "setExecutable";
const core::com::Slots::SlotKeyType IAction::s_SET_INEXECUTABLE_SLOT  = "setInexecutable";
const core::com::Slots::SlotKeyType IAction::s_SET_VISIBLE_SLOT       = "setVisible";
const core::com::Slots::SlotKeyType IAction::s_SHOW_SLOT              = "show";
const core::com::Slots::SlotKeyType IAction::s_HIDE_SLOT              = "hide";

const core::com::Signals::SignalKeyType IAction::s_ENABLED_SIG   = "enabled";
const core::com::Signals::SignalKeyType IAction::s_DISABLED_SIG  = "disabled";
const core::com::Signals::SignalKeyType IAction::s_CHECKED_SIG   = "checked";
const core::com::Signals::SignalKeyType IAction::s_UNCHECKED_SIG = "unchecked";

IAction::IAction()
{
    newSlot(s_SET_IS_ACTIVE_SLOT, &IAction::setIsActive, this);
    newSlot(s_ACTIVATE_SLOT, &IAction::activate, this);
    newSlot(s_DEACTIVATE_SLOT, &IAction::deactivate, this);
    newSlot(s_SET_IS_EXECUTABLE_SLOT, &IAction::setIsExecutable, this);
    newSlot(s_SET_EXECUTABLE_SLOT, &IAction::setExecutable, this);
    newSlot(s_SET_INEXECUTABLE_SLOT, &IAction::setInexecutable, this);
    newSlot(s_SET_VISIBLE_SLOT, &IAction::setVisible, this);
    newSlot(s_SHOW_SLOT, &IAction::show, this);
    newSlot(s_HIDE_SLOT, &IAction::hide, this);

    m_sigEnabled   = newSignal< EnabledSignalType >(s_ENABLED_SIG);
    m_sigDisabled  = newSignal< DisabledSignalType >(s_DISABLED_SIG);
    m_sigChecked   = newSignal< CheckedSignalType >(s_CHECKED_SIG);
    m_sigUnchecked = newSignal< UncheckedSignalType >(s_UNCHECKED_SIG);
}

//-----------------------------------------------------------------------------

IAction::~IAction()
{
}

//-----------------------------------------------------------------------------

void IAction::initialize()
{
    m_registrar = ui::base::registrar::ActionRegistrar::New(this->getID());

    SLM_ASSERT("Depreciated tag <name> in "<< this->getID() << " configuration.", !m_configuration->hasAttribute(
                   "name"));
    SLM_ASSERT("Depreciated tag <shortcut> in "<< this->getID() << " configuration.",
               !m_configuration->hasAttribute("shortcut"));
    SLM_ASSERT("Depreciated tag <enable> in "<< this->getID() << " configuration.",
               !m_configuration->hasAttribute("enable"));
    SLM_ASSERT(
        "Depreciated tag <specialAction> in "<< this->getID() << " configuration.",
            !m_configuration->hasAttribute("specialAction"));
    SLM_ASSERT("Depreciated tag <style> in "<< this->getID() << " configuration.",
               !m_configuration->hasAttribute("style"));
    SLM_ASSERT("Depreciated tag <state> in "<< this->getID() << " configuration.",
               !m_configuration->hasAttribute("state"));

    core::runtime::ConfigurationElementContainer::Iterator iter;
    for( iter = m_configuration->begin(); iter != m_configuration->end(); ++iter )
    {
        if( (*iter)->getName() == "state" )
        {
            ConfigurationType stateCfg = *iter;

            if( stateCfg->hasAttribute("inverse") )
            {
                std::string invertState = stateCfg->getExistingAttributeValue("inverse");
                SLM_ASSERT("["+this->getID()+"'] Wrong attribute value for 'inverse': must be 'true' or 'false'",
                           (invertState == "true") || (invertState == "false"));
                m_isInverted = (invertState == "true");
            }

            if( stateCfg->hasAttribute("active") )
            {
                std::string isActive = stateCfg->getExistingAttributeValue("active");
                SLM_ASSERT("["+this->getID()+"'] Wrong attribute value for 'active': must be 'true' or 'false'",
                           (isActive == "true") || (isActive == "false"));
                m_isActive = (isActive == "true");
            }

            if( stateCfg->hasAttribute("executable") )
            {
                std::string isExecutable = stateCfg->getExistingAttributeValue("executable");
                SLM_ASSERT("["+this->getID()+"'] Wrong attribute value for 'executable': must be 'true' or 'false'",
                           (isExecutable == "true") || (isExecutable == "false"));
                m_isExecutable = (isExecutable == "true");
            }

            if( stateCfg->hasAttribute("visible") )
            {
                std::string isVisible = stateCfg->getExistingAttributeValue("visible");
                SLM_ASSERT("["+this->getID()+"'] Wrong attribute value for 'visible': must be 'true' or 'false'",
                           (isVisible == "true") || (isVisible == "false"));
                m_isVisible = (isVisible == "true");
            }
        }

        if( (*iter)->getName() == "confirmation" )
        {
            ConfigurationType cfg = *iter;

            SLM_ASSERT("["+this->getID()+"'] Missing attribute 'value' for 'confirmation'", cfg->hasAttribute("value"));
            std::string confirm = cfg->getExistingAttributeValue("value");
            SLM_ASSERT("["+this->getID()+"'] Wrong attribute value  for 'confirmation': must be 'true' or 'false'",
                       (confirm == "true") || (confirm == "false"));
            m_confirmAction = (confirm == "true");

            if( cfg->hasAttribute("message") )
            {
                m_confirmMessage = cfg->getExistingAttributeValue("message");
            }

            if( cfg->hasAttribute("defaultbutton") )
            {
                m_defaultButton = cfg->getExistingAttributeValue("defaultbutton");
            }

        }
    }
}

//-----------------------------------------------------------------------------

void IAction::actionServiceStopping()
{
    this->m_registrar->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void IAction::actionServiceStarting()
{
    this->m_registrar->actionServiceStarting();
    this->setIsActive(m_isActive);
}

//-----------------------------------------------------------------------------

void IAction::setIsActive(bool isActive)
{
    m_isActive = isActive;

    this->m_registrar->actionServiceSetActive(m_isActive);
    if(m_isActive)
    {
        m_sigChecked->asyncEmit();
    }
    else
    {
        m_sigUnchecked->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void IAction::activate()
{
    this->setIsActive(true);
}

//-----------------------------------------------------------------------------

void IAction::deactivate()
{
    this->setIsActive(false);
}

//-----------------------------------------------------------------------------

bool IAction::getIsActive() const
{
    return m_isActive;
}

//-----------------------------------------------------------------------------

void IAction::setIsExecutable(bool isExecutable)
{
    m_isExecutable = isExecutable;

    this->m_registrar->actionServiceSetExecutable(m_isExecutable);
    if(m_isExecutable)
    {
        m_sigEnabled->asyncEmit();
    }
    else
    {
        m_sigDisabled->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void IAction::setExecutable()
{
    this->setIsExecutable(true);
}

//-----------------------------------------------------------------------------

void IAction::setInexecutable()
{
    this->setIsExecutable(false);
}

//-----------------------------------------------------------------------------

bool IAction::getIsExecutable() const
{
    return m_isExecutable;
}

//-----------------------------------------------------------------------------

void IAction::setVisible(bool isVisible)
{
    m_isVisible = isVisible;
    this->m_registrar->actionServiceSetVisible(isVisible);
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

bool IAction::isVisible() const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

bool IAction::getActiveStateValue() const
{
    FW_DEPRECATED_MSG("`getActiveStateValue` is deprecated, please use `isInverted` instead.", "21.0");
    return this->isInverted();
}

//-----------------------------------------------------------------------------

bool IAction::isInverted() const
{
    return m_isInverted;
}

//-----------------------------------------------------------------------------

bool IAction::confirmAction()
{
    bool actionIsConfirmed = true;

    if (m_confirmAction)
    {
        ui::base::dialog::MessageDialog dialog;
        dialog.setTitle("Confirmation");
        std::stringstream ss;
        ss << "Do you really want to execute this action ? ";
        if (!m_confirmMessage.empty())
        {
            ss << std::endl << m_confirmMessage;
        }
        dialog.setMessage( ss.str() );

        if(m_defaultButton == "yes")
        {
            dialog.setDefaultButton( ui::base::dialog::IMessageDialog::YES );
        }
        else if(m_defaultButton == "no")
        {
            dialog.setDefaultButton( ui::base::dialog::IMessageDialog::NO );
        }
        else if(!m_defaultButton.empty())
        {
            SLM_WARN("unknown button: " + m_defaultButton);
        }

        dialog.setIcon( ui::base::dialog::IMessageDialog::QUESTION );
        dialog.addButton( ui::base::dialog::IMessageDialog::YES_NO );
        ui::base::dialog::IMessageDialog::Buttons button = dialog.show();

        actionIsConfirmed = (button == ui::base::dialog::IMessageDialog::YES);
    }

    return actionIsConfirmed;
}
//-----------------------------------------------------------------------------

}
