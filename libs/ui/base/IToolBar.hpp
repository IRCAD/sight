/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/layoutManager/IToolBarLayoutManager.hpp"
#include "ui/base/registry/ToolBar.hpp"

#include <service/IService.hpp>

namespace sight::ui::base
{

/**
 * @brief   Defines the service interface managing the toolBar.
 */
class UI_BASE_CLASS_API IToolBar : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IToolBar, service::IService);

    /// Method called when an action service is stopping
    UI_BASE_API void actionServiceStopping(std::string actionSrvSID);

    /// Method called when an action service is starting
    UI_BASE_API void actionServiceStarting(std::string actionSrvSID);

    /// Method called when the action service is activated
    UI_BASE_API void actionServiceSetActive(std::string actionSrvSID, bool isActive);

    /// Method called when the action service is executable
    UI_BASE_API void actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable);

    /// Method called when the action service is visible
    UI_BASE_API void actionServiceSetVisible(std::string actionSrvSID, bool isVisible);

protected:

    UI_BASE_API IToolBar();

    UI_BASE_API virtual ~IToolBar();

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="ui::base::IToolBar" impl="::sight::module::ui::base::SToolBar"
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
     *   - \<menuItem\> represents IAction
     *   - \<menu\> represents IMenu
     *   - \<editor\> represents container service (IEditor, IView, ...)
     *
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the item in each section (gui and registry) must be the same.\n
     *  For example: the item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *  @see ui::base::registry::ToolBar::initialize(),
     *::ui::base::layoutManager::IToolBarLayoutManager::initialize()
     */
    UI_BASE_API void initialize();

    /// Create the layout and start the managed services.
    UI_BASE_API void create();

    /// Destroy the layout and stop the managed services.
    UI_BASE_API void destroy();

private:

    void initializeLayoutManager(core::runtime::ConfigurationElement::sptr layoutConfig);

    ui::base::layoutManager::IToolBarLayoutManager::sptr m_layoutManager;
    ui::base::registry::ToolBar::sptr m_registry;

    ConfigurationType m_registryConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hideActions;
};

} // namespace sight::ui::base
