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
    using ret_t = std::shared_ptr<core::location::base>;

    std::function<ret_t()> func = [this](auto&& ...){return m_implementation->show();};
    std::shared_future<ret_t> f = core::thread::get_default_worker()->post_task<ret_t>(func);

    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

void location::set_type(location::types _type)
{
    m_implementation->set_type(_type);
}

//-----------------------------------------------------------------------------

void location::add_filter(const std::string& _filter_name, const std::string& _wildcard_list)
{
    m_implementation->add_filter(_filter_name, _wildcard_list);
}

//-----------------------------------------------------------------------------

void location::set_option(location::options _option)
{
    m_implementation->set_option(_option);
}

//-----------------------------------------------------------------------------

void location::set_title(const std::string& _title)
{
    m_implementation->set_title(_title);
}

//-----------------------------------------------------------------------------

const std::string& location::get_title()
{
    return m_implementation->get_title();
}

//-----------------------------------------------------------------------------

void location::set_default_location(core::location::base::sptr _loc)
{
    m_implementation->set_default_location(_loc);
}

//-----------------------------------------------------------------------------

core::location::base::sptr location::get_default_location()
{
    return m_implementation->get_default_location();
}

//-----------------------------------------------------------------------------

void location::save_default_location(core::location::base::sptr _loc)
{
    m_implementation->save_default_location(_loc);
}

//-----------------------------------------------------------------------------

std::string location::get_current_selection() const
{
    return m_implementation->get_current_selection();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
