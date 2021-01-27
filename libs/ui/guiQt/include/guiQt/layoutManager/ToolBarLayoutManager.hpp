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
#include "guiQt/container/QtToolBarContainer.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <gui/container/fwToolBar.hpp>
#include <gui/layoutManager/IToolBarLayoutManager.hpp>

namespace sight::gui
{
namespace layoutManager
{

/**
 * @brief   Defines the toolBar layout manager for IHM.
 */
class GUIQT_CLASS_API ToolBarLayoutManager : public gui::layoutManager::IToolBarLayoutManager
{
public:
    fwCoreClassMacro(ToolBarLayoutManager, gui::layoutManager::IToolBarLayoutManager,
                     gui::factory::New< ToolBarLayoutManager >)

    GUIQT_API ToolBarLayoutManager(gui::GuiBaseObject::Key key);

    GUIQT_API virtual ~ToolBarLayoutManager();

    /**
     * @brief Instantiate actions with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    GUIQT_API virtual void createLayout( gui::container::fwToolBar::sptr parent ) override;

    /**
     * @brief Destroy local toolBars.
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
    guiQt::container::QtToolBarContainer::sptr m_parent;

};

} // namespace layoutManager
} // namespace sight::gui
