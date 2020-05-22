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

#include "fwGui/dialog/SelectorDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

namespace fwGui
{
namespace dialog
{

//-----------------------------------------------------------------------------

std::string SelectorDialog::showSelectorDialog(const std::string& title, const std::string& message,
                                               std::vector< std::string > _selections)
{
    ::fwGui::dialog::SelectorDialog selector(title, message, _selections);
    return selector.show();
}

//-----------------------------------------------------------------------------

SelectorDialog::SelectorDialog(const std::string& title, const std::string& message,
                               std::vector< std::string > _selections)
{
    create();
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
            {
                m_implementation->setTitle(title);
                m_implementation->setMessage( message );
                m_implementation->setSelections( _selections );
            }).wait();
}

//-----------------------------------------------------------------------------

SelectorDialog::SelectorDialog()
{
    create();
}

//------------------------------------------------------------------------------

void SelectorDialog::create()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ISelectorDialog::REGISTRY_KEY);
                m_implementation                    = ::fwGui::dialog::ISelectorDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

void SelectorDialog::setTitle(std::string title)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
            {
                m_implementation->setTitle(title);
            })).wait();
}

//-----------------------------------------------------------------------------

std::string SelectorDialog::show()
{
    std::function< std::string() > f         = std::bind(&ISelectorDialog::show, m_implementation);
    std::shared_future< std::string > future =
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< std::string >(f);
    future.wait();
    return future.get();
}

//-----------------------------------------------------------------------------

void SelectorDialog::setSelections(std::vector< std::string > _selections)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_implementation->setSelections( _selections );
            })).wait();
}

//-----------------------------------------------------------------------------

void SelectorDialog::setMessage(const std::string& msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_implementation->setMessage( msg );
            })).wait();
}

//------------------------------------------------------------------------------

void SelectorDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_implementation->addCustomButton( label, clickedFn );
            })).wait();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
