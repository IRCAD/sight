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

#include "ui/__/action_callback_base.hpp"
#include "ui/__/config.hpp"
#include "ui/__/container/menu.hpp"
#include "ui/__/container/menu_item.hpp"
#include "ui/__/container/toolbar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::detail::registry
{

/**
 * @brief   Defines the toolbar registry for IHM.
 */
class toolbar : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(toolbar, ui::object);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& _sid)
    {
        return std::make_shared<toolbar>(_sid);
    }

    using callbacks_t = std::vector<ui::menu_item_callback::sptr>;

    /// Constructor.
    toolbar(std::string _sid);

    /// Destructor. Do nothing
    ~toolbar() override = default;

    /// Return the parent container
    virtual ui::container::toolbar::sptr get_parent();

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
           <service uid="toolbar2" type="ui::toolbar"
     * impl="sight::module::ui::toolbar"
     * auto_connect="false" >
               <gui>
                   <layout>
                       <menuItem name="My item 2" style="radio" icon="TutoGui/system.png"/>
                       <menuItem name="My item 3" style="radio" icon="TutoGui/system.png"/>
                       <separator />
                       <menuItem name="My item A" style="radio" icon="TutoGui/monkey.png"/>
                       <menuItem name="My item B" style="radio" icon="TutoGui/monkey.png"/>
                       <separator />
                       <menu name="My menu" />
                       <separator />
                       <editor />
                       <editor />
                   </layout>
               </gui>
               <registry>
                   <menuItem sid="item2" />
                   <menuItem sid="item3" />
                   <menuItem sid="item4" />
                   <menuItem sid="item5" />
                   <menu sid="menu" />
                   <editor sid="editor" start="true"/>
                   <editor wid="editorWindow" />
               </registry>
           </service>
       @endcode
     * This method analyzes the registry section of the configuration.
     *
     * - \<menuItem sid="item2" start="false" /\> : define the service of the menuItem to add in the toolbar.
     *   - \b sid (mandatory): the service identifier.
     *   - \b start = {true| false} (default value false): indicate if the service must be started by the toolbar
     * service.
     * - \<editor sid="editor" start="true" /\> : define the service of the editor to add in the toolbar.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {true| false} (default value false): indicate if the editor service must be started by the
     * service.
     * - \<editor wid="editorWindow" /\> : reserve a sub container for the editor in the toolbar with the name
     *   "editorWindow". The service which want to use this sub container will have to define a parent
     *   with \<parent wid="editorWindow" /\>.
     *   - \b wid  (mandatory): the window identifier.
     */
    virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief manages action service associated with menuItem of toolbar.
     *
     * Register the menuItem containers for the associated services. Start the services if start=true.
     *
     * If a menuItem has attribute start="false", the associated action won't be started and the menuItem will be
     * disabled.
     * If a menuItem has attribute start="true", two possibilities: \n
     *  - the associated action has attribute enabled="false" then the menuItem will be disabled.\n
     *  - the associated action has attribute enabled="true" then the menuItem will be enabled.\n
     *
     * @warning If the action is present in a toolbar and a menu it must be started only one time.
     * @see ui::registry::menu for more information on interaction between menubar and toolbar.
     *
     * @pre tool_bar must be initialized before.
     * @pre sub toolbar items must be instanced before.
     */
    virtual void manage(std::vector<ui::container::menu_item::sptr> _menu_items);

    /**
     * @brief manages menu service associated with item of toolbar.
     *
     * Register the menu containers for the associated services. Start the services if start=true.
     *
     * @pre tool_bar must be initialized before.
     * @pre sub toolbar items must be instanced before.
     */
    virtual void manage(std::vector<ui::container::menu::sptr> _menus);

    /**
     * @brief manages editor service associated with item of toolbar.
     *
     * Register the containers for the associated services. Start the services if start=true.
     *
     * @pre tool_bar must be initialized before.
     * @pre sub toolbar items must be instanced before.
     */
    virtual void manage(std::vector<ui::container::widget::sptr> _containers);

    /**
     * @brief Stopping toolbar items manager.
     * All services managed in local toolbar items will be stopped.
     */
    virtual void unmanage();

    /**
     * @brief This method is called when an action is clicked.
     */
    virtual void on_item_action();

    /**
     * @brief Returns callbacks associate with toolbar items.
     */
    virtual callbacks_t get_callbacks()
    {
        return m_callbacks;
    }

protected:

    using sid_toolbar_map_t     = std::map<std::string, std::pair<unsigned int, bool> >;
    using wid_tool_bar_map_type = std::map<std::string, unsigned int>;

    /**
     * @brief All toolbar services ID managed and associated with pair containing:
     * action's index vector and boolean describing if is started by the manager.
     */
    sid_toolbar_map_t m_action_sids;

    /**
     * @brief All toolbar services ID managed and associated with pair containing:
     * menus index vector and boolean describing if is started by the manager.
     */
    sid_toolbar_map_t m_menu_sids;

    /**
     * @brief All toolbar services ID managed and associated with pair containing:
     * editors index vector and boolean describing if is started by the manager.
     */
    sid_toolbar_map_t m_editor_sids;

    /// All toolbar servicesID managed (and associated with subViews index vector).
    wid_tool_bar_map_type m_editor_wids;

    /// Main service ID associate with this tool_bar
    std::string m_sid;

    /// Callback associate with toolbar items
    callbacks_t m_callbacks;
};

} // namespace sight::ui::detail::registry
