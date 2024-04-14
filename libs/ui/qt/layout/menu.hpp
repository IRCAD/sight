/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/qt/config.hpp>

#include "ui/qt/container/menu.hpp"

#include <core/base.hpp>

#include <ui/__/container/menu.hpp>
#include <ui/__/layout/menu_manager.hpp>

namespace sight::ui::qt::layout
{

/**
 * @brief   Defines the menu layout manager for the GUI.
 */
class SIGHT_UI_QT_CLASS_API menu : public ui::layout::menu_manager
{
public:

    SIGHT_DECLARE_CLASS(
        menu,
        ui::layout::menu_manager,
        ui::factory::make<menu>
    )

    SIGHT_UI_QT_API ~menu() override = default;

    /**
     * @brief Instantiate actions with parent menu.
     * @pre layout must be initialized before.
     * @pre parent menu must be instanced.
     */
    SIGHT_UI_QT_API void create_layout(ui::container::menu::sptr _parent, const std::string& _id) override;

    /**
     * @brief Destroy local menus.
     * @pre services using this actions must be stopped before.
     */
    SIGHT_UI_QT_API void destroy_layout() override;

    /**
     * @brief Set the action visibility.
     */
    SIGHT_UI_QT_API void menu_item_set_visible(ui::container::menu_item::sptr _menu_item, bool _is_visible) override;

    /**
     * @brief Set the action enable or not.
     */
    SIGHT_UI_QT_API void menu_item_set_enabled(ui::container::menu_item::sptr _menu_item, bool _is_enabled) override;

    /**
     * @brief Set the action checked or not.
     */
    SIGHT_UI_QT_API void menu_item_set_checked(ui::container::menu_item::sptr /*menu_item*/, bool _is_checked) override;

protected:

    ui::qt::container::menu::sptr m_parent;
};

} // namespace sight::ui::qt::layout
