/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include "display_test_notifications.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace ex_notifications
{

static const sight::core::com::slots::key_t SET_ENUM_PARAMETER_SLOT = "set_enum_parameter";
static const sight::core::com::slots::key_t SET_BOOL_PARAMETER_SLOT = "set_bool_parameter";
static const sight::core::com::slots::key_t CLOSE_CHANNEL1_SLOT     = "close_channel1";

//------------------------------------------------------------------------------

display_test_notifications::display_test_notifications() noexcept :
    notifier(m_signals)
{
    new_slot(SET_ENUM_PARAMETER_SLOT, &display_test_notifications::set_enum_parameter, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &display_test_notifications::set_bool_parameter, this);
    new_slot(CLOSE_CHANNEL1_SLOT, &display_test_notifications::close_channel1, this);
}

//------------------------------------------------------------------------------

void display_test_notifications::set_enum_parameter(std::string _val, std::string _key)
{
    if(_key == "position")
    {
        m_display_all = false;
        if(_val == "ALL")
        {
            m_display_all = true;
        }
        else if(_val == "TOP_LEFT")
        {
            m_notification.position = ::dial::notification::position::top_left;
        }
        else if(_val == "TOP_RIGHT")
        {
            m_notification.position = ::dial::notification::position::top_right;
        }
        else if(_val == "CENTERED_TOP")
        {
            m_notification.position = ::dial::notification::position::centered_top;
        }
        else if(_val == "CENTERED")
        {
            m_notification.position = ::dial::notification::position::centered;
        }
        else if(_val == "BOTTOM_LEFT")
        {
            m_notification.position = ::dial::notification::position::bottom_left;
        }
        else if(_val == "BOTTOM_RIGHT")
        {
            m_notification.position = ::dial::notification::position::bottom_right;
        }
        else if(_val == "CENTERED_BOTTOM")
        {
            m_notification.position = ::dial::notification::position::centered_bottom;
        }
        else
        {
            SIGHT_ERROR(std::string("Value '") + _val + "' is not handled for key " + _key);
        }
    }
    else if(_key == "type")
    {
        if(_val == "SUCCESS")
        {
            m_notification.type = ::dial::notification::type::success;
        }
        else if(_val == "INFO")
        {
            m_notification.type = ::dial::notification::type::info;
        }
        else if(_val == "FAILURE")
        {
            m_notification.type = ::dial::notification::type::failure;
        }
        else
        {
            SIGHT_ERROR(std::string("Value '") + _val + "' is not handled for key " + _key);
        }
    }
    else if(_key == "duration")
    {
        try
        {
            if(_val == "infinite")
            {
                m_notification.duration = std::nullopt;
            }
            else
            {
                m_notification.duration = std::chrono::milliseconds(std::stoul(_val));
            }
        }
        catch(...)
        {
            SIGHT_ERROR(std::string("Value '") + _val + "' is not handled for key " + _key);
        }
    }
    else if(_key == "channel")
    {
        if(_val == "CHANNEL1")
        {
            m_notification.channel  = "CHANNEL1";
            m_notification.closable = std::nullopt;
        }
        else if(_val == "CHANNEL2")
        {
            m_notification.channel = "CHANNEL2";
        }
        else if(_val == "GLOBAL")
        {
            m_notification.channel.clear();
            m_notification.closable = std::nullopt;
        }
        else
        {
            SIGHT_ERROR(std::string("Value '") + _val + "' is not handled for key " + _key);
        }
    }
    else
    {
        SIGHT_ERROR("Key '" + _key + "' is not handled.");
    }
}

//------------------------------------------------------------------------------

void display_test_notifications::set_bool_parameter(bool _val, std::string _key)
{
    if(_key == "usenotifier")
    {
        m_usenotifier = _val;
    }
    else if(_key == "reachMaxCharacters")
    {
        m_reach_max_characters = _val;
    }
    else if(_key == "sound")
    {
        m_notification.sound = _val;
    }
    else
    {
        SIGHT_ERROR("Key '" + _key + "' is not handled.");
    }
}

//------------------------------------------------------------------------------

void display_test_notifications::close_channel1()
{
    close_notification("CHANNEL1");
}

//------------------------------------------------------------------------------

void display_test_notifications::info(std::ostream& _sstream)
{
    _sstream << "display_test_notifications" << std::endl;
}

//------------------------------------------------------------------------------

void display_test_notifications::configuring()
{
    this->action::initialize();
    this->notifier::initialize(this->get_config());
}

//------------------------------------------------------------------------------

void display_test_notifications::updating()
{
    static std::uint64_t count = 1;

    std::string channel = m_notification.channel;
    if(!channel.empty())
    {
        channel += ", ";
    }

    std::string duration = "timeout:";

    if(channel == "CHANNEL2")
    {
        duration += "5s";
    }
    else if(m_notification.duration && m_notification.duration->count() > 0)
    {
        duration += std::to_string(
            std::chrono::duration_cast<std::chrono::seconds>(*m_notification.duration).count()
                    ) + "s";
    }
    else
    {
        duration += "infinite";
    }

    std::string closable;

    if(channel != "CHANNEL2"
       && ((m_notification.closable && *m_notification.closable)
           || (!m_notification.closable && m_notification.duration && m_notification.duration->count() > 0)))
    {
        closable = ", closable";
    }

    std::string message = "[" + channel + duration + closable + "] " + std::to_string(count);

    if(m_usenotifier)
    {
        // Mode 1: You use the notifier service that will display for you the notifications, you need to emit the
        // proper signal.
        // Notification will always be displayed at the same place,
        // and will be queued if several notifications are displayed at the same time.

        if(m_reach_max_characters)
        {
            message = "This notification "
                      + std::to_string(count)
                      + " will exceeds the maximum allowed characters ! "
                        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
                        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
                        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
                        "nisi ut aliquip ex ea commodo consequat. "
                        "Duis aute irure dolor in reprehenderit in voluptate "
                        "velit esse cillum dolore eu fugiat nulla pariatur. "
                        "Excepteur sint occaecat cupidatat non proident, "
                        "sunt in culpa qui officia deserunt mollit anim id est laborum.";
        }

        this->notify(
            {
                .type     = m_notification.type,
                .message  = message,
                .duration = m_notification.duration,
                .channel  = m_notification.channel,
                .closable = m_notification.closable,
                .sound    = m_notification.sound
            });
    }
    else
    {
        // Mode 2: Standalone, you decide where to pop the notification by calling directly the notification.
        if(m_display_all)
        {
            using position_t = enum sight::service::notification::position;

            for(const auto& position : {
                    position_t::top_left,
                    position_t::top_right,
                    position_t::centered_top,
                    position_t::centered,
                    position_t::bottom_left,
                    position_t::bottom_right,
                    position_t::centered_bottom
                })
            {
                ::dial::notification::show(
                    sight::service::notification
                    {
                        .type     = m_notification.type,
                        .position = position,
                        .message  = message,
                        .duration = m_notification.duration,
                        .channel  = m_notification.channel,
                        .closable = m_notification.closable
                    });
            }
        }
        else
        {
            ::dial::notification::show(
                sight::service::notification
                {
                    .type     = m_notification.type,
                    .position = m_notification.position,
                    .message  = message,
                    .duration = m_notification.duration,
                    .channel  = m_notification.channel,
                    .closable = m_notification.closable
                });
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

void display_test_notifications::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void display_test_notifications::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace ex_notifications
