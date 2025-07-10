/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/__/layout/menu_manager.hpp"

#include <core/runtime/path.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::layout
{

const menu_manager::registry_key_t menu_manager::REGISTRY_KEY = "sight::ui::layout::menu";

//-----------------------------------------------------------------------------

void menu_manager::initialize(const ui::config_t& _configuration)
{
    for(const auto& menu_item : _configuration)
    {
        if(menu_item.first == "menu_item" or menu_item.first == "menuItem")
        {
            action_info info;

            info.m_name     = menu_item.second.get<std::string>("<xmlattr>.name");
            info.m_shortcut = menu_item.second.get<std::string>("<xmlattr>.shortcut", info.m_shortcut);
            const auto icon = menu_item.second.get<std::string>("<xmlattr>.icon", "");
            if(!icon.empty())
            {
                info.m_icon = core::runtime::get_module_resource_file_path(icon);
            }

            if(const auto style = menu_item.second.get_optional<std::string>("<xmlattr>.style"); style.has_value())
            {
                info.m_is_checkable = (*style == "check");
                info.m_is_radio     = (*style == "radio");
            }

            if(const auto action =
                   menu_item.second.get_optional<std::string>("<xmlattr>.special_action"); action.has_value())
            {
                const std::string& special_action_name = action.value();
                if(special_action_name == "DEFAULT")
                {
                    info.m_type = DEFAULT;
                }
                else if(special_action_name == "QUIT")
                {
                    info.m_type = quit;
                }
                else if(special_action_name == "ABOUT")
                {
                    info.m_type = about;
                }
                else if(special_action_name == "HELP")
                {
                    info.m_type = help;
                }
                else if(special_action_name == "NEW")
                {
                    info.m_type = NEW;
                }
                else
                {
                    SIGHT_FATAL("special_action " << special_action_name << " is unknown.");
                }
            }

            m_action_info.push_back(info);
        }

        if(menu_item.first == "separator")
        {
            action_info info;
            info.m_is_separator = true;
            info.m_type         = separator;
            m_action_info.push_back(info);
        }

        if(menu_item.first == "menu")
        {
            action_info info;
            info.m_is_menu = true;
            info.m_name    = menu_item.second.get<std::string>("<xmlattr>.name", "");

            m_action_info.push_back(info);
        }
    }
}

//-----------------------------------------------------------------------------

void menu_manager::destroy_actions()
{
    for(const ui::container::menu_item::sptr& menu_item : m_menu_items)
    {
        menu_item->destroy_container();
    }

    m_menu_items.clear();
    for(const ui::container::menu::sptr& menu : m_menus)
    {
        menu->destroy_container();
    }

    m_menus.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::container::menu_item::sptr> menu_manager::get_menu_items()
{
    return this->m_menu_items;
}

//-----------------------------------------------------------------------------

std::vector<ui::container::menu::sptr> menu_manager::get_menus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
