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
#include "fwGui/container/fwMenu.hpp"
#include "fwGui/container/fwMenuBar.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace registrar
{

/**
 * @brief   Defines the menuBar registrar for IHM.
 */
class FWGUI_CLASS_API MenuBarRegistrar : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassMacro(MenuBarRegistrar, ::fwGui::GuiBaseObject);

    //------------------------------------------------------------------------------

    static sptr New(const std::string& sid)
    {
        return std::make_shared<MenuBarRegistrar>(sid);
    }

    /// Constructor.
    FWGUI_API MenuBarRegistrar( const std::string& sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~MenuBarRegistrar();

    /// Return the parent container
    FWGUI_API virtual ::fwGui::container::fwMenuBar::sptr getParent();

    /**
     * @brief Return the fwMenu associated with the menuSid.
     * @param menuSid sid of the menu service
     * @param menus  vector containing the fwMenu manages by this registrar.
     */
    FWGUI_API virtual ::fwGui::container::fwMenu::sptr getFwMenu(std::string menuSid,
                                                                 std::vector< ::fwGui::container::fwMenu::sptr > menus);

    /**
     * @brief Initialize registry managers.
     *
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
     * This method analyzes the registry section of the configuration.
     *  - \<menu sid="myMenu" start="yes" /\> : define the service of the menu to add in the menu bar.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the service must be started by the menu bar service.
     */

    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting menu bar registrar.
     * All services managed in local menus
     * and with start="yes" in configuration will be started.
     * @pre MenuBarRegistrar must be initialized before.
     * @pre sub menus must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::container::fwMenu::sptr > menus );

    /**
     * @brief Stopping view manager.
     * All services managed in local menus will be stopped.
     */
    FWGUI_API virtual void unmanage();

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<unsigned int, bool> > SIDMenuMapType;

    /**
     * @brief All menu services ID managed and associated with pair containing:
     * menu's index vector and boolean describing if is started by the manager.
     */
    SIDMenuMapType m_menuSids;

    /// Main service ID associate with this MenuBarRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui
