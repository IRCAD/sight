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
#include "ui/base/container/fwMenu.hpp"
#include "ui/base/container/fwMenuBar.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <core/runtime/ConfigurationElement.hpp>

namespace sight::ui::base
{

namespace registry
{

/**
 * @brief   Defines the menuBar registry for IHM.
 */
class UI_BASE_CLASS_API MenuBar : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(MenuBar, ui::base::GuiBaseObject);

    //------------------------------------------------------------------------------

    static sptr New(const std::string& sid)
    {
        return std::make_shared<MenuBar>(sid);
    }

    /// Constructor.
    UI_BASE_API MenuBar(const std::string& sid);

    /// Destructor. Do nothing
    UI_BASE_API virtual ~MenuBar();

    /// Return the parent container
    UI_BASE_API virtual ui::base::container::fwMenuBar::sptr getParent();

    /**
     * @brief Return the fwMenu associated with the menuSid.
     * @param menuSid sid of the menu service
     * @param menus  vector containing the fwMenu manages by this registry.
     */
    UI_BASE_API virtual ui::base::container::fwMenu::sptr getFwMenu(
        std::string menuSid,
        std::vector<ui::base::container::fwMenu::sptr> menus
    );

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="ui::base::IMenuBar" impl="::sight::module::ui::base::SMenuBar"
     * autoConnect="false"
     *>
          <gui>
              <layout>
                  <menu name="My Menu"/>
                  <menu name="My Menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" start="true" />
              <menu sid="myMenu2" start="true" />
          </registry>
       </service>
       @endcode
     * This method analyzes the registry section of the configuration.
     *  - \<menu sid="myMenu" start="true" /\> : define the service of the menu to add in the menu bar.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {true| false} (default value false): indicate if the service must be started by the menu bar
     * service.
     */

    UI_BASE_API virtual void initialize(core::runtime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting menu bar registry.
     * All services managed in local menus
     * and with start="true" in configuration will be started.
     * @pre MenuBar must be initialized before.
     * @pre sub menus must be instanced before.
     */
    UI_BASE_API virtual void manage(std::vector<ui::base::container::fwMenu::sptr> menus);

    /**
     * @brief Stopping view manager.
     * All services managed in local menus will be stopped.
     */
    UI_BASE_API virtual void unmanage();

protected:

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map<std::string, std::pair<unsigned int, bool> > SIDMenuMapType;

    /**
     * @brief All menu services ID managed and associated with pair containing:
     * menu's index vector and boolean describing if is started by the manager.
     */
    SIDMenuMapType m_menuSids;

    /// Main service ID associate with this MenuBar
    std::string m_sid;
};

} // namespace registry

} // namespace sight::ui::base
