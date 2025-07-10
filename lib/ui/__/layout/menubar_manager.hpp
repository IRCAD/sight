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

/**
 * @brief This file defines the interface of the base class for managing a menubar.
 */

#pragma once

#include <sight/ui/__/config.hpp>

#include "ui/__/container/menu.hpp"
#include "ui/__/container/menubar.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::layout
{

/**
 * @brief   Defines the menu bar layout manager for UI.
 */
class SIGHT_UI_CLASS_API menubar_manager : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menubar_manager, ui::object);

    using registry_key_t = std::string;

    SIGHT_UI_API const static registry_key_t REGISTRY_KEY;

    /// Constructor. Do nothing.
    SIGHT_UI_API menubar_manager() = default;

    /// Destructor. Do nothing.
    SIGHT_UI_API ~menubar_manager() override = default;

    /**
     * @brief Returns the vector of menu managed by this layout.
     */
    SIGHT_UI_API virtual std::vector<ui::container::menu::sptr> get_menus();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="menubar" type="ui::menubar" impl="sight::module::ui::menubar"
     * auto_connect="false"
     *>
          <gui>
              <layout>
                  <menu name="My menu"/>
                  <menu name="My menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" />
              <menu sid="myMenu2" />
          </registry>
       </service>
       @endcode
     * This method analyzes the gui section of the configuration.
     *  - \<layout\> (mandatory) : give the list of the menu that will appear in the menu bar.
     *  - \<menu name="My menu"/\> :
     *   - \b name (mandatory) : give the name of the menu that will appear in the interface.
     */

    SIGHT_UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Instantiate menu with parent menubar.
     * @pre layout must be initialized before.
     * @pre parent menubar must be instanced.
     */
    SIGHT_UI_API virtual void create_layout(ui::container::menubar::sptr _parent, const std::string& _id) = 0;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    SIGHT_UI_API virtual void destroy_layout() = 0;

    /**
     * @brief Set the menu visibility.
     */
    SIGHT_UI_API virtual void menu_is_visible(ui::container::menu::sptr, bool _is_visible) = 0;

    /**
     * @brief Set the menu enable or not.
     */
    SIGHT_UI_API virtual void menu_is_enabled(ui::container::menu::sptr, bool _is_enabled) = 0;

protected:

    /**
     * @brief Helper to destroy local sub views.
     */
    SIGHT_UI_API virtual void destroy_menus();

    /// All sub containers managed by this layout.
    std::vector<ui::container::menu::sptr> m_menus;

    /// Save menu name from configuration.
    std::vector<std::string> m_menu_names;
};

} // namespace sight::ui::layout
