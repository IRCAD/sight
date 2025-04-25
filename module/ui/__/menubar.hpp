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

#include <ui/__/menubar.hpp>

namespace sight::module::ui
{

/**
 * @brief   Defines the default menubar for standard application
 */
class menubar : public sight::ui::menubar
{
public:

    SIGHT_DECLARE_SERVICE(menubar, sight::ui::menubar);

    /// Constructor. Do nothing.
    menubar() noexcept;

    /// Destructor. Do nothing.
    ~menubar() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /**
     * @brief Configuring method allows to configure an application with menu bar.
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
     *  @see sight::ui::menubar::initialize(),
     * sight::ui::layout::menubar_manager::initialize()
     */
    void configuring() override;

    /**
     * @brief Create the menus and start the managed services.
     * @see sight::ui::menubar::create()
     */
    void starting() override;

    /**
     * @brief Stop all menu of this menubar and destroy menu_bar
     * @see sight::ui::menubar::destroy()
     */
    void stopping() override;

    /// Updating service, do nothing.
    void updating() override;

    ///@}
};

} // namespace sight::module::ui
