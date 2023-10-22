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

/**
 * @brief This file defines the interface of the base class for managing a menubar.
 */

#pragma once

#include "ui/__/config.hpp"
#include "ui/__/container/menu.hpp"
#include "ui/__/container/menubar.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::layout
{

/**
 * @brief   Defines the menu bar layout manager for IHM.
 */
class UI_CLASS_API menubar_manager : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menubar_manager, ui::object);

    using registry_key_t = std::string;

    UI_API const static registry_key_t REGISTRY_KEY;

    /// Constructor. Do nothing.
    UI_API menubar_manager() = default;

    /// Destructor. Do nothing.
    UI_API ~menubar_manager() override = default;

    /**
     * @brief Returns the vector of menu managed by this layout.
     */
    UI_API virtual std::vector<ui::container::menu::sptr> getMenus();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="ui::menu_bar" impl="sight::module::ui::menu_bar"
     * auto_connect="false"
     *>
          <gui>
              <layout>
                  <menu name="My menu"/>
                  <menu name="My menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" start="true" />
              <menu sid="myMenu2" start="true" />
          </registry>
       </service>
       @endcode
     * This method analyzes the gui section of the configuration.
     *  - \<layout\> (mandatory) : give the list of the menu that will appear in the menu bar.
     *  - \<menu name="My menu"/\> :
     *   - \b name (mandatory) : give the name of the menu that will appear in the interface.
     */

    UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre layout must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    UI_API virtual void createLayout(ui::container::menubar::sptr _parent, const std::string& _id) = 0;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    UI_API virtual void destroyLayout() = 0;

    /**
     * @brief Set the menu visibility.
     */
    UI_API virtual void menuIsVisible(ui::container::menu::sptr, bool _is_visible) = 0;

    /**
     * @brief Set the menu enable or not.
     */
    UI_API virtual void menuIsEnabled(ui::container::menu::sptr, bool _is_enabled) = 0;

protected:

    /**
     * @brief Helper to destroy local sub views.
     */
    UI_API virtual void destroyMenus();

    /// All sub containers managed by this layout.
    std::vector<ui::container::menu::sptr> m_menus;

    /// Save menu name from configuration.
    std::vector<std::string> m_menuNames;
};

} // namespace sight::ui::layout
