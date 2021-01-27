/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "guiQt/config.hpp"
#include "guiQt/container/QtMenuContainer.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <gui/container/fwMenu.hpp>
#include <gui/layoutManager/IMenuLayoutManager.hpp>

namespace sight::gui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu layout manager for IHM.
 */
class GUIQT_CLASS_API MenuLayoutManager : public gui::layoutManager::IMenuLayoutManager
{
public:
    fwCoreClassMacro(MenuLayoutManager, gui::layoutManager::IMenuLayoutManager,
                     gui::factory::New< MenuLayoutManager >)

    GUIQT_API MenuLayoutManager(gui::GuiBaseObject::Key key);

    GUIQT_API virtual ~MenuLayoutManager();

    /**
     * @brief Instantiate actions with parent menu.
     * @pre LayoutManager must be initialized before.
     * @pre parent menu must be instanced.
     */
    GUIQT_API virtual void createLayout( gui::container::fwMenu::sptr parent ) override;

    /**
     * @brief Destroy local menus.
     * @pre services using this actions must be stopped before.
     */
    GUIQT_API virtual void destroyLayout() override;

    /**
     * @brief Set the action visibility.
     */
    GUIQT_API virtual void menuItemSetVisible(gui::container::fwMenuItem::sptr menuItem, bool isVisible) override;

    /**
     * @brief Set the action enable or not.
     */
    GUIQT_API virtual void menuItemSetEnabled(gui::container::fwMenuItem::sptr menuItem, bool isEnabled) override;

    /**
     * @brief Set the action checked or not.
     */
    GUIQT_API virtual void menuItemSetChecked(gui::container::fwMenuItem::sptr, bool isChecked) override;

protected:
    guiQt::container::QtMenuContainer::sptr m_parent;

};

} // namespace layoutManager
} // namespace sight::gui
