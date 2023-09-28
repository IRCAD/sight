/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/dialog/location.hpp"

#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

location::location()
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            ui::object::sptr guiObj = ui::factory::make(location_base::REGISTRY_KEY);
            m_implementation        = std::dynamic_pointer_cast<ui::dialog::location_base>(guiObj);
        })
    ).wait();
}

//------------------------------------------------------------------------------

location::~location()
= default;

//-----------------------------------------------------------------------------

core::location::base::sptr location::show()
{
    using R = std::shared_ptr<core::location::base>;

    std::function<R()> func = [this](auto&& ...){return m_implementation->show();};
    std::shared_future<R> f = core::thread::get_default_worker()->post_task<R>(func);

    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

void location::setType(location::Types type)
{
    m_implementation->setType(type);
}

//-----------------------------------------------------------------------------

void location::addFilter(const std::string& filterName, const std::string& wildcardList)
{
    m_implementation->addFilter(filterName, wildcardList);
}

//-----------------------------------------------------------------------------

void location::setOption(location::Options option)
{
    m_implementation->setOption(option);
}

//-----------------------------------------------------------------------------

void location::setTitle(const std::string& title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

const std::string& location::getTitle()
{
    return m_implementation->getTitle();
}

//-----------------------------------------------------------------------------

void location::setDefaultLocation(core::location::base::sptr loc)
{
    m_implementation->setDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

core::location::base::sptr location::getDefaultLocation()
{
    return m_implementation->getDefaultLocation();
}

//-----------------------------------------------------------------------------

void location::saveDefaultLocation(core::location::base::sptr loc)
{
    m_implementation->saveDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

std::string location::getCurrentSelection() const
{
    return m_implementation->getCurrentSelection();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
