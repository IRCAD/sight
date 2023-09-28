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

#include "MenuBar.hpp"

#include "ui/__/registry.hpp"

#include <service/op/Get.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

MenuBar::MenuBar(std::string sid) :
    m_sid(std::move(sid))
{
}

//-----------------------------------------------------------------------------

ui::container::menubar::sptr MenuBar::getParent()
{
    return ui::registry::getSIDMenuBar(m_sid);
}

//-----------------------------------------------------------------------------

ui::container::menu::sptr MenuBar::getFwMenu(
    std::string menuSid,
    std::vector<ui::container::menu::sptr> menus
)
{
    SIGHT_ASSERT(
        "The menu '" + menuSid + "'declared in '" + m_sid + "' is not found",
        m_menuSids.find(menuSid) != m_menuSids.end()
    );
    ui::container::menu::sptr menu = menus.at(m_menuSids[menuSid].first);
    return menu;
}

//-----------------------------------------------------------------------------

void MenuBar::initialize(const ui::config_t& configuration)
{
    // index represents associated menu with position in menus vector
    unsigned int index = 0;

    // initialize m_menuSids map with configuration
    for(const auto& menu : boost::make_iterator_range(configuration.equal_range("menu")))
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

void MenuBar::manage(std::vector<ui::container::menu::sptr> menus)
{
    ui::container::menu::sptr menu;
    for(const SIDMenuMapType::value_type& sid : m_menuSids)
    {
        SIGHT_ASSERT(
            "The menuBar '" << m_sid << "' contains more menus in <registry> than in <layout>: "
            << (sid.second.first + 1) << " menus in <registry>, but only " << menus.size() << " in <layout>.",
            sid.second.first < menus.size()
        );
        menu = menus.at(sid.second.first);
        ui::registry::registerSIDMenu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "Menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menuBar.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->start();
        }
        else
        {
            bool service_exists = core::tools::id::exist(sid.first);
            if(!service_exists || service::get(sid.first)->isStopped())
            {
                ui::registry::actionServiceStopping(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void MenuBar::unmanage()
{
    for(const SIDMenuMapType::value_type& sid : m_menuSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "Menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menuBar.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregisterSIDMenu(sid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
