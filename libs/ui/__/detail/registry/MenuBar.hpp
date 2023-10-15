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
#include "ui/__/container/menu.hpp"
#include "ui/__/container/menubar.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::detail::registry
{

/**
 * @brief   Defines the menuBar registry for IHM.
 */
class MenuBar : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(MenuBar, ui::object);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& sid)
    {
        return std::make_shared<MenuBar>(sid);
    }

    /// Constructor.
    MenuBar(std::string sid);

    /// Destructor. Do nothing
    ~MenuBar() override = default;

    /// Return the parent container
    virtual ui::container::menubar::sptr getParent();

    /**
     * @brief Return the menu associated with the menuSid.
     * @param menuSid sid of the menu service
     * @param menus  vector containing the menu manages by this registry.
     */
    virtual ui::container::menu::sptr getFwMenu(
        std::string menuSid,
        std::vector<ui::container::menu::sptr> menus
    );

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="ui::menu_bar" impl="sight::module::ui::menu_bar"
     * auto_connect="false"
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

    virtual void initialize(const ui::config_t& configuration);

    /**
     * @brief Starting menu bar registry.
     * All services managed in local menus
     * and with start="true" in configuration will be started.
     * @pre MenuBar must be initialized before.
     * @pre sub menus must be instanced before.
     */
    virtual void manage(std::vector<ui::container::menu::sptr> menus);

    /**
     * @brief Stopping view manager.
     * All services managed in local menus will be stopped.
     */
    virtual void unmanage();

protected:

    typedef std::map<std::string, std::pair<unsigned int, bool> > SIDMenuMapType;

    /**
     * @brief All menu services ID managed and associated with pair containing:
     * menu's index vector and boolean describing if is started by the manager.
     */
    SIDMenuMapType m_menuSids;

    /// Main service ID associate with this MenuBar
    std::string m_sid;
};

} // namespace sight::ui::detail::registry
