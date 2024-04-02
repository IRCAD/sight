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

#include "ui/__/config.hpp"
#include "ui/__/detail/registry/menu.hpp"
#include "ui/__/layout/menu_manager.hpp"

#include <service/base.hpp>

namespace sight::ui
{

namespace detail::registry
{

class menu;

}

/**
 * @brief   Defines the service interface managing the menu.
 */
class UI_CLASS_API menu : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(menu, service::base);

    /// Method called when an action service is stopping
    UI_API void action_service_stopping(std::string _action_srv_sid);

    /// Method called when an action service is starting
    UI_API void action_service_starting(std::string _action_srv_sid);

    /// Method called when the action service is activated
    UI_API void action_service_set_checked(std::string _action_srv_sid, bool _is_checked);

    /// Method called when the action service is executable
    UI_API void action_service_set_enabled(std::string _action_srv_sid, bool _is_enabled);

    /// Method called when the action service is visible
    UI_API void action_service_set_visible(std::string _action_srv_sid, bool _is_visible);

protected:

    UI_API ~menu() override = default;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
        <service uid="myMenu" type="ui::menu" impl="sight::module::ui::menu">
            <gui>
                <layout hideActions="false">
                    <menuItem name="My item 1" shortcut="1" style="check" />
                    <separator />
                    <menuItem name="My item 2" shortcut="2" style="radio" />
                    <menuItem name="My item 3" shortcut="3" style="radio" />
                    <separator />
                    <menu name="My menu" />
                    <separator />
                    <menuItem name="Quit" shortcut="Ctrl+Q" specialAction="QUIT" />
                </layout>
            </gui>
            <registry>
                <menuItem sid="item1" start="true" />
                <menuItem sid="item2" start="false" />
                <menuItem sid="item3" start="false" />
                <menu sid="myMenu" start="true" />
                <menuItem sid="actionQuit" start="false" />
            </registry>
        </service>
       @endcode
     * - \<gui\> \</gui\> : (mandatory) describe the interface of the service. It must contain a layout section.
     *   - \<layout\> \</layout\> : (mandatory) describe the layout of the service
     *     - \b hideActions: (optional, default=false): if true, the actions are hidden when they are stopped.
     * - \<registry\> \</registry\> : (mandatory) describe the service management.
     *   - \<menuItem\> represents action
     *   - \<menu\> represents menu
     *
     * @warning
     * - The number of item in the gui section must be <b> EQUAL </b> to the registry section.
     * - The order of the menu item in each section (gui and registry) must be the same.\n
     *   For example: the menu item named "My item 1" will be connected with the service which have the sid = "item1".
     * - A menu can't have the same service connected on two different menu items.
     *
     * @see ui::registry::menu::initialize(), ui::layout::menu_manager::initialize()
     */
    UI_API void initialize();

    /// Create the layout and start the managed services.
    UI_API void create();

    /// Destroy the layout and stop the managed services.
    UI_API void destroy();

private:

    void initialize_layout_manager(const ui::config_t& _layout_config);

    ui::layout::menu_manager::sptr m_layout_manager;
    SPTR(ui::detail::registry::menu) m_registry;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hide_actions {false};
};

} // namespace sight::ui
