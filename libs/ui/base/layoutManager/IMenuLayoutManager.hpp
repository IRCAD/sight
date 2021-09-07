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

/**
 * @file ui/base/layoutManager/IMenuLayoutManager.hpp
 * @brief This file defines the interface of the base class for managing a menu.
 */

#pragma once

#include "ui/base/config.hpp"
#include "ui/base/container/fwMenu.hpp"
#include "ui/base/container/fwMenuItem.hpp"
#include "ui/base/GuiBaseObject.hpp"
#include "ui/base/IMenuItemCallback.hpp"

#include <core/runtime/ConfigurationElement.hpp>

#include <filesystem>

namespace sight::ui::base
{

namespace layoutManager
{

/**
 * @brief   Defines the menu layout manager for IHM.
 */
class UI_BASE_CLASS_API IMenuLayoutManager : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IMenuLayoutManager, ui::base::GuiBaseObject);

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;
    typedef std::vector<ui::base::IMenuItemCallback::sptr> CallbacksType;

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
            m_name(""),
            m_shortcut(""),
            m_icon(""),
            m_isCheckable(false),
            m_isRadio(false),
            m_type(DEFAULT),
            m_isSeparator(false),
            m_isMenu(false)
        {
        }

        std::string m_name;
        std::string m_shortcut;
        std::filesystem::path m_icon;
        bool m_isCheckable;
        bool m_isRadio;
        ActionType m_type;
        bool m_isSeparator;
        bool m_isMenu;
    };

    UI_BASE_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    UI_BASE_API IMenuLayoutManager();

    /// Destructor. Do nothing.
    UI_BASE_API virtual ~IMenuLayoutManager();

    /**
     * @brief Returns the vector of fwMenuItem managed by this layout.
     */
    UI_BASE_API virtual std::vector<ui::base::container::fwMenuItem::sptr> getMenuItems();

    /**
     * @brief Returns the vector of fwMenu managed by this layout.
     */
    UI_BASE_API virtual std::vector<ui::base::container::fwMenu::sptr> getMenus();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="myMenu" type="ui::base::IMenu" impl="::sight::module::ui::base::SMenu"
     * autoConnect="false" >
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
                <menu sid="mymenu" start="true" />
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
    UI_BASE_API virtual void initialize(ConfigurationType configuration);

    /**
     * @brief Instantiate actions with parent menu.
     */
    UI_BASE_API virtual void createLayout(ui::base::container::fwMenu::sptr parent) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    UI_BASE_API virtual void destroyLayout() = 0;

    /**
     * @brief Set the action visibility.
     */
    UI_BASE_API virtual void menuItemSetVisible(ui::base::container::fwMenuItem::sptr, bool isVisible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    UI_BASE_API virtual void menuItemSetEnabled(ui::base::container::fwMenuItem::sptr, bool isEnabled) = 0;

    /**
     * @brief Set the action checked or not.
     */
    UI_BASE_API virtual void menuItemSetChecked(ui::base::container::fwMenuItem::sptr, bool isChecked) = 0;

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
    UI_BASE_API virtual void destroyActions();

    /// All actions managed by this layout.
    std::vector<ui::base::container::fwMenuItem::sptr> m_menuItems;

    /// All menus managed by this layout.
    std::vector<ui::base::container::fwMenu::sptr> m_menus;

    /// Save action informations from configuration.
    std::vector<ActionInfo> m_actionInfo;

    /// Callbacks associate with menu items
    CallbacksType m_callbacks;
};

} // namespace layoutManager

} // namespace sight::ui::base
