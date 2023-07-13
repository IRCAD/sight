/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "ui/base/dialog/INotificationDialog.hpp"

namespace sight::ui::base::dialog
{

static const std::string s_defaultMessage = "Empty message";

const INotificationDialog::FactoryRegistryKeyType INotificationDialog::REGISTRY_KEY =
    "::ui::base::dialog::NotificationDialog";

//-----------------------------------------------------------------------------

INotificationDialog::INotificationDialog() :
    m_notification({.message = s_defaultMessage})
{
}

//-----------------------------------------------------------------------------

void INotificationDialog::setMessage(std::string _msg)
{
    if(_msg.empty())
    {
        SIGHT_ERROR("Cannot set an empty message to notification, using 'Empty Message'");
        m_notification.message = s_defaultMessage;
    }
    else
    {
        m_notification.message = std::move(_msg);
    }
}

//-----------------------------------------------------------------------------

void INotificationDialog::setPosition(INotificationDialog::Position _position)
{
    m_notification.position = _position;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setType(INotificationDialog::Type _type)
{
    m_notification.type = _type;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setSize(std::array<int, 2> _size)
{
    m_notification.size = _size;
}

//------------------------------------------------------------------------------

std::array<int, 2> INotificationDialog::getSize() const
{
    return m_notification.size;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setIndex(unsigned int _index)
{
    m_index = _index;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setDuration(std::optional<std::chrono::milliseconds> _durationInMs)
{
    m_notification.duration = _durationInMs;
}

//------------------------------------------------------------------------------

void INotificationDialog::setChannel(std::string _channel)
{
    m_notification.channel = std::move(_channel);
}

//------------------------------------------------------------------------------

std::string INotificationDialog::getChannel() const
{
    return m_notification.channel;
}

//------------------------------------------------------------------------------

void INotificationDialog::setClosable(std::optional<bool> _closable)
{
    m_notification.closable = _closable;
}

//------------------------------------------------------------------------------

std::optional<bool> INotificationDialog::isClosable() const
{
    return m_notification.closable;
}

//------------------------------------------------------------------------------

std::optional<std::chrono::milliseconds> INotificationDialog::getDuration() const
{
    return m_notification.duration;
}

//------------------------------------------------------------------------------

void INotificationDialog::setNotification(service::Notification _notification)
{
    m_notification = std::move(_notification);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::dialog
