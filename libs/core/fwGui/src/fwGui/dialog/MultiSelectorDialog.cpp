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

#include "fwGui/dialog/MultiSelectorDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< void >(std::function< void() >(
                                                                                    [this] {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IMultiSelectorDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IMultiSelectorDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setTitle(std::string title)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->setTitle(title);
            }).wait();
}

//-----------------------------------------------------------------------------

IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    typedef IMultiSelectorDialog::Selections R;
    std::function< R() > func = std::bind( &IMultiSelectorDialog::show, m_implementation);
    std::shared_future< R > f = ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< R  >(func);

    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setSelections(Selections _selections)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->setSelections( _selections );
            }).wait();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string& msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->setMessage( msg );
            }).wait();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
