/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_REGISTRAR_TOOLBARREGISTRAR_HPP__
#define __FWGUI_REGISTRAR_TOOLBARREGISTRAR_HPP__

#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/GuiBaseObject.hpp"
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
 */
class FWGUI_CLASS_API ToolBarRegistrar : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ToolBarRegistrar)(::fwGui::GuiBaseObject), (( (const std::string&) )),
                                            new ToolBarRegistrar );
    typedef std::vector< ::fwGui::IMenuItemCallback::sptr > CallbacksType;

    /// Constructor.
    FWGUI_API ToolBarRegistrar( const std::string& sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ToolBarRegistrar();

    /// Return the parent container
    FWGUI_API virtual ::fwGui::container::fwToolBar::sptr getParent();

    /**
     * @brief Return the fwMenuItem associated with the actionSid.
     * @param actionSid sid of the action service
     * @param menuItems  vector containing the fwMenuItem manages by this registrar.
     */
    FWGUI_API virtual ::fwGui::container::fwMenuItem::sptr getFwMenuItem(std::string actionSid,
                                                                         std::vector< ::fwGui::container::fwMenuItem::sptr > menuItems);

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @code{.xml}
           <service uid="toolbar2" type="::fwGui::IToolBarSrv" impl="::gui::aspect::SDefaultToolBar" autoConnect="no" >
               <gui>
                   <layout>
                       <menuItem name="My item 2" style="radio" icon="@BUNDLE_PREFIX@/TutoGui_0-1/icons/system.png"/>
                       <menuItem name="My item 3" style="radio" icon="@BUNDLE_PREFIX@/TutoGui_0-1/icons/system.png"/>
                       <separator />
                       <menuItem name="My item A" style="radio" icon="@BUNDLE_PREFIX@/TutoGui_0-1/icons/monkey.png"/>
                       <menuItem name="My item B" style="radio" icon="@BUNDLE_PREFIX@/TutoGui_0-1/icons/monkey.png"/>
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
                   <editor sid="editor" start="yes"/>
                   <editor wid="editorWindow" />
               </registry>
           </service>
       @endcode
     * This method analyzes the registry section of the configuration.
     *
     * - \<menuItem sid="item2" start="no" /\> : define the service of the menuItem to add in the toolbar.
     *   - \b sid (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the service must be started by the toolbar service.
     * - \<editor sid="editor" start="yes" /\> : define the service of the editor to add in the toolbar.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the editor service must be started by the service.
     * - \<editor wid="editorWindow" /\> : reserve a sub container for the editor in the toolbar with the name
     *   "editorWindow". The service which want to use this sub container will have to define a parent
     *   with \<parent wid="editorWindow" /\>.
     *   - \b wid  (mandatory): the window identifier.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief manages action service associated with menuItem of toolbar.
     *
     * Register the menuItem containers for the associated services. Start the services if start=yes.
     *
     * If a menuItem has attribut start="no", the associated action won't be started and the menuItem will be disabled.
     * If a menuItem has attribut start="yes", two possibilities: \n
     *  - the associated action has attribut executable="false" then the menuItem will be disabled.\n
     *  - the associated action has attribut executable="true" then the menuItem will be enabled.\n
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
    virtual CallbacksType getCallbacks()
    {
        return m_callbacks;
    }

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<unsigned int, bool> > SIDToolBarMapType;
    typedef std::map< std::string, unsigned int > WIDToolBarMapType;

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

    /// Main service ID associate with this ToolBarRegistrar
    std::string m_sid;

    /// Callback associate with toolBar items
    CallbacksType m_callbacks;
};

} // namespace registrar
} // namespace fwGui

#endif /*__FWGUI_REGISTRAR_TOOLBARREGISTRAR_HPP__*/


