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

#include "ui/__/config.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/layout/menubar_manager.hpp"

#include <service/base.hpp>

namespace sight::ui
{

namespace detail::registry
{

class menu_bar;

}
/**
 * @brief   Defines the service interface managing the menu bar.
 */
class UI_CLASS_API menu_bar : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(menu_bar, service::base);

    /// Method called when a menu service is stopping
    UI_API void menuServiceStopping(std::string _menu_srv_sid);

    /// Method called when a menu service is starting
    UI_API void menuServiceStarting(std::string _menu_srv_sid);

protected:

    UI_API menu_bar()           = default;
    UI_API ~menu_bar() override = default;

    /**
     * @brief Initialize the layout and registry managers.
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
     *  @see ui::registry::menu_bar::initialize(),
     *::ui::layout::menubar_manager::initialize()
     */
    UI_API void initialize();

    /// Create the layout and start the managed services.
    UI_API void create();

    /// Destroy the layout and stop the managed services.
    UI_API void destroy();

private:

    void initializeLayoutManager(const ui::config_t& _layout_config);

    ui::layout::menubar_manager::sptr m_layoutManager;
    SPTR(ui::detail::registry::menu_bar) m_registry;

    /// Flag to hide or disable the menu if the service is stopped
    bool m_hideMenus {false};
};

} // namespace sight::ui
