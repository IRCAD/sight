/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#include "fwGui/dialog/InputDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

std::string InputDialog::showInputDialog(const std::string& title, const std::string& message, const std::string& text)
{
    ::fwGui::dialog::InputDialog inputBox(title, message, text);
    return inputBox.getInput();
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IInputDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IInputDialog::dynamicCast(guiObj);
            }));
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog(const std::string& title, const std::string& message, const std::string& text)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IInputDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IInputDialog::dynamicCast(guiObj);
                m_implementation->setTitle(title);
                m_implementation->setMessage(message);
                m_implementation->setInput(text);
            }));
}

//-----------------------------------------------------------------------------

InputDialog::~InputDialog()
{
}

//-----------------------------------------------------------------------------

void InputDialog::setTitle( const std::string& title )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_implementation->setTitle(title);
            })).wait();
}

//-----------------------------------------------------------------------------

void InputDialog::setMessage( const std::string& msg )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
            {
                m_implementation->setMessage(msg);
            })).wait();
}

//-----------------------------------------------------------------------------

void InputDialog::setInput(const std::string& text)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_implementation->setInput(text);
            })).wait();
}

//-----------------------------------------------------------------------------

std::string InputDialog::getInput()
{
    std::function< std::string() > func = std::bind(&IInputDialog::getInput, m_implementation);
    std::shared_future< std::string > f =
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<std::string>(func);
    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui

