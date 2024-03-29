/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/dialog/message.hpp"

#include <core/thread/worker.hpp>

#include <functional>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

message::buttons message::show(
    const std::string& _title,
    const std::string& _message,
    message::icons _icon
)
{
    ui::dialog::message message_box(_title, _message, _icon);
    message_box.add_button(ui::dialog::message::ok);
    return message_box.show();
}

//-----------------------------------------------------------------------------

message::message()
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(message_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::message_base>(gui_obj);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

message::message(
    const std::string& _title,
    const std::string& _message,
    message::icons _icon
)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(message_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::message_base>(gui_obj);

            if(m_implementation)
            {
                m_implementation->set_title(_title);
                m_implementation->set_message(_message);
                m_implementation->set_icon(_icon);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

message::~message()
= default;

//-----------------------------------------------------------------------------

void message::set_title(const std::string& _title)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->set_title(_title);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void message::set_message(const std::string& _msg)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->set_message(_msg);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void message::set_icon(message::icons _icon)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->set_icon(_icon);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void message::add_button(message::buttons _button)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->add_button(_button);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void message::set_default_button(message::buttons _button)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->set_default_button(_button);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void message::add_custom_button(const std::string& _label, std::function<void()> _clicked_fn)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->add_custom_button(_label, _clicked_fn);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

message::buttons message::show()
{
    if(m_implementation)
    {
        using ret_t = message::buttons;

        std::function<ret_t()> func = [this](auto&& ...){return m_implementation->show();};
        std::shared_future<ret_t> f = core::thread::get_default_worker()->post_task<ret_t>(func);
        f.wait();

        return f.get();
    }

    return message::nobutton;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
