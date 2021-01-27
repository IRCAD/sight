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

#include "module_gui/config.hpp"

#include <gui/IMenuBarSrv.hpp>

namespace sight::modules::gui
{
namespace aspect
{

/**
 * @brief   Defines the default menubar for standard application
 */
class MODULE_GUI_CLASS_API SDefaultMenuBar : public ::sight::gui::IMenuBarSrv
{

public:

    fwCoreServiceMacro(SDefaultMenuBar, ::sight::gui::IMenuBarSrv)

    /// Constructor. Do nothing.
    MODULE_GUI_API SDefaultMenuBar() noexcept;

    /// Destructor. Do nothing.
    MODULE_GUI_API virtual ~SDefaultMenuBar() noexcept;

protected:

    /** @name Service methods ( override from services::IService )
     * @{
     */

    /**
     * @brief Configuring method allows to configure an application with menu bar.
     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="::gui::IMenuBarSrv" impl="::modules::gui::aspect::SDefaultMenuBar" autoConnect="no"
     *>
          <gui>
              <layout>
                  <menu name="My Menu"/>
                  <menu name="My Menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" start="yes" />
              <menu sid="myMenu2" start="yes" />
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
     *   For example: the menu named "My Menu" will be connected with the service which have the sid = "myMenu".
     * - A menu bar can't have the same service connected on two different menu.
     *
     *  @see gui::IMenuBarSrv::initialize(), gui::layoutManager::IMenuBarLayoutManager::initialize()
     */
    MODULE_GUI_API virtual void configuring() override;

    /**
     * @brief Create the menus and start the managed services.
     * @see gui::IMenuBarSrv::create()
     */
    MODULE_GUI_API virtual void starting() override;

    /**
     * @brief Stop all menu of this menuBar and destroy MenuBar
     * @see gui::IMenuBarSrv::destroy()
     */
    MODULE_GUI_API virtual void stopping() override;

    /// Updating service, do nothing.
    MODULE_GUI_API virtual void updating() override;

    ///@}
};

}

}
