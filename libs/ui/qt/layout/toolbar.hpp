/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/qt/container/toolbar.hpp"

#include <core/base.hpp>

#include <ui/__/container/toolbar.hpp>
#include <ui/__/layout/toolbar_manager.hpp>

#include <QAction>
#include <QTimer>

namespace sight::ui::qt::layout
{

/**
 * @brief Defines the tool bar layout manager for the GUI.
 */
class SIGHT_UI_QT_CLASS_API toolbar : public ui::layout::toolbar_manager
{
public:

    SIGHT_DECLARE_CLASS(
        toolbar,
        ui::layout::toolbar_manager,
        ui::factory::make<toolbar>
    )

    SIGHT_UI_QT_API ~toolbar() override = default;

    /**
     * @brief Instantiate actions with parent toolbar.
     * @pre layout must be initialized before.
     * @pre parent toolbar must be instanced.
     */
    SIGHT_UI_QT_API void create_layout(ui::container::toolbar::sptr _parent, const std::string& _id) override;

    /**
     * @brief Destroy local toolBars.
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

    SIGHT_UI_QT_API void set_visible(bool _is_visible) override;
    SIGHT_UI_QT_API bool visible() const override;

protected:

    ui::qt::container::toolbar::sptr m_parent;

    // Hidden QAction which allows to change the toolbar visibility.
    QAction* m_toggle_toolbar_visibility_action {nullptr};

private:

    /// @brief  Schedule the toolbar size adjustment.
    ///
    /// This is necessary because the correct toolbar size is not immediately available after its creation.
    /// In the case of overlay, re-layout is *not* done automatically when buttons are added or removed.
    ///
    /// @todo Investigate why this is mandatory.
    void schedule_adjust_size();

    std::unique_ptr<QTimer> m_timer;
};

} // namespace sight::ui::qt::layout
