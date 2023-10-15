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

/**
 * @file ui/__/layout/menu_manager.hpp
 * @brief This file defines the interface of the base class for managing a menu.
 */

#pragma once

#include "ui/__/config.hpp"
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
class UI_CLASS_API menu_manager : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menu_manager, ui::object);

    typedef std::string RegistryKeyType;
    typedef std::vector<ui::menu_item_callback::sptr> CallbacksType;

    typedef enum
    {
        DEFAULT,
        QUIT,
        ABOUT,
        HELP,
        NEW,
        SEPARATOR
    } ActionType;

    class ActionInfo
    {
    public:

        ActionInfo() :

            m_icon("")
        {
        }

        std::string m_name;
        std::string m_shortcut;
        std::filesystem::path m_icon;
        bool m_isCheckable {false};
        bool m_isRadio {false};
        ActionType m_type {DEFAULT};
        bool m_isSeparator {false};
        bool m_isMenu {false};
    };

    UI_API const static RegistryKeyType REGISTRY_KEY;

    /// Destructor. Do nothing.
    UI_API ~menu_manager() override = default;

    /**
     * @brief Returns the vector of menu_item managed by this layout.
     */
    UI_API virtual std::vector<ui::container::menu_item::sptr> getMenuItems();

    /**
     * @brief Returns the vector of menu managed by this layout.
     */
    UI_API virtual std::vector<ui::container::menu::sptr> getMenus();

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
    UI_API virtual void initialize(const ui::config_t& configuration);

    /**
     * @brief Instantiate actions with parent menu.
     */
    UI_API virtual void createLayout(ui::container::menu::sptr parent, const std::string& id) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    UI_API virtual void destroyLayout() = 0;

    /**
     * @brief Set the action visibility.
     */
    UI_API virtual void menuItemSetVisible(ui::container::menu_item::sptr, bool isVisible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    UI_API virtual void menuItemSetEnabled(ui::container::menu_item::sptr, bool isEnabled) = 0;

    /**
     * @brief Set the action checked or not.
     */
    UI_API virtual void menuItemSetChecked(ui::container::menu_item::sptr, bool isChecked) = 0;

    /**
     * @brief Sets callbacks associate with menu items.
     */
    virtual void setCallbacks(CallbacksType callbacks)
    {
        m_callbacks = callbacks;
    }

protected:

    /**
     * @brief Helper to destroy local actions.
     */
    UI_API virtual void destroyActions();

    /// All actions managed by this layout.
    std::vector<ui::container::menu_item::sptr> m_menuItems;

    /// All menus managed by this layout.
    std::vector<ui::container::menu::sptr> m_menus;

    /// Save action informations from configuration.
    std::vector<ActionInfo> m_actionInfo;

    /// Callbacks associate with menu items
    CallbacksType m_callbacks;
};

} // namespace sight::ui::layout
