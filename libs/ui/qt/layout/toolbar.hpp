/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "ui/qt/container/toolbar.hpp"

#include <core/base.hpp>

#include <ui/__/container/toolbar.hpp>
#include <ui/__/layout/toolbar_manager.hpp>

#include <QAction>

namespace sight::ui::qt::layout
{

/**
 * @brief Defines the tool bar layout manager for the GUI.
 */
class UI_QT_CLASS_API toolbar : public ui::layout::toolbar_manager
{
public:

    SIGHT_DECLARE_CLASS(
        toolbar,
        ui::layout::toolbar_manager,
        ui::factory::make<toolbar>
    )

    UI_QT_API ~toolbar() override = default;

    /**
     * @brief Instantiate actions with parent toolBar.
     * @pre layout must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    UI_QT_API void createLayout(ui::container::toolbar::sptr parent, const std::string& id) override;

    /**
     * @brief Destroy local toolBars.
     * @pre services using this actions must be stopped before.
     */
    UI_QT_API void destroyLayout() override;

    /**
     * @brief Set the action visibility.
     */
    UI_QT_API void menuItemSetVisible(ui::container::menu_item::sptr menuItem, bool isVisible) override;

    /**
     * @brief Set the action enable or not.
     */
    UI_QT_API void menuItemSetEnabled(ui::container::menu_item::sptr menuItem, bool isEnabled) override;

    /**
     * @brief Set the action checked or not.
     */
    UI_QT_API void menuItemSetChecked(ui::container::menu_item::sptr /*menu_item*/, bool isChecked) override;

    UI_QT_API void setVisible(bool isVisible) override;

protected:

    ui::qt::container::toolbar::sptr m_parent;

    // Hidden QAction which allows to change the toolbar visibility.
    QAction* m_toggleToolbarVisibilityAction {nullptr};
};

} // namespace sight::ui::qt::layout
