/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/dialog/InputDialog.hpp"

#include <core/thread/Worker.hpp>

namespace sight::ui::base
{

namespace dialog
{

//-----------------------------------------------------------------------------

std::string InputDialog::showInputDialog(
    const std::string& title,
    const std::string& message,
    const std::string& text,
    EchoMode echoMode
)
{
    ui::base::dialog::InputDialog inputBox(title, message, text, echoMode);
    return inputBox.getInput();
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog()
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(IInputDialog::REGISTRY_KEY);
                m_implementation                     = ui::base::dialog::IInputDialog::dynamicCast(guiObj);
            })
    );
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog(
    const std::string& title,
    const std::string& message,
    const std::string& text,
    EchoMode echoMode
)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(IInputDialog::REGISTRY_KEY);
                m_implementation                     = ui::base::dialog::IInputDialog::dynamicCast(guiObj);
                m_implementation->setTitle(title);
                m_implementation->setMessage(message);
                m_implementation->setInput(text);
                m_implementation->setEchoMode(echoMode);
            })
    );
}

//-----------------------------------------------------------------------------

InputDialog::~InputDialog()
{
}

//-----------------------------------------------------------------------------

void InputDialog::setTitle(const std::string& title)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                m_implementation->setTitle(title);
            })
    ).wait();
}

//-----------------------------------------------------------------------------

void InputDialog::setMessage(const std::string& msg)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                m_implementation->setMessage(msg);
            })
    ).wait();
}

//-----------------------------------------------------------------------------

void InputDialog::setInput(const std::string& text)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                m_implementation->setInput(text);
            })
    ).wait();
}

//------------------------------------------------------------------------------

void InputDialog::setEchoMode(EchoMode echoMode)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                m_implementation->setEchoMode(echoMode);
            })
    ).wait();
}

//-----------------------------------------------------------------------------

std::string InputDialog::getInput()
{
    std::function<std::string()> func = std::bind(&IInputDialog::getInput, m_implementation);
    std::shared_future<std::string> f =
        core::thread::getDefaultWorker()->postTask<std::string>(func);
    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

} //namespace dialog

} // namespace sight::ui::base
