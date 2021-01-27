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

#include "gui/config.hpp"
#include "gui/layoutManager/IToolBarLayoutManager.hpp"
#include "gui/registrar/ToolBarRegistrar.hpp"

#include <services/IService.hpp>

namespace sight::gui
{

/**
 * @brief   Defines the service interface managing the toolBar.
 */
class GUI_CLASS_API IToolBarSrv : public services::IService
{

public:

    fwCoreServiceMacro(IToolBarSrv, services::IService)

    /// Method called when an action service is stopping
    GUI_API void actionServiceStopping(std::string actionSrvSID);

    /// Method called when an action service is starting
    GUI_API void actionServiceStarting(std::string actionSrvSID);

    /// Method called when the action service is activated
    GUI_API void actionServiceSetActive(std::string actionSrvSID, bool isActive);

    /// Method called when the action service is executable
    GUI_API void actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable);

    /// Method called when the action service is visible
    GUI_API void actionServiceSetVisible(std::string actionSrvSID, bool isVisible);

protected:

    GUI_API IToolBarSrv();

    GUI_API virtual ~IToolBarSrv();

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="::gui::IToolBarSrv" impl="::modules::gui::aspect::SDefaultToolBar" autoConnect="no"
     *>
           <gui>
               <layout hideActions="false" style="ToolButtonIconOnly">
                   <menuItem name="My item 2" style="radio" icon="TutoGui-0.1/icons/system.png"/>
                   <menuItem name="My item 3" style="radio" icon="TutoGui-0.1/icons/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
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
     *   - \<menuItem\> represents IActionSrv
     *   - \<menu\> represents IMenuSrv
     *   - \<editor\> represents container service (IEditor, IView, ...)
     *
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the item in each section (gui and registry) must be the same.\n
     *  For example: the item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *  @see gui::registrar::ToolBarRegistrar::initialize(),
     *::gui::layoutManager::IToolBarLayoutManager::initialize()
     */
    GUI_API void initialize();

    /// Create the layout and start the managed services.
    GUI_API void create();

    /// Destroy the layout and stop the managed services.
    GUI_API void destroy();

private:

    void initializeLayoutManager( core::runtime::ConfigurationElement::sptr layoutConfig );

    gui::layoutManager::IToolBarLayoutManager::sptr m_layoutManager;
    gui::registrar::ToolBarRegistrar::sptr m_registrar;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hideActions;
};

} // namespace sight::gui
