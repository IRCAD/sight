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

#include "ui/base/layoutManager/IToolBarLayoutManager.hpp"

#include <core/runtime/operations.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::base::layoutManager
{

const IToolBarLayoutManager::RegistryKeyType IToolBarLayoutManager::REGISTRY_KEY =
    "sight::ui::base::ToolBarLayoutManager";

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::initialize(const ui::base::config_t& configuration)
{
    if(const auto style = configuration.get_optional<std::string>("<xmlattr>.style"); style.has_value())
    {
        if(*style == "ToolButtonIconOnly" || *style == "ToolButtonTextOnly" || *style == "ToolButtonTextBesideIcon"
           || *style == "ToolButtonTextUnderIcon" || *style == "ToolButtonFollowStyle")
        {
            m_style = *style;
        }
        else
        {
            SIGHT_ERROR(
                "`Style` attribute value must be `ToolButtonIconOnly` "
                "or `ToolButtonTextOnly` or `ToolButtonTextBesideIcon` "
                "or `ToolButtonTextUnderIcon` or `ToolButtonFollowStyle`"
            );
        }
    }

    m_unifyButtonSize = configuration.get<bool>("<xmlattr>.uniformSize", m_unifyButtonSize);

    for(const auto& toolBarItem : configuration)
    {
        if(toolBarItem.first == "menuItem")
        {
            ActionInfo info;

            info.m_name     = toolBarItem.second.get<std::string>("<xmlattr>.name");
            info.m_shortcut = toolBarItem.second.get<std::string>("<xmlattr>.shortcut", info.m_shortcut);

            const auto icon = toolBarItem.second.get<std::string>("<xmlattr>.icon", "");
            if(!icon.empty())
            {
                info.m_icon = core::runtime::getModuleResourceFilePath(icon);
            }

            const auto icon2 = toolBarItem.second.get<std::string>("<xmlattr>.icon2", "");
            if(!icon2.empty())
            {
                SIGHT_ASSERT("'icon' attribute must be defined before 'icon2'", !info.m_icon.empty());
                info.m_icon2 = core::runtime::getModuleResourceFilePath(icon2);
            }

            if(const auto style = toolBarItem.second.get_optional<std::string>("<xmlattr>.style"); style.has_value())
            {
                info.m_isCheckable = (*style == "check");
                info.m_isRadio     = (*style == "radio");
            }

            m_actionInfo.push_back(info);
        }
        else if(toolBarItem.first == "separator")
        {
            ActionInfo info;
            info.m_isSeparator = true;
            info.m_size        = toolBarItem.second.get<int>("<xmlattr>.size", info.m_size);
            m_actionInfo.push_back(info);
        }
        else if(toolBarItem.first == "spacer")
        {
            ActionInfo info;
            info.m_isSpacer = true;
            m_actionInfo.push_back(info);
        }
        else if(toolBarItem.first == "menu")
        {
            ActionInfo info;

            info.m_isMenu = true;
            info.m_name   = toolBarItem.second.get<std::string>("<xmlattr>.name", "");

            const auto icon = toolBarItem.second.get<std::string>("<xmlattr>.icon", "");
            if(!icon.empty())
            {
                info.m_icon = core::runtime::getModuleResourceFilePath(icon);
            }

            m_actionInfo.push_back(info);
        }
        else if(toolBarItem.first == "editor")
        {
            ActionInfo info;
            info.m_isEditor = true;

            m_actionInfo.push_back(info);
        }
    }
}

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::destroyActions()
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
    for(const ui::base::container::fwContainer::sptr& container : m_containers)
    {
        container->destroyContainer();
    }

    m_containers.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::base::container::fwMenuItem::sptr> IToolBarLayoutManager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

std::vector<ui::base::container::fwMenu::sptr> IToolBarLayoutManager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

std::vector<ui::base::container::fwContainer::sptr> IToolBarLayoutManager::getContainers()
{
    return this->m_containers;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::layoutManager
