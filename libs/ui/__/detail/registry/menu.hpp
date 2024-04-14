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

#include <sight/ui/__/config.hpp>

#include "ui/__/action_callback_base.hpp"
#include "ui/__/container/menu.hpp"
#include "ui/__/container/menu_item.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::detail::registry
{

/**
 * @brief   Defines the menu registry for IHM.
 */
class menu : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menu, ui::object);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& _sid)
    {
        return std::make_shared<menu>(_sid);
    }

    using callbacks_t = std::vector<ui::menu_item_callback::sptr>;

    /// Constructor.
    menu(std::string _sid);

    /// Destructor. Does nothing
    ~menu() override = default;

    /// Return the parent container
    virtual ui::container::menu::sptr get_parent();

    /**
     * @brief Return the menu_item associated with the actionSid.
     * @param _action_sid sid of the action service
     * @param _menu_items  vector containing the menu_item manages by this registry.
     */
    virtual ui::container::menu_item::sptr get_menu_item(
        std::string _action_sid,
        std::vector<ui::container::menu_item::sptr> _menu_items
    );

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @code{.xml}
        <service uid="my_menu" type="ui::menu" impl="sight::module::ui::menu"
     * auto_connect="false" >
            <gui>
                <layout>
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
                <menuItem sid="item1" start="true" />
                <menuItem sid="item2" start="false" />
                <menuItem sid="item3" start="false" />
                <menu sid="my_menu" start="true" />
                <menuItem sid="actionQuit" start="false" />
            </registry>
        </service>
       @endcode
     * This method analyzes the registry section of the configuration.
     *  - \<menuItem sid="item1" start="false" /\> : define the service of the menuItem to add in the menu.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {true| false} (default value false): indicate if the service must be started by the menu service.
     */
    virtual void initialize(const ui::config_t&);

    /**
     * @brief manages action service associated with menuItem of menu.
     *
     * Register the menuItem containers for the associated services. Start the services if start=true.
     *
     * If a menuItem has attribute start="false", the associated action won't be started and the menuItem will be
     * disabled.
     * If a menuItem has attribute start="true", two possibilities: \n
     *  - the associated action has attribute enabled="false" then the menuItem will be disabled.\n
     *  - the associated action has attribute enabled="true" then the menuItem will be enabled.\n
     *
     * If an action manages by menuItems in a toolbar and/or in the menubar has its attribute enabled="false",
     * the associated menuItems will be disabled in toolbar and in menubar.
     *
     * @warning If the action is present in different toolbars and menus it must be started only one time.
     *
     * @pre menu must be initialized before.
     * @pre sub menu items must be instanced before.
     */
    virtual void manage(std::vector<ui::container::menu_item::sptr> _menu_items);

    /**
     * @brief manages menu service associated with menu of menu.
     *
     * If a menuItem has attribute start="false", the associated menu won't be started.
     * If a menuItem has attribute start="true", the associated menu will be started
     *
     * @pre menu must be initialized before.
     * @pre sub menu items must be instanced before.
     */
    virtual void manage(std::vector<ui::container::menu::sptr> _menus);

    /**
     * @brief Stopping menu items manager.
     * All services managed in local menu items will be stopped.
     */
    virtual void unmanage();

    /**
     * @brief This method is called when an action is clicked.
     */
    virtual void on_item_action();

    /**
     * @brief Returns callbacks associate with menu items.
     */
    virtual callbacks_t get_callbacks()
    {
        return m_callbacks;
    }

protected:

    using sid_menu_map_t = std::map<std::string, std::pair<unsigned int, bool> >;

    /**
     * @brief All menu services ID (sid) managed is associated with pair containing:
     * an action's position in the menu and boolean describing if is started by the manager.
     */
    sid_menu_map_t m_action_sids;

    /**
     * @brief All toolbar services ID managed and associated with pair containing:
     * menu's index vector and boolean describing if is started by the manager.
     */
    sid_menu_map_t m_menu_sids;

    /// Main service ID associate with this menu
    std::string m_sid;

    /// Callback associate with menu items
    callbacks_t m_callbacks;
};

} // namespace sight::ui::detail::registry
