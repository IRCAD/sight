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
        if(menu_item.first == "menuItem")
        {
            ActionInfo info;

            info.m_name     = menu_item.second.get<std::string>("<xmlattr>.name");
            info.m_shortcut = menu_item.second.get<std::string>("<xmlattr>.shortcut", info.m_shortcut);
            const auto icon = menu_item.second.get<std::string>("<xmlattr>.icon", "");
            if(!icon.empty())
            {
                info.m_icon = core::runtime::get_module_resource_file_path(icon);
            }

            if(const auto style = menu_item.second.get_optional<std::string>("<xmlattr>.style"); style.has_value())
            {
                info.m_isCheckable = (*style == "check");
                info.m_isRadio     = (*style == "radio");
            }

            if(const auto action =
                   menu_item.second.get_optional<std::string>("<xmlattr>.specialAction"); action.has_value())
            {
                const std::string& special_action_name = action.value();
                if(special_action_name == "DEFAULT")
                {
                    info.m_type = DEFAULT;
                }
                else if(special_action_name == "QUIT")
                {
                    info.m_type = QUIT;
                }
                else if(special_action_name == "ABOUT")
                {
                    info.m_type = ABOUT;
                }
                else if(special_action_name == "HELP")
                {
                    info.m_type = HELP;
                }
                else if(special_action_name == "NEW")
                {
                    info.m_type = NEW;
                }
                else
                {
                    SIGHT_FATAL("specialAction " << special_action_name << " is unknown.");
                }
            }

            m_actionInfo.push_back(info);
        }

        if(menu_item.first == "separator")
        {
            ActionInfo info;
            info.m_isSeparator = true;
            info.m_type        = SEPARATOR;
            m_actionInfo.push_back(info);
        }

        if(menu_item.first == "menu")
        {
            ActionInfo info;
            info.m_isMenu = true;
            info.m_name   = menu_item.second.get<std::string>("<xmlattr>.name", "");

            m_actionInfo.push_back(info);
        }
    }
}

//-----------------------------------------------------------------------------

void menu_manager::destroyActions()
{
    for(const ui::container::menu_item::sptr& menu_item : m_menuItems)
    {
        menu_item->destroyContainer();
    }

    m_menuItems.clear();
    for(const ui::container::menu::sptr& menu : m_menus)
    {
        menu->destroyContainer();
    }

    m_menus.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::container::menu_item::sptr> menu_manager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

std::vector<ui::container::menu::sptr> menu_manager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
