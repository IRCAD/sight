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

#include "ui/__/dialog/notification.hpp"

#include <core/thread/worker.hpp>

#include <functional>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

void notification::show(service::Notification _notification)
{
    ui::dialog::notification notif(std::move(_notification));
    notif.show();
}

//-----------------------------------------------------------------------------

notification::notification()
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(notification_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::notification_base>(gui_obj);
        }).wait();
}

//-----------------------------------------------------------------------------

notification::notification(service::Notification _notification)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(notification_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::notification_base>(gui_obj);

            if(m_implementation)
            {
                m_implementation->setNotification(std::move(_notification));
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::show()
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->show();
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::setMessage(std::string _msg)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setMessage(std::move(_msg));
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::setType(notification_base::Type _type)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setType(_type);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::setPosition(notification_base::Position _position)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setPosition(_position);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::setSize(std::array<int, 2> _size)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setSize(_size);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::array<int, 2> notification::size() const
{
    if(m_implementation)
    {
        return m_implementation->size();
    }

    return notification_base::size();
}

//-----------------------------------------------------------------------------

void notification::setIndex(unsigned int _index)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setIndex(_index);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::setDuration(std::optional<std::chrono::milliseconds> _duration_in_ms)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setDuration(_duration_in_ms);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::optional<std::chrono::milliseconds> notification::getDuration() const
{
    if(m_implementation)
    {
        return m_implementation->getDuration();
    }

    return notification_base::getDuration();
}

//------------------------------------------------------------------------------

void notification::setChannel(std::string _channel)
{
    std::string channel(_channel);
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setChannel(std::move(_channel));
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::string notification::getChannel() const
{
    if(m_implementation)
    {
        return m_implementation->getChannel();
    }

    return notification_base::getChannel();
}

//------------------------------------------------------------------------------

void notification::setClosable(std::optional<bool> _closable)
{
    std::optional<bool> is_closable(_closable);
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setClosable(is_closable);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::optional<bool> notification::isClosable() const
{
    if(m_implementation)
    {
        return m_implementation->isClosable();
    }

    return notification_base::isClosable();
}

//-----------------------------------------------------------------------------

bool notification::isVisible() const
{
    bool visible = false;
    core::thread::get_default_worker()->post_task<void>(
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

void notification::close() const
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->close();
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::moveDown()
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->moveDown();
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::setContainer(container::widget::csptr _container)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setContainer(_container);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::setClosedCallback(std::function<void()> _f)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setClosedCallback(_f);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
