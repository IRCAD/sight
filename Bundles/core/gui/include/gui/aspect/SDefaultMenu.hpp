/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __GUI_ASPECT_SDEFAULTMENU_HPP__
#define __GUI_ASPECT_SDEFAULTMENU_HPP__

#include "gui/config.hpp"

#include <fwGui/IMenuSrv.hpp>

namespace gui
{
namespace aspect
{

/**
 * @brief   Defines the default menu for standard application
 */
class GUI_CLASS_API SDefaultMenu : public ::fwGui::IMenuSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SDefaultMenu)(::fwGui::IMenuSrv) );

    /// Constructor. Do nothing.
    GUI_API SDefaultMenu() noexcept;

    /// Destructor. Do nothing.
    GUI_API virtual ~SDefaultMenu() noexcept;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Configuring method allows to configure a menu with several actions.

     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="::fwGui::IMenuBarSrv" impl="::gui::aspect::SDefaultMenuBar" autoConnect="no" >
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
     *    - <separator /> : allows to put a separator in the menu
     *  - \<registry\> \</registry\> : (mandatory) describe the service management.
     *
     * @warning
     * - The number of item in the gui section must be equal or greater than in the registry section.
     * - The order of the menu in each section (gui and registry) must be the same.\n
     *   For example: the menu named "My Menu" will be connected with the service which have the sid = "myMenu".
     * - A menu bar can't have the same service connected on two different menu.
     *
     *  @see ::fwGui::IMenuBarSrv::initialize(), ::fwGui::layoutManager::IMenuLayoutManager::initialize()
     */
    GUI_API virtual void configuring() override;

    /**
     * @brief Create the menu items and start the managed services.
     * @see ::fwGui::IMenuSrv::create()
     */
    GUI_API virtual void starting() override;

    /**
     * @brief Stop all actions and separator of this menu and remove menu to menuBar
     * @see ::fwGui::IMenuSrv::destroy()
     */
    GUI_API virtual void stopping() override;

    /// Updating service, do nothing.
    GUI_API virtual void updating() override;
    ///@}

};

}

}

#endif // __GUI_ASPECT_SDEFAULTMENU_HPP__
