/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

void NotificationDialog::show(
    const std::string& _message,
    INotificationDialog::Type _type,
    INotificationDialog::Position _pos
)
{
    ui::base::dialog::NotificationDialog notif(_message, _type, _pos);
    notif.show();
}

//-----------------------------------------------------------------------------

NotificationDialog::NotificationDialog()
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(INotificationDialog::REGISTRY_KEY);
            m_implementation                     = ui::base::dialog::INotificationDialog::dynamicCast(guiObj);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

NotificationDialog::NotificationDialog(
    const std::string& _message,
    INotificationDialog::Type _type,
    INotificationDialog::Position _pos
)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(INotificationDialog::REGISTRY_KEY);
            m_implementation                     = ui::base::dialog::INotificationDialog::dynamicCast(guiObj);

            if(m_implementation)
            {
                m_implementation->setMessage(_message);
                m_implementation->setType(_type);
                m_implementation->setPosition(_pos);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

NotificationDialog::~NotificationDialog()
= default;

//-----------------------------------------------------------------------------

void NotificationDialog::show()
{
    if(m_implementation)
    {
        std::function<void()> func = [this](auto&& ...){m_implementation->show();};
        std::shared_future<void> f = core::thread::getDefaultWorker()->postTask<void>(func);
        f.wait();
    }
}

//-----------------------------------------------------------------------------

void NotificationDialog::setMessage(const std::string& _msg)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setMessage(_msg);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setType(INotificationDialog::Type _type)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setType(_type);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setPosition(INotificationDialog::Position _position)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setPosition(_position);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setSize(unsigned int _width, unsigned int _height)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setSize(_width, _height);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setIndex(unsigned int _index)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setIndex(_index);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setDuration(int _durationInMs)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setDuration(_durationInMs);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

bool NotificationDialog::isVisible() const
{
    bool visible = false;
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                visible = m_implementation->isVisible();
            }
        })
    ).wait();

    return visible;
}

//-----------------------------------------------------------------------------

void NotificationDialog::close() const
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->close();
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::moveDown()
{
    if(m_implementation)
    {
        std::function<void()> func = [this](auto&& ...){m_implementation->moveDown();};
        std::shared_future<void> f = core::thread::getDefaultWorker()->postTask<void>(func);
        f.wait();
    }
}

//-----------------------------------------------------------------------------

void NotificationDialog::setContainer(container::fwContainer::csptr _container)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setContainer(_container);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setClosedCallback(std::function<void()> f)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            if(m_implementation)
            {
                m_implementation->setClosedCallback(f);
            }
        })
    ).wait();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::dialog
