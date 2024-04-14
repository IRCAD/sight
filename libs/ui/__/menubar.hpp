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

#include <sight/ui/__/config.hpp>

#include "ui/__/container/widget.hpp"
#include "ui/__/layout/menubar_manager.hpp"

#include <service/base.hpp>

namespace sight::ui
{

namespace detail::registry
{

class menubar;

}
/**
 * @brief   Defines the service interface managing the menu bar.
 */
class SIGHT_UI_CLASS_API menubar : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(menubar, service::base);

    /// Method called when a menu service is stopping
    SIGHT_UI_API void menu_service_stopping(std::string _menu_srv_sid);

    /// Method called when a menu service is starting
    SIGHT_UI_API void menu_service_starting(std::string _menu_srv_sid);

protected:

    SIGHT_UI_API menubar()           = default;
    SIGHT_UI_API ~menubar() override = default;

    /**
     * @brief Initialize the layout and registry managers.
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
              <menu sid="myMenu" start=true />
              <menu sid="myMenu2" start=true />
          </registry>
       </service>
       @endcode
     *  - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *  - \<registry\> \</registry\> : (mandatory) describe the service management.
     *
     * @warning
     * - The number of item in the gui section must be equal or greater than in the registry section.
     * - The order of the menu in each section (gui and registry) must be the same.\n
     *   For example: the menu named "My menu" will be connected with the service which have the sid = "myMenu".
     * - A menu bar can't have the same service connected on two different menu.
     *
     *  @see ui::registry::menubar::initialize(),
     *::ui::layout::menubar_manager::initialize()
     */
    SIGHT_UI_API void initialize();

    /// Create the layout and start the managed services.
    SIGHT_UI_API void create();

    /// Destroy the layout and stop the managed services.
    SIGHT_UI_API void destroy();

private:

    void initialize_layout_manager(const ui::config_t& _layout_config);

    ui::layout::menubar_manager::sptr m_layout_manager;
    SPTR(ui::detail::registry::menubar) m_registry;

    /// Flag to hide or disable the menu if the service is stopped
    bool m_hide_menus {false};
};

} // namespace sight::ui
