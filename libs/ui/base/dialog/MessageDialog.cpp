/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/dialog/MessageDialog.hpp"

#include <core/thread/ActiveWorkers.hpp>

#include <functional>

namespace sight::ui::base
{

namespace dialog
{

//-----------------------------------------------------------------------------

IMessageDialog::Buttons MessageDialog::show(
    const std::string& title,
    const std::string& message,
    ui::base::dialog::IMessageDialog::Icons icon
)
{
    ui::base::dialog::MessageDialog messageBox(title, message, icon);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    return messageBox.show();
}

//-----------------------------------------------------------------------------

IMessageDialog::Buttons MessageDialog::showMessageDialog(
    const std::string& title,
    const std::string& message,
    ui::base::dialog::IMessageDialog::Icons icon
)
{
    return show(title, message, icon);
}

//-----------------------------------------------------------------------------

MessageDialog::MessageDialog()
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(IMessageDialog::REGISTRY_KEY);
                m_implementation                     = ui::base::dialog::IMessageDialog::dynamicCast(guiObj);
            })
    ).wait();
}

//-----------------------------------------------------------------------------

MessageDialog::MessageDialog(
    const std::string& title,
    const std::string& message,
    ui::base::dialog::IMessageDialog::Icons icon
)
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(IMessageDialog::REGISTRY_KEY);
                m_implementation                     = ui::base::dialog::IMessageDialog::dynamicCast(guiObj);

                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                    m_implementation->setMessage(message);
                    m_implementation->setIcon(icon);
                }
            })
    ).wait();
}

//-----------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{
}

//-----------------------------------------------------------------------------

void MessageDialog::setTitle(const std::string& title)
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                }
            })
    ).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setMessage(const std::string& msg)
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                if(m_implementation)
                {
                    m_implementation->setMessage(msg);
                }
            })
    ).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setIcon(ui::base::dialog::IMessageDialog::Icons icon)
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                if(m_implementation)
                {
                    m_implementation->setIcon(icon);
                }
            })
    ).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::addButton(ui::base::dialog::IMessageDialog::Buttons button)
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                if(m_implementation)
                {
                    m_implementation->addButton(button);
                }
            })
    ).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton(ui::base::dialog::IMessageDialog::Buttons button)
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                if(m_implementation)
                {
                    m_implementation->setDefaultButton(button);
                }
            })
    ).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                if(m_implementation)
                {
                    m_implementation->addCustomButton(label, clickedFn);
                }
            })
    ).wait();
}

//-----------------------------------------------------------------------------

ui::base::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    if(m_implementation)
    {
        typedef ui::base::dialog::IMessageDialog::Buttons R;

        std::function<R()> func = std::bind(&IMessageDialog::show, m_implementation);
        std::shared_future<R> f = core::thread::ActiveWorkers::getDefaultWorker()->postTask<R>(func);
        f.wait();

        return f.get();
    }
    else
    {
        return ui::base::dialog::IMessageDialog::NOBUTTON;
    }
}

//-----------------------------------------------------------------------------

} //namespace dialog

} // namespace sight::ui::base
