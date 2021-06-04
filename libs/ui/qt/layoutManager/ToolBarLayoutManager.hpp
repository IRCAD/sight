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

#include "ui/qt/config.hpp"
#include "ui/qt/container/QtToolBarContainer.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <ui/base/container/fwToolBar.hpp>
#include <ui/base/layoutManager/IToolBarLayoutManager.hpp>

namespace sight::ui::qt
{

namespace layoutManager
{

/**
 * @brief   Defines the tool bar layout manager for the GUI.
 */
class UI_QT_CLASS_API ToolBarLayoutManager : public ui::base::layoutManager::IToolBarLayoutManager
{
public:

    SIGHT_DECLARE_CLASS(
        ToolBarLayoutManager,
        ui::base::layoutManager::IToolBarLayoutManager,
        ui::base::factory::New<ToolBarLayoutManager>
    )

    UI_QT_API ToolBarLayoutManager(ui::base::GuiBaseObject::Key key);

    UI_QT_API virtual ~ToolBarLayoutManager();

    /**
     * @brief Instantiate actions with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    UI_QT_API void createLayout(ui::base::container::fwToolBar::sptr parent) override;

    /**
     * @brief Destroy local toolBars.
     * @pre services using this actions must be stopped before.
     */
    UI_QT_API void destroyLayout() override;

    /**
     * @brief Set the action visibility.
     */
    UI_QT_API void menuItemSetVisible(ui::base::container::fwMenuItem::sptr menuItem, bool isVisible) override;

    /**
     * @brief Set the action enable or not.
     */
    UI_QT_API void menuItemSetEnabled(ui::base::container::fwMenuItem::sptr menuItem, bool isEnabled) override;

    /**
     * @brief Set the action checked or not.
     */
    UI_QT_API void menuItemSetChecked(ui::base::container::fwMenuItem::sptr, bool isChecked) override;

protected:

    ui::qt::container::QtToolBarContainer::sptr m_parent;
};

} // namespace layoutManager

} // namespace sight::ui::qt
