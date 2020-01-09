/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwGui/dialog/NotificationDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

#include <functional>

namespace fwGui
{
namespace dialog
{

//-----------------------------------------------------------------------------

void NotificationDialog::showNotificationDialog( const std::string& _message,
                                                 INotificationDialog::Type _type, INotificationDialog::Position _pos)
{
    ::fwGui::dialog::NotificationDialog notif( _message, _type, _pos);
    notif.show();
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

} //namespace dialog
} // namespace fwGui
