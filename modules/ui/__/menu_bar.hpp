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

#include "modules/ui/__/config.hpp"

#include <ui/__/menu_bar.hpp>

namespace sight::module::ui
{

/**
 * @brief   Defines the default menubar for standard application
 */
class MODULE_UI_CLASS_API menu_bar : public sight::ui::menu_bar
{
public:

    SIGHT_DECLARE_SERVICE(menu_bar, sight::ui::menu_bar);

    /// Constructor. Do nothing.
    MODULE_UI_API menu_bar() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_API ~menu_bar() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /**
     * @brief Configuring method allows to configure an application with menu bar.
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
     *  - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *  - \<registry\> \</registry\> : (mandatory) describe the service management.
     *   - sid of the menu services
     *   - start or not the menu service automatically
     *
     * @warning
     * - The number of item in the gui section must be equal or greater than in the registry section.
     * - The order of the menu in each section (gui and registry) must be the same.\n
     *   For example: the menu named "My menu" will be connected with the service which have the sid = "myMenu".
     * - A menu bar can't have the same service connected on two different menu.
     *
     *  @see sight::ui::menu_bar::initialize(),
     * sight::ui::layout::menubar_manager::initialize()
     */
    MODULE_UI_API void configuring() override;

    /**
     * @brief Create the menus and start the managed services.
     * @see sight::ui::menu_bar::create()
     */
    MODULE_UI_API void starting() override;

    /**
     * @brief Stop all menu of this menuBar and destroy menu_bar
     * @see sight::ui::menu_bar::destroy()
     */
    MODULE_UI_API void stopping() override;

    /// Updating service, do nothing.
    MODULE_UI_API void updating() override;

    ///@}
};

} // namespace sight::module::ui
