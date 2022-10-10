/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/layoutManager/IMenuLayoutManager.hpp"

#include <core/runtime/operations.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::base::layoutManager
{

const IMenuLayoutManager::RegistryKeyType IMenuLayoutManager::REGISTRY_KEY = "sight::ui::base::MenuLayoutManager";

//-----------------------------------------------------------------------------

void IMenuLayoutManager::initialize(const ui::base::config_t& configuration)
{
    for(const auto& menuItem : configuration)
    {
        if(menuItem.first == "menuItem")
        {
            ActionInfo info;

            info.m_name     = menuItem.second.get<std::string>("<xmlattr>.name");
            info.m_shortcut = menuItem.second.get<std::string>("<xmlattr>.shortcut", info.m_shortcut);
            const auto icon = menuItem.second.get<std::string>("<xmlattr>.icon", "");
            if(!icon.empty())
            {
                info.m_icon = core::runtime::getModuleResourceFilePath(icon);
            }

            if(const auto style = menuItem.second.get_optional<std::string>("<xmlattr>.style"); style.has_value())
            {
                info.m_isCheckable = (*style == "check");
                info.m_isRadio     = (*style == "radio");
            }

            if(const auto action =
                   menuItem.second.get_optional<std::string>("<xmlattr>.specialAction"); action.has_value())
            {
                const std::string& specialActionName = action.value();
                if(specialActionName == "DEFAULT")
                {
                    info.m_type = DEFAULT;
                }
                else if(specialActionName == "QUIT")
                {
                    info.m_type = QUIT;
                }
                else if(specialActionName == "ABOUT")
                {
                    info.m_type = ABOUT;
                }
                else if(specialActionName == "HELP")
                {
                    info.m_type = HELP;
                }
                else if(specialActionName == "NEW")
                {
                    info.m_type = NEW;
                }
                else
                {
                    SIGHT_FATAL("specialAction " << specialActionName << " is unknown.");
                }
            }

            m_actionInfo.push_back(info);
        }

        if(menuItem.first == "separator")
        {
            ActionInfo info;
            info.m_isSeparator = true;
            info.m_type        = SEPARATOR;
            m_actionInfo.push_back(info);
        }

        if(menuItem.first == "menu")
        {
            ActionInfo info;
            info.m_isMenu = true;
            info.m_name   = menuItem.second.get<std::string>("<xmlattr>.name", "");

            m_actionInfo.push_back(info);
        }
    }
}

//-----------------------------------------------------------------------------

void IMenuLayoutManager::destroyActions()
{
    for(const ui::base::container::fwMenuItem::sptr& menuItem : m_menuItems)
    {
        menuItem->destroyContainer();
    }

    m_menuItems.clear();
    for(const ui::base::container::fwMenu::sptr& menu : m_menus)
    {
        menu->destroyContainer();
    }

    m_menus.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::base::container::fwMenuItem::sptr> IMenuLayoutManager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

std::vector<ui::base::container::fwMenu::sptr> IMenuLayoutManager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::layoutManager
