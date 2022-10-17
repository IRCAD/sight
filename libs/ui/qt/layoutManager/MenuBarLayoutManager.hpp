/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "ui/qt/container/QtMenuBarContainer.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <ui/base/container/fwMenu.hpp>
#include <ui/base/container/fwMenuBar.hpp>
#include <ui/base/layoutManager/IMenuBarLayoutManager.hpp>

namespace sight::ui::qt::layoutManager
{

/**
 * @brief   Defines the menu bar layout manager for the GUI.
 */
class UI_QT_CLASS_API MenuBarLayoutManager : public ui::base::layoutManager::IMenuBarLayoutManager
{
public:

    SIGHT_DECLARE_CLASS(
        MenuBarLayoutManager,
        ui::base::layoutManager::IMenuBarLayoutManager,
        ui::base::factory::New<MenuBarLayoutManager>
    )

    UI_QT_API MenuBarLayoutManager(ui::base::GuiBaseObject::Key key);

    UI_QT_API ~MenuBarLayoutManager() override;

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    UI_QT_API void createLayout(ui::base::container::fwMenuBar::sptr parent, const std::string& id) override;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    UI_QT_API void destroyLayout() override;

    /**
     * @brief Set the menu visibility.
     */
    UI_QT_API void menuIsVisible(ui::base::container::fwMenu::sptr fwMenu, bool isVisible) override;

    /**
     * @brief Set the menu enable or not.
     */
    UI_QT_API void menuIsEnabled(ui::base::container::fwMenu::sptr fwMenu, bool isEnabled) override;

protected:

    ui::qt::container::QtMenuBarContainer::sptr m_parent;
};

} // namespace sight::ui::qt::layoutManager
