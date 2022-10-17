/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
    m_message(s_defaultMessage),
    m_fullMessage(m_message)
{
}

//-----------------------------------------------------------------------------

INotificationDialog::~INotificationDialog()
= default;

//-----------------------------------------------------------------------------

void INotificationDialog::setMessage(const std::string& _msg)
{
    if(_msg.empty())
    {
        SIGHT_ERROR("Cannot set an empty message to notification, using 'Empty Message'");
        m_message     = s_defaultMessage;
        m_fullMessage = m_message;
        return;
    }

    m_message     = _msg;
    m_fullMessage = m_message;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setPosition(INotificationDialog::Position _position)
{
    m_position = _position;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setType(INotificationDialog::Type _type)
{
    m_notificationType = _type;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setSize(unsigned int _width, unsigned int _height)
{
    m_size[0] = _width;
    m_size[1] = _height;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setIndex(unsigned int _index)
{
    m_index = _index;
}

//-----------------------------------------------------------------------------

void INotificationDialog::setDuration(int _durationInMs)
{
    m_duration = _durationInMs;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::dialog
