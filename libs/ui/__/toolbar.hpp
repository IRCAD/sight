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
#include "ui/__/detail/registry/ToolBar.hpp"
#include "ui/__/layout/toolbar_manager.hpp"

#include <service/base.hpp>

#include <ui/__/parameter.hpp>

namespace sight::ui
{

namespace detail::registry
{

class ToolBar;

}

/**
 * @brief   Defines the service interface managing the toolBar.
 */
class UI_CLASS_API toolbar : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(toolbar, service::base);

    /// Method called when an action service is stopping
    UI_API void actionServiceStopping(std::string actionSrvSID);

    /// Method called when an action service is starting
    UI_API void actionServiceStarting(std::string actionSrvSID);

    /// Method called when the action service is activated
    UI_API void actionServiceSetChecked(std::string actionSrvSID, bool isChecked);

    /// Method called when the action service is executable
    UI_API void actionServiceSetEnabled(std::string actionSrvSID, bool isEnabled);

    /// Method called when the action service is visible
    UI_API void actionServiceSetVisible(std::string actionSrvSID, bool isVisible);

protected:

    UI_API toolbar();
    UI_API ~toolbar() override = default;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="ui::toolbar" impl="sight::module::ui::SToolBar"
     * autoConnect="false"
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
     *   - \<editor\> represents container service (editor, IView, ...)
     *
     *  * @section Slots Slots
     * - \b setVisible(bool isVisible) : this slot shows the toolBar (if isVisible = true) or hides it.
     * - \b setVisibleByParam(ui::parameter_t isVisible) : this slot shows the toolBar (if isEnabled holds boolean
     * alternative) or hides it.
     * - \b show() : this slot shows the toolBar.
     * - \b hide() : this slot hides the toolBar.
     *
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the item in each section (gui and registry) must be the same.\n
     *  For example: the item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *  @see ui::registry::ToolBar::initialize(),
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
        static inline const key_t SET_VISIBLE_SLOT          = "setVisible";
        static inline const key_t SET_VISIBLE_BY_PARAM_SLOT = "setVisibleByParam";
        static inline const key_t SHOW_SLOT                 = "show";
        static inline const key_t HIDE_SLOT                 = "hide";
    };

    /// SLOT: show/hide the container
    UI_API void setVisible(bool isVisible);

    /// SLOT: show/hide the container using parameter_t (only testing bool alternative).
    UI_API void setVisibleByParameter(ui::parameter_t);

    /// SLOT: show the container
    UI_API void show();

    /// SLOT: hide the container
    UI_API void hide();

private:

    void initializeLayoutManager(const ui::config_t& layoutConfig);

    ui::layout::toolbar_manager::sptr m_layoutManager;
    SPTR(ui::detail::registry::ToolBar) m_registry;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hideActions {false};
};

} // namespace sight::ui
