/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

/**
 * @file ui/base/layoutManager/IMenuBarLayoutManager.cpp
 * @brief This file defines the implementation of the base class for managing a menubar.
 *
 */

#include "ui/base/layoutManager/IMenuBarLayoutManager.hpp"

namespace sight::ui::base
{
namespace layoutManager
{

const IMenuBarLayoutManager::RegistryKeyType IMenuBarLayoutManager::REGISTRY_KEY = "::ui::base::MenuBarLayoutManager";

//-----------------------------------------------------------------------------

IMenuBarLayoutManager::IMenuBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

IMenuBarLayoutManager::~IMenuBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IMenuBarLayoutManager::initialize( ConfigurationType configuration)
{
    SLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
               configuration->getName() == "layout");

    std::vector < ConfigurationType > vectMenus = configuration->find("menu");
    m_menus.clear();
    for (ConfigurationType menu : vectMenus)
    {
        SLM_ASSERT("missing <name> attribute", menu->hasAttribute("name"));
        if( menu->hasAttribute("name") )
        {
            std::string name = menu->getExistingAttributeValue("name");
            m_menuNames.push_back(name);
        }
    }
}

//-----------------------------------------------------------------------------

void IMenuBarLayoutManager::destroyMenus()
{
    for( ui::base::container::fwMenu::sptr menu :  m_menus)
    {
        menu->destroyContainer();
    }
    m_menus.clear();
}

//-----------------------------------------------------------------------------

std::vector< ui::base::container::fwMenu::sptr > IMenuBarLayoutManager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace sight::ui::base
