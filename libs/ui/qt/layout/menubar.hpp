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
#include "ui/qt/container/menubar.hpp"

#include <core/base.hpp>

#include <ui/__/container/menu.hpp>
#include <ui/__/container/menubar.hpp>
#include <ui/__/layout/menubar_manager.hpp>

namespace sight::ui::qt::layout
{

/**
 * @brief   Defines the menu bar layout manager for the GUI.
 */
class UI_QT_CLASS_API menubar : public ui::layout::menubar_manager
{
public:

    SIGHT_DECLARE_CLASS(
        menubar,
        ui::layout::menubar_manager,
        ui::factory::make<menubar>
    )

    UI_QT_API ~menubar() override = default;

    /**
     * @brief Instantiate menu with parent menubar.
     * @pre layout must be initialized before.
     * @pre parent menubar must be instanced.
     */
    UI_QT_API void create_layout(ui::container::menubar::sptr _parent, const std::string& _id) override;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    UI_QT_API void destroy_layout() override;

    /**
     * @brief Set the menu visibility.
     */
    UI_QT_API void menu_is_visible(ui::container::menu::sptr _menu, bool _is_visible) override;

    /**
     * @brief Set the menu enable or not.
     */
    UI_QT_API void menu_is_enabled(ui::container::menu::sptr _menu, bool _is_enabled) override;

protected:

    ui::qt::container::menubar::sptr m_parent;
};

} // namespace sight::ui::qt::layout
