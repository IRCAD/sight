/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <boost/lexical_cast.hpp>

namespace sight::ui::base
{

namespace layoutManager
{

const IToolBarLayoutManager::RegistryKeyType IToolBarLayoutManager::REGISTRY_KEY =
    "sight::ui::base::ToolBarLayoutManager";

//-----------------------------------------------------------------------------

IToolBarLayoutManager::IToolBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

IToolBarLayoutManager::~IToolBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::initialize(ConfigurationType configuration)
{
    SIGHT_ASSERT(
        "Bad configuration name " << configuration->getName() << ", must be layout",
        configuration->getName() == "layout"
    );

    if(configuration->hasAttribute("style"))
    {
        const std::string style = configuration->getAttributeValue("style");
        if(style == "ToolButtonIconOnly" || style == "ToolButtonTextOnly" || style == "ToolButtonTextBesideIcon"
           || style == "ToolButtonTextUnderIcon" || style == "ToolButtonFollowStyle")
        {
            m_style = style;
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

    if(configuration->hasAttribute("uniformSize"))
    {
        const auto value = configuration->getAttributeValue("uniformSize");

        if(value == "true")
        {
            m_unifyButtonSize = true;
        }
        else if(value == "false")
        {
            m_unifyButtonSize = false;
        }
        else
        {
            SIGHT_ERROR("'uniformSize' attribute value should be 'true' of 'false'.");
        }
    }

    core::runtime::ConfigurationElementContainer::Iterator iter;
    for(iter = configuration->begin() ; iter != configuration->end() ; ++iter)
    {
        if((*iter)->getName() == "menuItem")
        {
            ConfigurationType toolBarItem = *iter;
            ActionInfo info;
            SIGHT_ASSERT("Depreciated tag <state>", !toolBarItem->hasAttribute("state"));
            SIGHT_ASSERT("Depreciated tag <enable>", !toolBarItem->hasAttribute("enable"));

            SIGHT_ASSERT("missing <name> attribute", toolBarItem->hasAttribute("name"));
            if(toolBarItem->hasAttribute("name"))
            {
                info.m_name = toolBarItem->getExistingAttributeValue("name");
            }

            if(toolBarItem->hasAttribute("icon"))
            {
                info.m_icon = core::runtime::getModuleResourceFilePath(toolBarItem->getAttributeValue("icon"));
            }

            if(toolBarItem->hasAttribute("icon2"))
            {
                SIGHT_ASSERT("'icon' attribute must be defined before 'icon2'", !info.m_icon.empty());
                info.m_icon2 = core::runtime::getModuleResourceFilePath(toolBarItem->getAttributeValue("icon2"));
            }

            if(toolBarItem->hasAttribute("style"))
            {
                std::string style = toolBarItem->getExistingAttributeValue("style");
                info.m_isCheckable = (style == "check");
                info.m_isRadio     = (style == "radio");
            }

            if(toolBarItem->hasAttribute("shortcut"))
            {
                info.m_shortcut = toolBarItem->getExistingAttributeValue("shortcut");
            }

            m_actionInfo.push_back(info);
        }
        else if((*iter)->getName() == "separator")
        {
            ActionInfo info;
            info.m_isSeparator = true;

            if((*iter)->hasAttribute("size"))
            {
                info.m_size = ::boost::lexical_cast<int>((*iter)->getExistingAttributeValue("size"));
            }

            m_actionInfo.push_back(info);
        }
        else if((*iter)->getName() == "spacer")
        {
            ActionInfo info;
            info.m_isSpacer = true;
            m_actionInfo.push_back(info);
        }
        else if((*iter)->getName() == "menu")
        {
            ActionInfo info;
            info.m_isMenu = true;
            if((*iter)->hasAttribute("name"))
            {
                info.m_name = (*iter)->getExistingAttributeValue("name");
            }

            if((*iter)->hasAttribute("icon"))
            {
                info.m_icon = core::runtime::getModuleResourceFilePath((*iter)->getExistingAttributeValue("icon"));
            }

            m_actionInfo.push_back(info);
        }
        else if((*iter)->getName() == "editor")
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
    for(ui::base::container::fwMenuItem::sptr menuItem : m_menuItems)
    {
        menuItem->destroyContainer();
    }

    m_menuItems.clear();
    for(ui::base::container::fwMenu::sptr menu : m_menus)
    {
        menu->destroyContainer();
    }

    m_menus.clear();
    for(ui::base::container::fwContainer::sptr container : m_containers)
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

} // namespace layoutManager

} // namespace sight::ui::base
