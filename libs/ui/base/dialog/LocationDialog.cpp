/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ui/base/dialog/LocationDialog.hpp"

#include <core/thread/ActiveWorkers.hpp>

namespace sight::ui::base
{

namespace dialog
{

//-----------------------------------------------------------------------------

LocationDialog::LocationDialog()
{
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
            {
                ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ILocationDialog::REGISTRY_KEY);
                m_implementation                     = ui::base::dialog::ILocationDialog::dynamicCast(guiObj);
            })
    ).wait();
}

//------------------------------------------------------------------------------

LocationDialog::~LocationDialog()
{
}

//-----------------------------------------------------------------------------

core::location::ILocation::sptr LocationDialog::show()
{
    typedef SPTR(core::location::ILocation) R;

    std::function<R()> func = std::bind(&ILocationDialog::show, m_implementation);
    std::shared_future<R> f = core::thread::ActiveWorkers::getDefaultWorker()->postTask<R>(func);

    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

void LocationDialog::setType(ui::base::dialog::ILocationDialog::Types type)
{
    m_implementation->setType(type);
}

//-----------------------------------------------------------------------------

void LocationDialog::addFilter(const std::string& filterName, const std::string& wildcardList)
{
    m_implementation->addFilter(filterName, wildcardList);
}

//-----------------------------------------------------------------------------

ILocationDialog& LocationDialog::setOption(ui::base::dialog::ILocationDialog::Options option)
{
    return m_implementation->setOption(option);
}

//-----------------------------------------------------------------------------

void LocationDialog::setTitle(const std::string& title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

const std::string& LocationDialog::getTitle()
{
    return m_implementation->getTitle();
}

//-----------------------------------------------------------------------------

void LocationDialog::setDefaultLocation(core::location::ILocation::sptr loc)
{
    m_implementation->setDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

const core::location::ILocation::sptr LocationDialog::getDefaultLocation()
{
    return m_implementation->getDefaultLocation();
}

//-----------------------------------------------------------------------------

void LocationDialog::saveDefaultLocation(core::location::ILocation::sptr loc)
{
    m_implementation->saveDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

std::string LocationDialog::getCurrentSelection() const
{
    return m_implementation->getCurrentSelection();
}

//-----------------------------------------------------------------------------

} //namespace dialog

} //namespace sight::ui::base
