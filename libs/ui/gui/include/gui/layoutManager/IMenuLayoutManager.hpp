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
 * @file gui/layoutManager/IMenuLayoutManager.hpp
 * @brief This file defines the interface of the base class for managing a menu.
 */

#pragma once

#include "gui/config.hpp"
#include "gui/container/fwMenu.hpp"
#include "gui/container/fwMenuItem.hpp"
#include "gui/GuiBaseObject.hpp"
#include "gui/IMenuItemCallback.hpp"

#include <core/runtime/ConfigurationElement.hpp>

#include <filesystem>

namespace sight::gui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu layout manager for IHM.
 */
class GUI_CLASS_API IMenuLayoutManager : public gui::GuiBaseObject
{
public:
    fwCoreClassMacro(IMenuLayoutManager, gui::GuiBaseObject)

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;
    typedef std::vector< gui::IMenuItemCallback::sptr > CallbacksType;

    typedef enum
    {
        DEFAULT,
        QUIT,
        ABOUT,
        HELP,
        NEW,
        SEPARATOR
    }ActionType;

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

    GUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    GUI_API IMenuLayoutManager();

    /// Destructor. Do nothing.
    GUI_API virtual ~IMenuLayoutManager();

    /**
     * @brief Returns the vector of fwMenuItem managed by this layout.
     */
    GUI_API virtual std::vector< gui::container::fwMenuItem::sptr > getMenuItems();

    /**
     * @brief Returns the vector of fwMenu managed by this layout.
     */
    GUI_API virtual std::vector< gui::container::fwMenu::sptr > getMenus();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="myMenu" type="::gui::IMenuSrv" impl="::modules::gui::aspect::SDefaultMenu" autoConnect="no" >
            <gui>
                <layout>
                    <menuItem name="My item 1" shortcut="1" style="check" icon="TutoGui-0.1/icons/system.png" />
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
                <menuItem sid="item1" start="yes" />
                <menuItem sid="item2" start="no" />
                <menuItem sid="item3" start="no" />
                <menu sid="mymenu" start="yes" />
                <menuItem sid="actionQuit" start="no" />
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
    GUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate actions with parent menu.
     */
    GUI_API virtual void createLayout( gui::container::fwMenu::sptr parent ) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    GUI_API virtual void destroyLayout() = 0;

    /**
     * @brief Set the action visibility.
     */
    GUI_API virtual void menuItemSetVisible(gui::container::fwMenuItem::sptr, bool isVisible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    GUI_API virtual void menuItemSetEnabled(gui::container::fwMenuItem::sptr, bool isEnabled) = 0;

    /**
     * @brief Set the action checked or not.
     */
    GUI_API virtual void menuItemSetChecked(gui::container::fwMenuItem::sptr, bool isChecked) = 0;

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
    GUI_API virtual void destroyActions();

    /// All actions managed by this layout.
    std::vector< gui::container::fwMenuItem::sptr > m_menuItems;

    /// All menus managed by this layout.
    std::vector< gui::container::fwMenu::sptr > m_menus;

    /// Save action informations from configuration.
    std::vector< ActionInfo > m_actionInfo;

    /// Callbacks associate with menu items
    CallbacksType m_callbacks;
};

} // namespace layoutManager
} // namespace sight::gui
