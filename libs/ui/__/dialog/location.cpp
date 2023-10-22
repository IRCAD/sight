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
            ui::object::sptr gui_obj = ui::factory::make(location_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::location_base>(gui_obj);
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

void location::setType(location::Types _type)
{
    m_implementation->setType(_type);
}

//-----------------------------------------------------------------------------

void location::addFilter(const std::string& _filter_name, const std::string& _wildcard_list)
{
    m_implementation->addFilter(_filter_name, _wildcard_list);
}

//-----------------------------------------------------------------------------

void location::setOption(location::Options _option)
{
    m_implementation->setOption(_option);
}

//-----------------------------------------------------------------------------

void location::setTitle(const std::string& _title)
{
    m_implementation->setTitle(_title);
}

//-----------------------------------------------------------------------------

const std::string& location::getTitle()
{
    return m_implementation->getTitle();
}

//-----------------------------------------------------------------------------

void location::setDefaultLocation(core::location::base::sptr _loc)
{
    m_implementation->setDefaultLocation(_loc);
}

//-----------------------------------------------------------------------------

core::location::base::sptr location::getDefaultLocation()
{
    return m_implementation->getDefaultLocation();
}

//-----------------------------------------------------------------------------

void location::saveDefaultLocation(core::location::base::sptr _loc)
{
    m_implementation->saveDefaultLocation(_loc);
}

//-----------------------------------------------------------------------------

std::string location::getCurrentSelection() const
{
    return m_implementation->getCurrentSelection();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
