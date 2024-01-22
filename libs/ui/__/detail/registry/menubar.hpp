/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
 * @brief   Defines the menubar registry for IHM.
 */
class menubar : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menubar, ui::object);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& _sid)
    {
        return std::make_shared<menubar>(_sid);
    }

    /// Constructor.
    menubar(std::string _sid);

    /// Destructor. Do nothing
    ~menubar() override = default;

    /// Return the parent container
    virtual ui::container::menubar::sptr get_parent();

    /**
     * @brief Return the menu associated with the menuSid.
     * @param _menu_sid sid of the menu service
     * @param _menus  vector containing the menu manages by this registry.
     */
    virtual ui::container::menu::sptr get_fw_menu(
        std::string _menu_sid,
        std::vector<ui::container::menu::sptr> _menus
    );

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="menubar" type="ui::menubar" impl="sight::module::ui::menubar"
     * auto_connect="false"
     *>
          <gui>
              <layout>
                  <menu name="My menu"/>
                  <menu name="My menu 2"/>
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

    virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Starting menu bar registry.
     * All services managed in local menus
     * and with start="true" in configuration will be started.
     * @pre menu_bar must be initialized before.
     * @pre sub menus must be instanced before.
     */
    virtual void manage(std::vector<ui::container::menu::sptr> _menus);

    /**
     * @brief Stopping view manager.
     * All services managed in local menus will be stopped.
     */
    virtual void unmanage();

protected:

    using sid_menu_map_t = std::map<std::string, std::pair<unsigned int, bool> >;

    /**
     * @brief All menu services ID managed and associated with pair containing:
     * menu's index vector and boolean describing if is started by the manager.
     */
    sid_menu_map_t m_menu_sids;

    /// Main service ID associate with this menu_bar
    std::string m_sid;
};

} // namespace sight::ui::detail::registry
