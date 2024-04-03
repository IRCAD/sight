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
 * @brief Defines the interface of the base class for managing a toolbar.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="toolbar2" type="ui::toolbar" auto_connect="false">
       <gui>
           <layout style="ToolButtonIconOnly" uniformSize="true">
               <menuItem name="My item 2" style="check" icon="TutoGui/system.png" icon2="TutoGui/monkey.png" />
               <menuItem name="My item 3" style="check" icon="TutoGui/system.png"/>
               <separator />
               <menuItem name="My item A" style="radio" icon="TutoGui/monkey.png"/>
               <menuItem name="My item B" style="radio" icon="TutoGui/monkey.png"/>
               <separator />
               <menu name="My menu" />
               <separator />
               <editor />
               <accordion>
                    <menuItem name="Show/hide distance-related actions" icon="icons/Distance.svg" style="check" />
                    <menuItem name="Add/Edit distance" icon="icons/AddDistance.svg" style="check" />
                    <menuItem name="Remove distance" icon="icons/RemoveDistance.svg" />
                </accordion>
           </layout>
       </gui>
       <registry>
           <menuItem sid="item2" />
           <menuItem sid="item3" />
           <menuItem sid="item4" />
           <menuItem sid="item5" />
           <menu sid="menu" />
           <editor sid="editor" />
           <menuItem sid="distanceActionsVisibilityAct" start="true" />
           <menuItem sid="addDistanceAct" start="true" />
           <menuItem sid="removeDistanceAct" start="false" />
       </registry>
   </service>
   @endcode
 * @subsection Configuration Configuration
 *  - \<layout\> (mandatory) : give the list of the menu item that will appear in the toolbar.
 *   - \b style : (optional, default=ToolButtonIconOnly) describe the style of the tool button
 * (possible values: ToolButtonIconOnly/ToolButtonTextOnly/ToolButtonTextBesideIcon/ToolButtonTextUnderIcon
 * /ToolButtonFollowStyle )
 *   - \b uniformSize : (optional) set to true to uniform size of buttons to the wider one (default false).
 *  - \<menuItem name="My item 2" style="radio" icon="TutoGui/system.png"/\> :
 *   - \b name (mandatory) : give the name of the menu item that will appear in the interface.
 *   - \b style {check|radio} : give the style of the menu item.
 *   - \b icon : give the path of the icon file
 *   - \b icon2 : give the path of the icon file used when the item is checked
 *  - \<menu name="My menu" /\> :
 *   - \b name (mandatory) : give the name of the menu that will appear in the interface.
 *   - \b icon : give the path of the icon file
 *  - \<editor\> : to add an editor in the toolbar
 *  - \<separator/\> : allow to divide the toolbar by part (draw a line).
 *  - \<accordion\> : allows to make some buttons collapsable. The first button of the list will be the button that will
 *                    make the other buttons in the list appear if checked. The style of the first button must be
 *                    "check".
 */

#pragma once

#include <sight/ui/__/config.hpp>

#include "ui/__/container/menu.hpp"
#include "ui/__/container/menu_item.hpp"
#include "ui/__/container/toolbar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/menu_item_callback.hpp"
#include "ui/__/object.hpp"

#include <filesystem>

namespace sight::ui::layout
{

/**
 * @brief   Defines the toolbar layout manager for IHM.
 */
class SIGHT_UI_CLASS_API toolbar_manager : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(toolbar_manager, ui::object);

    using registry_key_t = std::string;
    using callbacks_t    = std::vector<ui::menu_item_callback::sptr>;

    enum class accordion
    {
        no,
        first,
        yes
    };

    class action_info
    {
    public:

        action_info() :

            m_icon(""),
            m_icon2("")
        {
        }

        std::string m_name;
        std::vector<std::string> m_shortcuts;
        std::filesystem::path m_icon;
        std::filesystem::path m_icon2;
        bool m_is_checkable {false};
        bool m_is_radio {false};
        bool m_is_separator {false};
        bool m_is_spacer {false};
        bool m_is_menu {false};
        bool m_is_editor {false};
        accordion m_accordion {accordion::no};
        int m_size {0};
    };

    SIGHT_UI_API const static registry_key_t REGISTRY_KEY;

    /// Constructor. Do nothing.
    SIGHT_UI_API toolbar_manager() = default;

    /// Destructor. Do nothing.
    SIGHT_UI_API ~toolbar_manager() override = default;

    /**
     * @brief Returns the vector of menu_item managed by this layout.
     */
    SIGHT_UI_API virtual std::vector<ui::container::menu_item::sptr> get_menu_items();

    /**
     * @brief Returns the vector of menu managed by this layout.
     */
    SIGHT_UI_API virtual std::vector<ui::container::menu::sptr> get_menus();

    /**
     * @brief Returns the vector of widget managed by this layout.
     */
    SIGHT_UI_API virtual std::vector<ui::container::widget::sptr> get_containers();

    /**
     * @brief Initialize layout managers & parse configuration.
     */
    SIGHT_UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Instantiate actions with parent toolbar.
     */
    SIGHT_UI_API virtual void create_layout(ui::container::toolbar::sptr _parent, const std::string& _id) = 0;

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
     * @brief Set the toolbar visibility.
     */
    SIGHT_UI_API virtual void set_visible(bool _is_visible) = 0;

    /**
     * @brief Sets callbacks associate with toolbar items.
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

    /// All editors managed by this layout.
    std::vector<ui::container::widget::sptr> m_containers;

    /// Save action informations from configuration.
    std::vector<action_info> m_action_info;

    /// Callbacks associate with toolbar items
    callbacks_t m_callbacks;

    /// String to describe the tool button style
    std::string m_style {"ToolButtonIconOnly"};

    ///If set to true (uniformSize="true"), button will be expended to match the wider one.
    bool m_unify_button_size {true};
};

} // namespace sight::ui::layout
