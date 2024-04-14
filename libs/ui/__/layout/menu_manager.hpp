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

/**
 * @file ui/__/layout/menu_manager.hpp
 * @brief This file defines the interface of the base class for managing a menu.
 */

#pragma once

#include <sight/ui/__/config.hpp>

#include "ui/__/container/menu.hpp"
#include "ui/__/container/menu_item.hpp"
#include "ui/__/menu_item_callback.hpp"
#include "ui/__/object.hpp"

#include <filesystem>

namespace sight::ui::layout
{

/**
 * @brief   Defines the menu layout manager for IHM.
 */
class SIGHT_UI_CLASS_API menu_manager : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menu_manager, ui::object);

    using registry_key_t = std::string;
    using callbacks_t    = std::vector<ui::menu_item_callback::sptr>;

    enum action_t
    {
        DEFAULT,
        quit,
        about,
        help,
        NEW,
        separator
    };

    class action_info
    {
    public:

        action_info() :

            m_icon("")
        {
        }

        std::string m_name;
        std::string m_shortcut;
        std::filesystem::path m_icon;
        bool m_is_checkable {false};
        bool m_is_radio {false};
        action_t m_type {DEFAULT};
        bool m_is_separator {false};
        bool m_is_menu {false};
    };

    SIGHT_UI_API const static registry_key_t REGISTRY_KEY;

    /// Destructor. Do nothing.
    SIGHT_UI_API ~menu_manager() override = default;

    /**
     * @brief Returns the vector of menu_item managed by this layout.
     */
    SIGHT_UI_API virtual std::vector<ui::container::menu_item::sptr> get_menu_items();

    /**
     * @brief Returns the vector of menu managed by this layout.
     */
    SIGHT_UI_API virtual std::vector<ui::container::menu::sptr> get_menus();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="myMenu" type="ui::menu" impl="sight::module::ui::menu"
     * auto_connect="false" >
            <gui>
                <layout>
                    <menuItem name="My item 1" shortcut="1" style="check" icon="TutoGui/system.png" />
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
                <menu sid="myMenu" start="true" />
                <menuItem sid="actionQuit" start="false" />
            </registry>
       </service>
       @endcode
     * This method analyzes the gui section of the configuration.
     * - \<layout\> (mandatory) : give the list of the menu item that will appear in the menu.
     * - \<menuItem name="My item 1" shortcut="1" style="check" specialAction="NEW" /\> :
     *  - \b name (mandatory) : give the name of the menu item that will appear in the interface.
     *  - \b shortcut : give the shortcut for this menu item.
     *  - \b style {check|radio} : give the style of the menu item.
     *  - \b specialAction {DEFAULT | QUIT | NEW | ABOUT | HELP}: specify a pre define action. If it isn't define the
     * value is DEFAULT.
     *   - \b icon : give the path of the icon file
     * - \<menu name="My menu" /\> :
     *  - \b name (mandatory) : give the name of the menu that will appear in the interface.
     * - \<separator/\> : allow to divide the menu by part (draw a line).
     */
    SIGHT_UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Instantiate actions with parent menu.
     */
    SIGHT_UI_API virtual void create_layout(ui::container::menu::sptr _parent, const std::string& _id) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    SIGHT_UI_API virtual void destroy_layout() = 0;

    /**
     * @brief Set the action visibility.
     */
    SIGHT_UI_API virtual void menu_item_set_visible(ui::container::menu_item::sptr, bool _is_visible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    SIGHT_UI_API virtual void menu_item_set_enabled(ui::container::menu_item::sptr, bool _is_enabled) = 0;

    /**
     * @brief Set the action checked or not.
     */
    SIGHT_UI_API virtual void menu_item_set_checked(ui::container::menu_item::sptr, bool _is_checked) = 0;

    /**
     * @brief Sets callbacks associate with menu items.
     */
    virtual void set_callbacks(callbacks_t _callbacks)
    {
        m_callbacks = _callbacks;
    }

protected:

    /**
     * @brief Helper to destroy local actions.
     */
    SIGHT_UI_API virtual void destroy_actions();

    /// All actions managed by this layout.
    std::vector<ui::container::menu_item::sptr> m_menu_items;

    /// All menus managed by this layout.
    std::vector<ui::container::menu::sptr> m_menus;

    /// Save action informations from configuration.
    std::vector<action_info> m_action_info;

    /// Callbacks associate with menu items
    callbacks_t m_callbacks;
};

} // namespace sight::ui::layout
