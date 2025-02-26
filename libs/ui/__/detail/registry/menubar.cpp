/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "menubar.hpp"

#include "ui/__/registry.hpp"

#include <service/op.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

menubar::menubar(std::string _sid) :
    m_sid(std::move(_sid))
{
}

//-----------------------------------------------------------------------------

ui::container::menubar::sptr menubar::get_parent()
{
    return ui::registry::get_sid_menu_bar(m_sid);
}

//-----------------------------------------------------------------------------

ui::container::menu::sptr menubar::get_fw_menu(
    std::string _menu_sid,
    std::vector<ui::container::menu::sptr> _menus
)
{
    SIGHT_ASSERT(
        "The menu '" + _menu_sid + "'declared in '" + m_sid + "' is not found",
        m_menu_sids.find(_menu_sid) != m_menu_sids.end()
    );
    ui::container::menu::sptr menu = _menus.at(m_menu_sids[_menu_sid].first);
    return menu;
}

//-----------------------------------------------------------------------------

void menubar::initialize(const ui::config_t& _configuration)
{
    // index represents associated menu with position in menus vector
    unsigned int index = 0;

    // initialize m_menuSids map with configuration
    for(const auto& menu : boost::make_iterator_range(_configuration.equal_range("menu")))
    {
        if(const auto sid = menu.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
        {
            const bool start = menu.second.get("<xmlattr>.start", false);
            m_menu_sids[sid.value()] = sid_menu_map_t::mapped_type(index, start);
        }

        index++;
    }
}

//-----------------------------------------------------------------------------

void menubar::manage(std::vector<ui::container::menu::sptr> _menus)
{
    ui::container::menu::sptr menu;
    for(const auto& sid : m_menu_sids)
    {
        SIGHT_ASSERT(
            "The menubar '" << m_sid << "' contains more menus in <registry> than in <layout>: "
            << (sid.second.first + 1) << " menus in <registry>, but only " << _menus.size() << " in <layout>.",
            sid.second.first < _menus.size()
        );
        menu = _menus.at(sid.second.first);
        ui::registry::register_sid_menu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menubar.",
                core::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->start();
        }
        else
        {
            bool service_exists = core::id::exist(sid.first);
            if(!service_exists || service::get(sid.first)->stopped())
            {
                ui::registry::action_service_stopping(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void menubar::unmanage()
{
    for(const auto& sid : m_menu_sids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menubar.",
                core::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregister_sid_menu(sid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
