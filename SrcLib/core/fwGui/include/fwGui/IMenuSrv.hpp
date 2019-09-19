/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGui/config.hpp"
#include "fwGui/layoutManager/IMenuLayoutManager.hpp"
#include "fwGui/registrar/MenuRegistrar.hpp"

#include <fwServices/IService.hpp>

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the menu.
 */
class FWGUI_CLASS_API IMenuSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IMenuSrv)(::fwServices::IService) )

    /// Method called when an action service is stopping
    FWGUI_API void actionServiceStopping(std::string actionSrvSID);

    /// Method called when an action service is starting
    FWGUI_API void actionServiceStarting(std::string actionSrvSID);

    /// Method called when the action service is activated
    FWGUI_API void actionServiceSetActive(std::string actionSrvSID, bool isActive);

    /// Method called when the action service is executable
    FWGUI_API void actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable);

    /// Method called when the action service is visible
    FWGUI_API void actionServiceSetVisible(std::string actionSrvSID, bool isVisible);

protected:

    FWGUI_API IMenuSrv();

    FWGUI_API virtual ~IMenuSrv();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
        <service uid="myMenu" type="::fwGui::IMenuSrv" impl="::gui::aspect::SDefaultMenu" autoConnect="no" >
            <gui>
                <layout hideAction="false">
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
                <menuItem sid="item1" start="yes" />
                <menuItem sid="item2" start="no" />
                <menuItem sid="item3" start="no" />
                <menu sid="mymenu" start="yes" />
                <menuItem sid="actionQuit" start="no" />
            </registry>
        </service>
       @endcode
     * - \<gui\> \</gui\> : (mandatory) describe the interface of the service. It must contain a layout section.
     *   - \<layout\> \</layout\> : (mandatory) describe the layout of the service
     *     - \b hideAction: (optional, default=false): if true, the actions are hidden when they are stopped.
     * - \<registry\> \</registry\> : (mandatory) describe the service management.
     *   - \<menuItem\> represents IActionSrv
     *   - \<menu\> represents IMenuSrv
     *
     * @warning
     * - The number of item in the gui section must be <b> EQUAL </b> to the registry section.
     * - The order of the menu item in each section (gui and registry) must be the same.\n
     *   For example: the menu item named "My item 1" will be connected with the service which have the sid = "item1".
     * - A menu can't have the same service connected on two different menu items.
     *
     * @see ::fwGui::registrar::MenuRegistrar::initialize(), ::fwGui::layoutManager::IMenuLayoutManager::initialize()
     */
    FWGUI_API void initialize();

    /// Create the layout and start the managed services.
    FWGUI_API void create();

    /// Destroy the layout and stop the managed services.
    FWGUI_API void destroy();

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );

    ::fwGui::layoutManager::IMenuLayoutManager::sptr m_layoutManager;
    ::fwGui::registrar::MenuRegistrar::sptr m_registrar;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hideActions;
};

} // namespace fwGui
