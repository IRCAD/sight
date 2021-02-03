/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwGui/dialog/NotificationDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

#include <functional>

namespace fwGui
{
namespace dialog
{

//-----------------------------------------------------------------------------

void NotificationDialog::show( const std::string& _message,
                               INotificationDialog::Type _type, INotificationDialog::Position _pos)
{
    ::fwGui::dialog::NotificationDialog notif( _message, _type, _pos);
    notif.show();
}

//-----------------------------------------------------------------------------

void NotificationDialog::showNotificationDialog( const std::string& _message,
                                                 INotificationDialog::Type _type, INotificationDialog::Position _pos)
{
    ::fwGui::dialog::NotificationDialog::show(_message, _type, _pos);
}

//-----------------------------------------------------------------------------

NotificationDialog::NotificationDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(INotificationDialog::REGISTRY_KEY);
                m_implementation                    = ::fwGui::dialog::INotificationDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

NotificationDialog::NotificationDialog(
    const std::string& _message, INotificationDialog::Type _type, INotificationDialog::Position _pos)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(INotificationDialog::REGISTRY_KEY);
                m_implementation                    = ::fwGui::dialog::INotificationDialog::dynamicCast(guiObj);

                if(m_implementation)
                {
                    m_implementation->setMessage(_message);
                    m_implementation->setType(_type);
                    m_implementation->setPosition(_pos);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

NotificationDialog::~NotificationDialog()
{
}

//-----------------------------------------------------------------------------

void NotificationDialog::show()
{
    if(m_implementation)
    {
        std::function<void()> func = std::bind(&INotificationDialog::show, m_implementation);
        std::shared_future<void> f = ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(func);
        f.wait();
    }
}

//-----------------------------------------------------------------------------

void NotificationDialog::setMessage(const std::string& _msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setMessage(_msg);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setType(INotificationDialog::Type _type)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setType(_type);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setPosition(INotificationDialog::Position _position)
{

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setPosition(_position);
                }
            })).wait();

}

//-----------------------------------------------------------------------------

void NotificationDialog::setSize(unsigned int _width, unsigned int _height)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setSize(_width, _height);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setIndex(unsigned int _index)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setIndex(_index);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setDuration(int _durationInMs)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setDuration(_durationInMs);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

bool NotificationDialog::isVisible() const
{

    bool visible = false;
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    visible = m_implementation->isVisible();
                }
            })).wait();

    return visible;
}

//-----------------------------------------------------------------------------

void NotificationDialog::close() const
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->close();
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void NotificationDialog::setContainer(container::fwContainer::csptr _container)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setContainer(_container);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui
