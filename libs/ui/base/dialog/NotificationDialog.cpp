/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/base/dialog/NotificationDialog.hpp"

#include <core/thread/Worker.hpp>

#include <functional>

namespace sight::ui::base::dialog
{

//-----------------------------------------------------------------------------

void NotificationDialog::show(service::Notification notification)
{
    ui::base::dialog::NotificationDialog notif(std::move(notification));
    notif.show();
}

//-----------------------------------------------------------------------------

NotificationDialog::NotificationDialog()
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(INotificationDialog::REGISTRY_KEY);
            m_implementation                     = ui::base::dialog::INotificationDialog::dynamicCast(guiObj);
        }).wait();
}

//-----------------------------------------------------------------------------

NotificationDialog::NotificationDialog(service::Notification notification)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(INotificationDialog::REGISTRY_KEY);
            m_implementation                     = ui::base::dialog::INotificationDialog::dynamicCast(guiObj);

            if(m_implementation)
            {
                m_implementation->setNotification(std::move(notification));
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::show()
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->show();
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setMessage(std::string _msg)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setMessage(std::move(_msg));
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setType(INotificationDialog::Type _type)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setType(_type);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setPosition(INotificationDialog::Position _position)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setPosition(_position);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setSize(std::array<int, 2> _size)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setSize(_size);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::array<int, 2> NotificationDialog::getSize() const
{
    if(m_implementation)
    {
        return m_implementation->getSize();
    }

    return INotificationDialog::getSize();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setIndex(unsigned int _index)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setIndex(_index);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setDuration(std::optional<std::chrono::milliseconds> _durationInMs)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setDuration(_durationInMs);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::optional<std::chrono::milliseconds> NotificationDialog::getDuration() const
{
    if(m_implementation)
    {
        return m_implementation->getDuration();
    }

    return INotificationDialog::getDuration();
}

//------------------------------------------------------------------------------

void NotificationDialog::setChannel(std::string _channel)
{
    std::string channel(_channel);
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setChannel(std::move(_channel));
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::string NotificationDialog::getChannel() const
{
    if(m_implementation)
    {
        return m_implementation->getChannel();
    }

    return INotificationDialog::getChannel();
}

//------------------------------------------------------------------------------

void NotificationDialog::setClosable(std::optional<bool> _closable)
{
    std::optional<bool> is_closable(_closable);
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setClosable(is_closable);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::optional<bool> NotificationDialog::isClosable() const
{
    if(m_implementation)
    {
        return m_implementation->isClosable();
    }

    return INotificationDialog::isClosable();
}

//-----------------------------------------------------------------------------

bool NotificationDialog::isVisible() const
{
    bool visible = false;
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                visible = m_implementation->isVisible();
            }
        }).wait();

    return visible;
}

//-----------------------------------------------------------------------------

void NotificationDialog::close() const
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->close();
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::moveDown()
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->moveDown();
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setContainer(container::fwContainer::csptr _container)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setContainer(_container);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setClosedCallback(std::function<void()> f)
{
    core::thread::getDefaultWorker()->postTask<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setClosedCallback(f);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::dialog
