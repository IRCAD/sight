/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "menu_bar.hpp"

#include "ui/__/registry.hpp"

#include <service/op.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

menu_bar::menu_bar(std::string _sid) :
    m_sid(std::move(_sid))
{
}

//-----------------------------------------------------------------------------

ui::container::menubar::sptr menu_bar::getParent()
{
    return ui::registry::get_sid_menu_bar(m_sid);
}

//-----------------------------------------------------------------------------

ui::container::menu::sptr menu_bar::getFwMenu(
    std::string _menu_sid,
    std::vector<ui::container::menu::sptr> _menus
)
{
    SIGHT_ASSERT(
        "The menu '" + _menu_sid + "'declared in '" + m_sid + "' is not found",
        m_menuSids.find(_menu_sid) != m_menuSids.end()
    );
    ui::container::menu::sptr menu = _menus.at(m_menuSids[_menu_sid].first);
    return menu;
}

//-----------------------------------------------------------------------------

void menu_bar::initialize(const ui::config_t& _configuration)
{
    // index represents associated menu with position in menus vector
    unsigned int index = 0;

    // initialize m_menuSids map with configuration
    for(const auto& menu : boost::make_iterator_range(_configuration.equal_range("menu")))
    {
        if(const auto sid = menu.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
        {
            const bool start = menu.second.get("<xmlattr>.start", false);
            m_menuSids[sid.value()] = SIDMenuMapType::mapped_type(index, start);
        }

        index++;
    }
}

//-----------------------------------------------------------------------------

void menu_bar::manage(std::vector<ui::container::menu::sptr> _menus)
{
    ui::container::menu::sptr menu;
    for(const SIDMenuMapType::value_type& sid : m_menuSids)
    {
        SIGHT_ASSERT(
            "The menuBar '" << m_sid << "' contains more menus in <registry> than in <layout>: "
            << (sid.second.first + 1) << " menus in <registry>, but only " << _menus.size() << " in <layout>.",
            sid.second.first < _menus.size()
        );
        menu = _menus.at(sid.second.first);
        ui::registry::register_sid_menu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menuBar.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->start();
        }
        else
        {
            bool service_exists = core::tools::id::exist(sid.first);
            if(!service_exists || service::get(sid.first)->stopped())
            {
                ui::registry::action_service_stopping(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void menu_bar::unmanage()
{
    for(const SIDMenuMapType::value_type& sid : m_menuSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menuBar.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregister_sid_menu(sid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
