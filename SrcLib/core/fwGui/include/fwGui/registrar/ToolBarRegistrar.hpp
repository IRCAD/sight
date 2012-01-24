/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_REGISTRAR_TOOLBARREGISTRYMANAGER_HPP_
#define _FWGUI_REGISTRAR_TOOLBARREGISTRYMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/container/fwToolBar.hpp"
#include "fwGui/container/fwMenu.hpp"
#include "fwGui/container/fwMenuItem.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/ActionCallbackBase.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{
namespace registrar
{

/**
 * @brief   Defines the toolBar registrar for IHM.
 * @class   ToolBarRegistrar.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ToolBarRegistrar : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ToolBarRegistrar)(::fwCore::BaseObject), (( (const std::string) )), new ToolBarRegistrar );
    typedef std::vector< ::fwGui::IMenuItemCallback::sptr > CallbacksType;

    /// Constructor.
    FWGUI_API ToolBarRegistrar( const std::string sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ToolBarRegistrar();

    /// Return the parent container
    FWGUI_API virtual ::fwGui::container::fwToolBar::sptr getParent();

    /**
     * @brief Return the fwMenuItem associated with the toolBarSid.
     * @param actionSid sid of the action service
     * @param toolBarItems  vector containing the fwMenuItem manages by this registrar.
     */
    FWGUI_API virtual ::fwGui::container::fwMenuItem::sptr getFwMenuItem(std::string toolBarSid, std::vector< ::fwGui::container::fwMenuItem::sptr > menuItems);

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @verbatim
           <service uid="toolbar2" type="::fwGui::IToolBarSrv" implementation="::gui::aspect::DefaultToolBarSrv" autoComChannel="no" >
               <gui>
                   <layout>
                       <menuItem name="My item 2" style="radio" icon="Bundles/TutoGui_0-1/icons/system.png"/>
                       <menuItem name="My item 3" style="radio" icon="Bundles/TutoGui_0-1/icons/system.png"/>
                       <separator />
                       <menuItem name="My item A" style="radio" icon="Bundles/TutoGui_0-1/icons/monkey.png"/>
                       <menuItem name="My item B" style="radio" icon="Bundles/TutoGui_0-1/icons/monkey.png"/>
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
       @endverbatim
     * This method analyzes the registry section of the configuration.
     *
     *  - <menuItem sid="item2" start="no" /> : define the service of the menuItem to add in the toolbar.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the service must be started by the toolbar service.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief manages action service associated with menuItem of toolbar.
     *
     * Register the menuItem containers for the associated services. Start the services if start=yes.
     *
     * If the action manages by the menuItems has its attribute executable="false", the associated menuItems will be disabled.
     * In other words, the start value is take into account only when executable attribute is set to true in managed action
     *
     * @warning If the action is present in a toolbar and a menu it must be started only one time.
     * @see ::fwGui::registrar::MenuRegistrar for more information on interaction between menubar and toolbar.
     *
     * @pre ToolBarRegistrar must be initialized before.
     * @pre sub toolBar items must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::container::fwMenuItem::sptr > toolBarItems );

    /**
     * @brief manages menu service associated with item of toolbar.
     *
     * Register the menu containers for the associated services. Start the services if start=yes.
     *
     * @pre ToolBarRegistrar must be initialized before.
     * @pre sub toolBar items must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::container::fwMenu::sptr > toolBarItems );

    /**
     * @brief manages editor service associated with item of toolbar.
     *
     * Register the containers for the associated services. Start the services if start=yes.
     *
     * @pre ToolBarRegistrar must be initialized before.
     * @pre sub toolBar items must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::container::fwContainer::sptr > toolBarItems );

    /**
     * @brief Stopping toolBar items manager.
     * All services managed in local toolBar items will be stopped.
     */
    FWGUI_API virtual void unmanage();

    /**
     * @brief This method is called when an action is clicked.
     */
    FWGUI_API virtual void onItemAction();

    /**
     * @brief Returns callbacks associate with toolBar items.
     */
    FWGUI_API virtual CallbacksType getCallbacks(){ return m_callbacks;};

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<unsigned int, bool> > SIDToolBarMapType;

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

    /// Main service ID associate with this ToolBarRegistrar
    std::string m_sid;

    /// Callback associate with toolBar items
    CallbacksType m_callbacks;
};

} // namespace registrar
} // namespace fwGui

#endif /*_FWGUI_REGISTRAR_TOOLBARREGISTRYMANAGER_HPP_*/


