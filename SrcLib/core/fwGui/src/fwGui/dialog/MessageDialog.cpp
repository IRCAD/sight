/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGui/dialog/MessageDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

#include <functional>

namespace fwGui
{
namespace dialog
{

//-----------------------------------------------------------------------------

IMessageDialog::Buttons MessageDialog::showMessageDialog(
    const std::string& title, const std::string& message, ::fwGui::dialog::IMessageDialog::Icons icon)
{
    ::fwGui::dialog::MessageDialog messageBox(title, message, icon);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    return messageBox.show();
}

//-----------------------------------------------------------------------------

MessageDialog::MessageDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IMessageDialog::REGISTRY_KEY);
                m_implementation                    = ::fwGui::dialog::IMessageDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

MessageDialog::MessageDialog(
    const std::string& title, const std::string& message, ::fwGui::dialog::IMessageDialog::Icons icon)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IMessageDialog::REGISTRY_KEY);
                m_implementation                    = ::fwGui::dialog::IMessageDialog::dynamicCast(guiObj);

                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                    m_implementation->setMessage(message);
                    m_implementation->setIcon(icon);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{
}

//-----------------------------------------------------------------------------

void MessageDialog::setTitle( const std::string& title )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setMessage( const std::string& msg )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setMessage(msg);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setIcon( ::fwGui::dialog::IMessageDialog::Icons icon )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setIcon(icon);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::addButton( ::fwGui::dialog::IMessageDialog::Buttons button )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->addButton(button);
                }
            } )).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton( ::fwGui::dialog::IMessageDialog::Buttons button )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setDefaultButton(button);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( std::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->addCustomButton(label, clickedFn);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

::fwGui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    if(m_implementation)
    {
        typedef ::fwGui::dialog::IMessageDialog::Buttons R;

        std::function<R()> func = std::bind(&IMessageDialog::show, m_implementation);
        std::shared_future<R> f = ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<R>(func);
        f.wait();

        return f.get();
    }
    else
    {
        return ::fwGui::dialog::IMessageDialog::NOBUTTON;
    }
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui
