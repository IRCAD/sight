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

#include "menu.hpp"

#include "ui/__/action.hpp"
#include "ui/__/registry.hpp"

#include <service/op.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

menu::menu(std::string _sid) :
    m_sid(std::move(_sid))
{
}

//-----------------------------------------------------------------------------

ui::container::menu::sptr menu::get_parent()
{
    return ui::registry::get_sid_menu(m_sid);
}

//-----------------------------------------------------------------------------

ui::container::menu_item::sptr menu::get_menu_item(
    std::string _action_sid,
    std::vector<ui::container::menu_item::sptr> _menu_items
)
{
    SIGHT_ASSERT(
        "The action '" + _action_sid + "' declared by the menu '" + m_sid + "' is not found",
        m_action_sids.find(_action_sid) != m_action_sids.end()
    );
    ui::container::menu_item::sptr menu_item = _menu_items.at(m_action_sids[_action_sid].first);
    return menu_item;
}

//-----------------------------------------------------------------------------

void menu::initialize(const ui::config_t& _configuration)
{
    // index represents associated menu with position in menus vector
    unsigned int index = 0;
    m_callbacks.clear();

    // initialize m_actionSids map with configuration
    for(const auto& menu_item : boost::make_iterator_range(_configuration.equal_range("menuItem")))
    {
        if(const auto sid = menu_item.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
        {
            const bool start = menu_item.second.get("<xmlattr>.start", false);

            SIGHT_ASSERT(
                "The action '" + sid.value() + "' already exists for '" + m_sid + "' menu.",
                m_action_sids.find(sid.value()) == m_action_sids.end()
            );
            m_action_sids[sid.value()] = sid_menu_map_t::mapped_type(index, start);

            ui::action_callback_base::sptr callback;
            ui::object::sptr gui_obj = ui::factory::make(action_callback_base::REGISTRY_KEY);
            callback = std::dynamic_pointer_cast<ui::action_callback_base>(gui_obj);
            SIGHT_ASSERT(
                "Cannot create action callback, factory failed for '"
                + ui::action_callback_base::REGISTRY_KEY + "'",
                callback
            );

            callback->set_sid(sid.value());
            m_callbacks.push_back(callback);
        }

        index++;
    }

    index = 0;
    // initialize m_actionSids map with configuration
    for(const auto& menu : boost::make_iterator_range(_configuration.equal_range("menu")))
    {
        if(const auto sid = menu.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
        {
            const bool start = menu.second.get("<xmlattr>.start", false);
            SIGHT_ASSERT(
                "The menu '" + sid.value() + "' already exists for this menu '" + m_sid + "'",
                m_menu_sids.find(sid.value()) == m_menu_sids.end()
            );
            m_menu_sids[sid.value()] = sid_menu_map_t::mapped_type(index, start);
        }

        index++;
    }
}

//-----------------------------------------------------------------------------

void menu::manage(std::vector<ui::container::menu_item::sptr> _menu_items)
{
    ui::container::menu_item::sptr menu_item;
    for(const auto& sid : m_action_sids)
    {
        SIGHT_ASSERT(
            "The menu '" << m_sid << "' contains more menuItems in <registry> than in <layout>: "
            << (sid.second.first + 1) << " menuItems in <registry>, but only " << _menu_items.size() << " in <layout>.",
            sid.second.first < _menu_items.size()
        );
        menu_item = _menu_items.at(sid.second.first);
        ui::registry::register_action_sid_to_parent_sid(sid.first, m_sid);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The action '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menu, "
                                                                                                "the service may be created later if it uses deferred objects, thus use start=\"no\" and start it at the end of the configuration ",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            SIGHT_ASSERT(
                "The service '" + sid.first + "' cannot be started by '" + m_sid + "' because it is not stopped."
                ,
                service->stopped()
            );
            service->start();
        }
        else
        {
            bool service_exists = core::tools::id::exist(sid.first);
            if(!service_exists || service::get(sid.first)->stopped())
            {
                ui::registry::action_service_stopping(sid.first);
            }
            else
            {
                ui::registry::action_service_starting(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void menu::manage(std::vector<ui::container::menu::sptr> _menus)
{
    ui::container::menu::sptr menu;
    for(const auto& sid : m_menu_sids)
    {
        SIGHT_ASSERT(
            "The menu '" << m_sid << "' contains more menus in <registry> than in <layout>: "
            << sid.second.first << " menus in <registry>, but only " << _menus.size() << " in <layout>.",
            (sid.second.first + 1) < _menus.size()
        );
        menu = _menus.at(sid.second.first);
        ui::registry::register_sid_menu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menu.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            SIGHT_ASSERT(
                "The service '" + sid.first + "' cannot be started by '" + m_sid + "' because it is not stopped."
                ,
                service->stopped()
            );
            service->start();
        }
    }
}

//-----------------------------------------------------------------------------

void menu::unmanage()
{
    for(const auto& sid : m_action_sids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menu '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                   "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregister_action_sid_to_parent_sid(sid.first, m_sid);
    }

    for(const auto& sid : m_menu_sids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menu '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                   "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregister_sid_menu(sid.first);
    }
}

//-----------------------------------------------------------------------------

void menu::on_item_action()
{
    SIGHT_WARN("TODO: menu::onItemAction not yet implemented");
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
