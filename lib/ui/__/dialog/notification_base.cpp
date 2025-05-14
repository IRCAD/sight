/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "ui/__/dialog/notification_base.hpp"

namespace sight::ui::dialog
{

static const std::string DEFAULT_MESSAGE = "Empty message";

const notification_base::factory_registry_key_t notification_base::REGISTRY_KEY =
    "::ui::dialog::notification";

//-----------------------------------------------------------------------------

notification_base::notification_base() :
    m_notification({.m_message = DEFAULT_MESSAGE})
{
}

//-----------------------------------------------------------------------------

void notification_base::set_message(std::string _msg)
{
    if(_msg.empty())
    {
        SIGHT_ERROR("Cannot set an empty message to notification, using 'Empty Message'");
        m_notification.m_message = DEFAULT_MESSAGE;
    }
    else
    {
        m_notification.m_message = std::move(_msg);
    }
}

//-----------------------------------------------------------------------------

void notification_base::set_position(notification_base::position _position)
{
    m_notification.m_position = _position;
}

//-----------------------------------------------------------------------------

void notification_base::set_type(notification_base::type _type)
{
    m_notification.m_type = _type;
}

//-----------------------------------------------------------------------------

void notification_base::set_size(std::array<int, 2> _size)
{
    m_notification.m_size = _size;
}

//------------------------------------------------------------------------------

std::array<int, 2> notification_base::size() const
{
    return m_notification.m_size;
}

//-----------------------------------------------------------------------------

void notification_base::set_index(unsigned int _index)
{
    m_index = _index;
}

//-----------------------------------------------------------------------------

void notification_base::set_duration(std::optional<std::chrono::milliseconds> _duration_in_ms)
{
    m_notification.m_duration = _duration_in_ms;
}

//------------------------------------------------------------------------------

void notification_base::set_channel(std::string _channel)
{
    m_notification.m_channel = std::move(_channel);
}

//------------------------------------------------------------------------------

std::string notification_base::get_channel() const
{
    return m_notification.m_channel;
}

//------------------------------------------------------------------------------

void notification_base::set_closable(std::optional<bool> _closable)
{
    m_notification.m_closable = _closable;
}

//------------------------------------------------------------------------------

std::optional<bool> notification_base::is_closable() const
{
    return m_notification.m_closable;
}

//------------------------------------------------------------------------------

std::optional<std::chrono::milliseconds> notification_base::get_duration() const
{
    return m_notification.m_duration;
}

//------------------------------------------------------------------------------

void notification_base::set_notification(service::notification _notification)
{
    m_notification = std::move(_notification);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
