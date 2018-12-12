/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwGui/layoutManager/IToolBarLayoutManager.hpp"

#include <fwRuntime/operations.hpp>

#include <boost/lexical_cast.hpp>

namespace fwGui
{
namespace layoutManager
{

const IToolBarLayoutManager::RegistryKeyType IToolBarLayoutManager::REGISTRY_KEY = "::fwGui::ToolBarLayoutManager";

//-----------------------------------------------------------------------------

IToolBarLayoutManager::IToolBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

IToolBarLayoutManager::~IToolBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for( iter = configuration->begin(); iter != configuration->end(); ++iter )
    {
        if( (*iter)->getName() == "menuItem" )
        {
            ConfigurationType toolBarItem = *iter;
            ActionInfo info;
            SLM_ASSERT("Depreciated tag <state>", !toolBarItem->hasAttribute("state"));
            SLM_ASSERT("Depreciated tag <enable>", !toolBarItem->hasAttribute("enable"));

            SLM_ASSERT("missing <name> attribute", toolBarItem->hasAttribute("name"));
            if( toolBarItem->hasAttribute("name") )
            {
                info.m_name = toolBarItem->getExistingAttributeValue("name");
            }

            if( toolBarItem->hasAttribute("icon") )
            {
                info.m_icon = ::fwRuntime::getBundleResourceFilePath(toolBarItem->getAttributeValue("icon"));
            }

            if( toolBarItem->hasAttribute("icon2") )
            {
                SLM_ASSERT("'icon' attribute must be defined before 'icon2'", !info.m_icon.empty());
                info.m_icon2 = ::fwRuntime::getBundleResourceFilePath(toolBarItem->getAttributeValue("icon2"));
            }

            if( toolBarItem->hasAttribute("style") )
            {
                std::string style = toolBarItem->getExistingAttributeValue("style");
                info.m_isCheckable = (style == "check");
                info.m_isRadio     = (style == "radio");
            }

            if( toolBarItem->hasAttribute("shortcut") )
            {
                info.m_shortcut = toolBarItem->getExistingAttributeValue("shortcut");
            }
            m_actionInfo.push_back(info);
        }
        else if( (*iter)->getName() == "separator" )
        {
            ActionInfo info;
            info.m_isSeparator = true;

            if( (*iter)->hasAttribute("size") )
            {
                info.m_size = ::boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("size"));
            }

            m_actionInfo.push_back( info );
        }
        else if( (*iter)->getName() == "spacer" )
        {
            ActionInfo info;
            info.m_isSpacer = true;
            m_actionInfo.push_back( info );
        }
        else if( (*iter)->getName() == "menu" )
        {
            ActionInfo info;
            info.m_isMenu = true;
            if( (*iter)->hasAttribute("name") )
            {
                info.m_name = (*iter)->getExistingAttributeValue("name");
            }

            if( (*iter)->hasAttribute("icon") )
            {
                info.m_icon = ::fwRuntime::getBundleResourceFilePath((*iter)->getExistingAttributeValue("icon" ));
            }
            m_actionInfo.push_back( info );
        }
        else if( (*iter)->getName() == "editor" )
        {
            ActionInfo info;
            info.m_isEditor = true;

            m_actionInfo.push_back( info );
        }
    }
}

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::destroyActions()
{
    for( ::fwGui::container::fwMenuItem::sptr menuItem :  m_menuItems)
    {
        menuItem->destroyContainer();
    }
    m_menuItems.clear();
    for( ::fwGui::container::fwMenu::sptr menu :  m_menus)
    {
        menu->destroyContainer();
    }
    m_menus.clear();
    for( ::fwGui::container::fwContainer::sptr container :  m_containers)
    {
        container->destroyContainer();
    }
    m_containers.clear();
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwMenuItem::sptr > IToolBarLayoutManager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwMenu::sptr > IToolBarLayoutManager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwContainer::sptr > IToolBarLayoutManager::getContainers()
{
    return this->m_containers;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui

