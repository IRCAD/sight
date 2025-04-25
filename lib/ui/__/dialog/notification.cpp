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

void notification::show(service::notification _notification)
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

notification::notification(service::notification _notification)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(notification_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::notification_base>(gui_obj);

            if(m_implementation)
            {
                m_implementation->set_notification(std::move(_notification));
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

void notification::set_message(std::string _msg)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_message(std::move(_msg));
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::set_type(notification_base::type _type)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_type(_type);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::set_position(notification_base::position _position)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_position(_position);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::set_size(std::array<int, 2> _size)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_size(_size);
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

void notification::set_index(unsigned int _index)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_index(_index);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::set_duration(std::optional<std::chrono::milliseconds> _duration_in_ms)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_duration(_duration_in_ms);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::optional<std::chrono::milliseconds> notification::get_duration() const
{
    if(m_implementation)
    {
        return m_implementation->get_duration();
    }

    return notification_base::get_duration();
}

//------------------------------------------------------------------------------

void notification::set_channel(std::string _channel)
{
    std::string channel(_channel);
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_channel(std::move(_channel));
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::string notification::get_channel() const
{
    if(m_implementation)
    {
        return m_implementation->get_channel();
    }

    return notification_base::get_channel();
}

//------------------------------------------------------------------------------

void notification::set_closable(std::optional<bool> _closable)
{
    std::optional<bool> is_closable(_closable);
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_closable(is_closable);
            }
        }).wait();
}

//------------------------------------------------------------------------------

std::optional<bool> notification::is_closable() const
{
    if(m_implementation)
    {
        return m_implementation->is_closable();
    }

    return notification_base::is_closable();
}

//-----------------------------------------------------------------------------

bool notification::is_visible() const
{
    bool visible = false;
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                visible = m_implementation->is_visible();
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

void notification::move_down()
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->move_down();
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::set_container(container::widget::csptr _container)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_container(_container);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void notification::set_closed_callback(std::function<void()> _f)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->set_closed_callback(_f);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
