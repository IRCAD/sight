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
 * @file gui/layoutManager/IToolBarLayoutManager.hpp
 * @brief This file defines the interface of the base class for managing a toolbar.
 */

#pragma once

#include "gui/config.hpp"
#include "gui/container/fwContainer.hpp"
#include "gui/container/fwMenu.hpp"
#include "gui/container/fwMenuItem.hpp"
#include "gui/container/fwToolBar.hpp"
#include "gui/GuiBaseObject.hpp"
#include "gui/IMenuItemCallback.hpp"

#include <core/runtime/ConfigurationElement.hpp>

#include <filesystem>

namespace sight::gui
{
namespace layoutManager
{

/**
 * @brief   Defines the toolBar layout manager for IHM.
 */
class GUI_CLASS_API IToolBarLayoutManager : public gui::GuiBaseObject
{
public:
    fwCoreClassMacro(IToolBarLayoutManager, gui::GuiBaseObject)

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;
    typedef std::vector< gui::IMenuItemCallback::sptr > CallbacksType;

    class ActionInfo
    {
    public:

        ActionInfo() :
            m_name(""),
            m_shortcut(""),
            m_icon(""),
            m_icon2(""),
            m_isCheckable(false),
            m_isRadio(false),
            m_isSeparator(false),
            m_isSpacer(false),
            m_isMenu(false),
            m_isEditor(false),
            m_size(0)
        {
        }

        std::string m_name;
        std::string m_shortcut;
        std::filesystem::path m_icon;
        std::filesystem::path m_icon2;
        bool m_isCheckable;
        bool m_isRadio;
        bool m_isSeparator;
        bool m_isSpacer;
        bool m_isMenu;
        bool m_isEditor;
        int m_size;
    };

    GUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    GUI_API IToolBarLayoutManager();

    /// Destructor. Do nothing.
    GUI_API virtual ~IToolBarLayoutManager();

    /**
     * @brief Returns the vector of fwMenuItem managed by this layout.
     */
    GUI_API virtual std::vector< gui::container::fwMenuItem::sptr > getMenuItems();

    /**
     * @brief Returns the vector of fwMenu managed by this layout.
     */
    GUI_API virtual std::vector< gui::container::fwMenu::sptr > getMenus();

    /**
     * @brief Returns the vector of fwContainer managed by this layout.
     */
    GUI_API virtual std::vector< gui::container::fwContainer::sptr > getContainers();

    /**
     * @brief Initialize layout managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="::gui::IToolBarSrv" impl="::modules::gui::aspect::SDefaultToolBar" autoConnect="no"
     *>
           <gui>
               <layout style="ToolButtonIconOnly">
                   <menuItem name="My item 2" style="check" icon="TutoGui-0.1/icons/system.png"
     * icon2="TutoGui-0.1/icons/monkey.png" />
                   <menuItem name="My item 3" style="check" icon="TutoGui-0.1/icons/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <separator />
                   <menu name="My menu" />
                   <separator />
                   <editor />
               </layout>
           </gui>
           <registry>
               <menuItem sid="item2" />
               <menuItem sid="item3" />
               <menuItem sid="item4" />
               <menuItem sid="item5" />
               <menu sid="menu" />
               <editor sid="editor" />
           </registry>
       </service>
       @endcode
     * This method analyzes the gui section of the configuration.
     *
     *  - \<layout\> (mandatory) : give the list of the menu item that will appear in the toolbar.
     *   - \b style : (optional, default=ToolButtonIconOnly) describe the style of the tool button
     * (possible values: ToolButtonIconOnly/ToolButtonTextOnly/ToolButtonTextBesideIcon/ToolButtonTextUnderIcon
     * /ToolButtonFollowStyle )
     *  - \<menuItem name="My item 2" style="radio" icon="TutoGui-0.1/icons/system.png"/\> :
     *   - \b name (mandatory) : give the name of the menu item that will appear in the interface.
     *   - \b style {check|radio} : give the style of the menu item.
     *   - \b icon : give the path of the icon file
     *   - \b icon2 : give the path of the icon file used when the item is checked
     *  - \<menu name="My menu" /\> :
     *   - \b name (mandatory) : give the name of the menu that will appear in the interface.
     *   - \b icon : give the path of the icon file
     *  - \<editor\> : to add an editor in the toolbar
     *  - \<separator/\> : allow to divide the toolbar by part (draw a line).
     */
    GUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate actions with parent toolBar.
     */
    GUI_API virtual void createLayout( gui::container::fwToolBar::sptr parent) = 0;

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
     * @brief Sets callbacks associate with toolBar items.
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

    /// All editors managed by this layout.
    std::vector< gui::container::fwContainer::sptr > m_containers;

    /// Save action informations from configuration.
    std::vector< ActionInfo > m_actionInfo;

    /// Callbacks associate with toolBar items
    CallbacksType m_callbacks;

    /// String to describe the tool button style
    std::string m_style{"ToolButtonIconOnly"};
};

} // namespace layoutManager
} // namespace sight::gui
