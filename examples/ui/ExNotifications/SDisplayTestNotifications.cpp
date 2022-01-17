/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "SDisplayTestNotifications.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

namespace ExNotifications
{

static const sight::core::com::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT = "setEnumParameter";
static const sight::core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT = "setBoolParameter";

using namespace sight;

//------------------------------------------------------------------------------

SDisplayTestNotifications::SDisplayTestNotifications() noexcept
{
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SDisplayTestNotifications::setEnumParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SDisplayTestNotifications::setBoolParameter, this);
}

//------------------------------------------------------------------------------

SDisplayTestNotifications::~SDisplayTestNotifications() noexcept
{
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::setEnumParameter(std::string _val, std::string _key)
{
    if(_key == "position")
    {
        m_displayAll = false;
        if(_val == "ALL")
        {
            m_displayAll = true;
        }
        else if(_val == "TOP_LEFT")
        {
            m_position = ::dial::NotificationDialog::Position::TOP_LEFT;
        }
        else if(_val == "TOP_RIGHT")
        {
            m_position = ::dial::NotificationDialog::Position::TOP_RIGHT;
        }
        else if(_val == "CENTERED_TOP")
        {
            m_position = ::dial::NotificationDialog::Position::CENTERED_TOP;
        }
        else if(_val == "CENTERED")
        {
            m_position = ::dial::NotificationDialog::Position::CENTERED;
        }
        else if(_val == "BOTTOM_LEFT")
        {
            m_position = ::dial::NotificationDialog::Position::BOTTOM_LEFT;
        }
        else if(_val == "BOTTOM_RIGHT")
        {
            m_position = ::dial::NotificationDialog::Position::BOTTOM_RIGHT;
        }
        else if(_val == "CENTERED_BOTTOM")
        {
            m_position = ::dial::NotificationDialog::Position::CENTERED_BOTTOM;
        }
        else
        {
            SIGHT_ERROR("Value '" + _val + "' is not handled for key " + _key);
        }
    }
    else if(_key == "type")
    {
        if(_val == "SUCCESS")
        {
            m_type = ::dial::NotificationDialog::Type::SUCCESS;
        }
        else if(_val == "INFO")
        {
            m_type = ::dial::NotificationDialog::Type::INFO;
        }
        else if(_val == "FAILURE")
        {
            m_type = ::dial::NotificationDialog::Type::FAILURE;
        }
        else
        {
            SIGHT_ERROR("Value '" + _val + "' is not handled for key " + _key);
        }
    }
    else
    {
        SIGHT_ERROR("Key '" + _key + "' is not handled.");
    }
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::setBoolParameter(bool _val, std::string _key)
{
    if(_key == "useSNotifier")
    {
        m_useSNotifier = _val;
    }
    else if(_key == "reachMaxCharacters")
    {
        m_reachMaxCharacters = _val;
    }
    else
    {
        SIGHT_ERROR("Key '" + _key + "' is not handled.");
    }
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::info(std::ostream& _sstream)
{
    _sstream << "SDisplayTestNotifications" << std::endl;
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::configuring()
{
    this->sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::updating()
{
    static std::uint64_t count = 1;

    std::string message = "Notification Test " + std::to_string(count) + " !";
    if(m_useSNotifier)
    {
        // Mode 1: You use the SNotifier service that will display for you the notifications, you need to emit the
        // proper signal.
        // Notification will always be displayed at the same place,
        // and will be queued if several notifications are displayed at the same time.

        if(m_reachMaxCharacters)
        {
            message = "This notification " + std::to_string(count) + " will exceeds the maximum allowed characters ! " \
                                                                     "Lorem ipsum dolor sit amet, consectetur adipiscing elit, " \
                                                                     "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. " \
                                                                     "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris " \
                                                                     "nisi ut aliquip ex ea commodo consequat. " \
                                                                     "Duis aute irure dolor in reprehenderit in voluptate " \
                                                                     "velit esse cillum dolore eu fugiat nulla pariatur. " \
                                                                     "Excepteur sint occaecat cupidatat non proident, " \
                                                                     "sunt in culpa qui officia deserunt mollit anim id est laborum.";
        }

        if(m_type == ::dial::NotificationDialog::Type::SUCCESS)
        {
            this->notify(NotificationType::SUCCESS, message);
        }
        else if(m_type == ::dial::NotificationDialog::Type::FAILURE)
        {
            this->notify(NotificationType::FAILURE, message);
        }
        else
        {
            this->notify(NotificationType::INFO, message);
        }
    }
    else
    {
        // Mode 2: Standalone, you decide where to pop the notification by calling directly the NotificationDialog.
        if(m_displayAll)
        {
            ::dial::NotificationDialog::show(
                message,
                m_type,
                ::dial::NotificationDialog::Position::TOP_LEFT
            );
            ::dial::NotificationDialog::show(
                message,
                m_type,
                ::dial::NotificationDialog::Position::TOP_RIGHT
            );
            ::dial::NotificationDialog::show(
                message,
                m_type,
                ::dial::NotificationDialog::Position::CENTERED_TOP
            );
            ::dial::NotificationDialog::show(
                message,
                m_type,
                ::dial::NotificationDialog::Position::CENTERED
            );
            ::dial::NotificationDialog::show(
                message,
                m_type,
                ::dial::NotificationDialog::Position::BOTTOM_LEFT
            );
            ::dial::NotificationDialog::show(
                message,
                m_type,
                ::dial::NotificationDialog::Position::BOTTOM_RIGHT
            );
            ::dial::NotificationDialog::show(
                message,
                m_type,
                ::dial::NotificationDialog::Position::CENTERED_BOTTOM
            );
        }
        else
        {
            ::dial::NotificationDialog::show(message, m_type, m_position);
        }
    }

    // Good luck to reach this one, if it is the case return to 0.
    if(count == std::numeric_limits<std::uint64_t>::max())
    {
        count = 0;
    }

    ++count;
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::starting()
{
    this->sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::stopping()
{
    this->sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace ExNotifications
