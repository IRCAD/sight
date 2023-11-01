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
#include "ui/__/detail/registry/toolbar.hpp"
#include "ui/__/layout/toolbar_manager.hpp"

#include <service/base.hpp>

#include <ui/__/parameter.hpp>

namespace sight::ui
{

namespace detail::registry
{

class toolbar;

}

/**
 * @brief   Defines the service interface managing the toolbar.
 */
class UI_CLASS_API toolbar : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(toolbar, service::base);

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

    UI_API toolbar();
    UI_API ~toolbar() override = default;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="ui::toolbar" impl="sight::module::ui::toolbar"
     * auto_connect="false"
     *>
           <gui>
               <layout hideActions="false" style="ToolButtonIconOnly">
                   <menuItem name="My item 2" style="radio" icon="TutoGui/system.png"/>
                   <menuItem name="My item 3" style="radio" icon="TutoGui/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="TutoGui/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="TutoGui/monkey.png"/>
                   <separator />
                   <menu name="My menu" />
                   <separator />
                   <editor />
               </layout>
           </gui>
           <registry>
               <menuItem sid="item2" />
               <menuItem sid="item3" />
               <menuItem sid="item4" />
               <menuItem sid="item5" />
               <menu sid="menu" />
               <editor sid="editor" />
           </registry>
       </service>
       @endcode
     * - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *   - \<layout\> \</layout\> : (mandatory) describe the layout of the service
     *     - \b hideActions: (optional, default=false): if true, the actions are hidden when they are stopped.
     *     - \b style: (optional, default=ToolButtonIconOnly):  describe the style of the tool button (possible values:
     * ToolButtonIconOnly/ToolButtonTextOnly/ToolButtonTextBesideIcon/ToolButtonTextUnderIcon/ToolButtonFollowStyle )
     * - \<registry\> \</registry\> : (mandatory) describe the service management.
     *   - \<menuItem\> represents action
     *   - \<menu\> represents menu
     *   - \<editor\> represents container service (editor, view, ...)
     *
     *  * @section Slots Slots
     * - \b set_visible(bool isVisible) : this slot shows the toolbar (if isVisible = true) or hides it.
     * - \b set_visible_by_parameter(ui::parameter_t isVisible) : this slot shows the toolbar (if isEnabled holds
     * boolean
     * alternative) or hides it.
     * - \b show() : this slot shows the toolbar.
     * - \b hide() : this slot hides the toolbar.
     *
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the item in each section (gui and registry) must be the same.\n
     *  For example: the item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *  @see ui::registry::toolbar::initialize(),
     *::ui::layout::toolbar_manager::initialize()
     */
    UI_API void initialize();

    /// Create the layout and start the managed services.
    UI_API void create();

    /// Destroy the layout and stop the managed services.
    UI_API void destroy();

    /// @brief slots: change the toolbar visibility
    struct slots
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t SET_VISIBLE_SLOT          = "set_visible";
        static inline const key_t SET_VISIBLE_BY_PARAM_SLOT = "setVisibleByParam";
        static inline const key_t SHOW_SLOT                 = "show";
        static inline const key_t HIDE_SLOT                 = "hide";
    };

    /// SLOT: show/hide the container
    UI_API void set_visible(bool _is_visible);

    /// SLOT: show/hide the container using parameter_t (only testing bool alternative).
    UI_API void set_visible_by_parameter(ui::parameter_t);

    /// SLOT: show the container
    UI_API void show();

    /// SLOT: hide the container
    UI_API void hide();

private:

    void initialize_layout_manager(const ui::config_t& _layout_config);

    ui::layout::toolbar_manager::sptr m_layout_manager;
    SPTR(ui::detail::registry::toolbar) m_registry;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hide_actions {false};
};

} // namespace sight::ui
