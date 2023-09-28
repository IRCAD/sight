/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ui/__/dialog/input.hpp"

#include <core/thread/worker.hpp>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

std::pair<std::string, bool> input::showInputDialog(
    const std::string& title,
    const std::string& message,
    const std::string& text,
    EchoMode echoMode
)
{
    ui::dialog::input inputBox(title, message, text, echoMode);
    return inputBox.getInput();
}

//-----------------------------------------------------------------------------

input::input()
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            ui::object::sptr guiObj = ui::factory::make(input_base::REGISTRY_KEY);
            m_implementation        = std::dynamic_pointer_cast<ui::dialog::input_base>(guiObj);
        })
    );
}

//-----------------------------------------------------------------------------

input::input(
    const std::string& title,
    const std::string& message,
    const std::string& text,
    EchoMode echoMode
)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            ui::object::sptr guiObj = ui::factory::make(input_base::REGISTRY_KEY);
            m_implementation        = std::dynamic_pointer_cast<ui::dialog::input_base>(guiObj);
            m_implementation->setTitle(title);
            m_implementation->setMessage(message);
            m_implementation->setInput(text);
            m_implementation->setEchoMode(echoMode);
        })
    );
}

//-----------------------------------------------------------------------------

input::~input()
= default;

//-----------------------------------------------------------------------------

void input::setTitle(const std::string& title)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->setTitle(title);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void input::setMessage(const std::string& msg)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->setMessage(msg);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void input::setInput(const std::string& text)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->setInput(text);
        })
    ).wait();
}

//------------------------------------------------------------------------------

void input::setEchoMode(EchoMode echoMode)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->setEchoMode(echoMode);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

std::pair<std::string, bool> input::getInput()
{
    std::function<std::pair<std::string, bool>()> func  = [this](auto&& ...){return m_implementation->getInput();};
    std::shared_future<std::pair<std::string, bool> > f =
        core::thread::get_default_worker()->post_task<std::pair<std::string, bool> >(func);
    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
