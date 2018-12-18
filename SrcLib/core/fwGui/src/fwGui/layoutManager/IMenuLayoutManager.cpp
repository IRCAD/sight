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

#include "fwGui/layoutManager/IMenuLayoutManager.hpp"

#include <fwRuntime/operations.hpp>

namespace fwGui
{
namespace layoutManager
{

const IMenuLayoutManager::RegistryKeyType IMenuLayoutManager::REGISTRY_KEY = "::fwGui::MenuLayoutManager";

//-----------------------------------------------------------------------------

IMenuLayoutManager::IMenuLayoutManager()
{
}

//-----------------------------------------------------------------------------

IMenuLayoutManager::~IMenuLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IMenuLayoutManager::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for( iter = configuration->begin(); iter != configuration->end(); ++iter )
    {
        if( (*iter)->getName() == "menuItem" )
        {
            ConfigurationType menuItem = *iter;
            ActionInfo info;

            SLM_ASSERT("Depreciated tag <state>", !menuItem->hasAttribute("state"));
            SLM_ASSERT("Depreciated tag <enable>", !menuItem->hasAttribute("enable"));

            SLM_ASSERT("missing <name> attribute", menuItem->hasAttribute("name"));
            if( menuItem->hasAttribute("name") )
            {
                info.m_name = menuItem->getExistingAttributeValue("name");
            }

            if( menuItem->hasAttribute("shortcut") )
            {
                info.m_shortcut = menuItem->getExistingAttributeValue("shortcut");
            }

            if( menuItem->hasAttribute("icon") )
            {
                info.m_icon = ::fwRuntime::getBundleResourceFilePath(menuItem->getAttributeValue("icon"));
            }

            if( menuItem->hasAttribute("style") )
            {
                std::string style = menuItem->getExistingAttributeValue("style");
                info.m_isCheckable = (style == "check");
                info.m_isRadio     = (style == "radio");
            }

            if( menuItem->hasAttribute("specialAction") )
            {
                std::string specialActionName = menuItem->getExistingAttributeValue("specialAction");
                if (specialActionName == "DEFAULT")
                {
                    info.m_type = DEFAULT;
                }
                else if (specialActionName == "QUIT")
                {
                    info.m_type = QUIT;
                }
                else if (specialActionName == "ABOUT")
                {
                    info.m_type = ABOUT;
                }
                else if (specialActionName == "HELP")
                {
                    info.m_type = HELP;
                }
                else if (specialActionName == "NEW")
                {
                    info.m_type = NEW;
                }
                else
                {
                    OSLM_FATAL("specialAction " << specialActionName << " is unknown." );
                }
            }

            m_actionInfo.push_back(info);
        }
        if( (*iter)->getName() == "separator" )
        {
            ActionInfo info;
            info.m_isSeparator = true;
            info.m_type        = SEPARATOR;
            m_actionInfo.push_back( info );
        }

        if( (*iter)->getName() == "menu" )
        {
            ActionInfo info;
            info.m_isMenu = true;
            if( (*iter)->hasAttribute("name") )
            {
                info.m_name = (*iter)->getExistingAttributeValue("name");
            }
            m_actionInfo.push_back( info );
        }
    }
}

//-----------------------------------------------------------------------------

void IMenuLayoutManager::destroyActions()
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
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwMenuItem::sptr > IMenuLayoutManager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwMenu::sptr > IMenuLayoutManager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui

