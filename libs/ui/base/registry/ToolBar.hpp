/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/ActionCallbackBase.hpp"
#include "ui/base/config.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/container/fwMenu.hpp"
#include "ui/base/container/fwMenuItem.hpp"
#include "ui/base/container/fwToolBar.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base::registry
{

/**
 * @brief   Defines the toolBar registry for IHM.
 */
class UI_BASE_CLASS_API ToolBar : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(ToolBar, ui::base::GuiBaseObject);

    //------------------------------------------------------------------------------

    static sptr New(const std::string& sid)
    {
        return std::make_shared<ToolBar>(sid);
    }

    typedef std::vector<ui::base::IMenuItemCallback::sptr> CallbacksType;

    /// Constructor.
    UI_BASE_API ToolBar(std::string sid);

    /// Destructor. Do nothing
    UI_BASE_API ~ToolBar() override = default;

    /// Return the parent container
    UI_BASE_API virtual ui::base::container::fwToolBar::sptr getParent();

    /**
     * @brief Return the fwMenuItem associated with the actionSid.
     * @param actionSid sid of the action service
     * @param menuItems  vector containing the fwMenuItem manages by this registry.
     */
    UI_BASE_API virtual ui::base::container::fwMenuItem::sptr getFwMenuItem(
        std::string actionSid,
        std::vector<ui::base::container::fwMenuItem::sptr> menuItems
    );

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @code{.xml}
           <service uid="toolbar2" type="ui::base::IToolBar"
     * impl="sight::module::ui::base::SToolBar"
     * autoConnect="false" >
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
    UI_BASE_API virtual void initialize(const ui::base::config_t& configuration);

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
     * @see ui::base::registry::Menu for more information on interaction between menubar and toolbar.
     *
     * @pre ToolBar must be initialized before.
     * @pre sub toolBar items must be instanced before.
     */
    UI_BASE_API virtual void manage(std::vector<ui::base::container::fwMenuItem::sptr> toolBarItems);

    /**
     * @brief manages menu service associated with item of toolbar.
     *
     * Register the menu containers for the associated services. Start the services if start=true.
     *
     * @pre ToolBar must be initialized before.
     * @pre sub toolBar items must be instanced before.
     */
    UI_BASE_API virtual void manage(std::vector<ui::base::container::fwMenu::sptr> toolBarItems);

    /**
     * @brief manages editor service associated with item of toolbar.
     *
     * Register the containers for the associated services. Start the services if start=true.
     *
     * @pre ToolBar must be initialized before.
     * @pre sub toolBar items must be instanced before.
     */
    UI_BASE_API virtual void manage(std::vector<ui::base::container::fwContainer::sptr> toolBarItems);

    /**
     * @brief Stopping toolBar items manager.
     * All services managed in local toolBar items will be stopped.
     */
    UI_BASE_API virtual void unmanage();

    /**
     * @brief This method is called when an action is clicked.
     */
    UI_BASE_API virtual void onItemAction();

    /**
     * @brief Returns callbacks associate with toolBar items.
     */
    virtual CallbacksType getCallbacks()
    {
        return m_callbacks;
    }

protected:

    typedef std::map<std::string, std::pair<unsigned int, bool> > SIDToolBarMapType;
    typedef std::map<std::string, unsigned int> WIDToolBarMapType;

    /**
     * @brief All toolBar services ID managed and associated with pair containing:
     * action's index vector and boolean describing if is started by the manager.
     */
    SIDToolBarMapType m_actionSids;

    /**
     * @brief All toolBar services ID managed and associated with pair containing:
     * menus index vector and boolean describing if is started by the manager.
     */
    SIDToolBarMapType m_menuSids;

    /**
     * @brief All toolBar services ID managed and associated with pair containing:
     * editors index vector and boolean describing if is started by the manager.
     */
    SIDToolBarMapType m_editorSids;

    /// All toolBar servicesID managed (and associated with subViews index vector).
    WIDToolBarMapType m_editorWids;

    /// Main service ID associate with this ToolBar
    std::string m_sid;

    /// Callback associate with toolBar items
    CallbacksType m_callbacks;
};

} // namespace sight::ui::base::registry
