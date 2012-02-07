/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_REGISTRAR_MENUREGISTRYMANAGER_HPP_
#define _FWGUI_REGISTRAR_MENUREGISTRYMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/container/fwMenu.hpp"
#include "fwGui/container/fwMenuItem.hpp"
#include "fwGui/ActionCallbackBase.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{
namespace registrar
{

/**
 * @brief   Defines the menu registrar for IHM.
 * @class   MenuRegistrar.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API MenuRegistrar : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MenuRegistrar)(::fwCore::BaseObject), (( (const std::string) )), new MenuRegistrar );
    typedef std::vector< ::fwGui::IMenuItemCallback::sptr > CallbacksType;

    /// Constructor.
    FWGUI_API MenuRegistrar( const std::string sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~MenuRegistrar();

    /// Return the parent container
    FWGUI_API virtual ::fwGui::container::fwMenu::sptr getParent();

    /**
     * @brief Return the fwMenuItem associated with the actionSid.
     * @param actionSid sid of the action service
     * @param menuItems  vector containing the fwMenuItem manages by this registrar.
     */
    FWGUI_API virtual ::fwGui::container::fwMenuItem::sptr getFwMenuItem(std::string actionSid, std::vector< ::fwGui::container::fwMenuItem::sptr > menuItems);

    /**
     * @brief Initialize registry managers.
     *
     * Example of configuration
     * @verbatim
        <service uid="myMenu" type="::fwGui::IMenuSrv" implementation="::gui::aspect::DefaultMenuSrv" autoComChannel="no" >
            <gui>
                <layout>
                    <menuItem name="My item 1" shortcut="1" style="check" />
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
       @endverbatim
     * This method analyzes the registry section of the configuration.
     *  - <menuItem sid="item1" start="no" /> : define the service of the menuItem to add in the menu.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the service must be started by the menu service.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief manages action service associated with menuItem of menu.
     *
     * Register the menuItem containers for the associated services. Start the services if start=yes.
     *
     * If a menuItem has attribut start="no", the associated action won't be started and the menuItem will be disabled.
     * If a menuItem has attribut start="yes", two possibilities: \n
     *  - the associated action has attribut executable="false" then the menuItem will be disabled.\n
     *  - the associated action has attribut executable="true" then the menuItem will be enabled.\n
     *
     * If an action manages by menuItems in a toolbar and/or in the menuBar has its attribut executable="false",
     * the associated menuItems will be disabled in toolbar and in menuBar.
     *
     * @warning If the action is present in different toolbars and menus it must be started only one time.
     *
     * @pre MenuRegistrar must be initialized before.
     * @pre sub menu items must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::container::fwMenuItem::sptr > menuItems );

    /**
     * @brief manages menu service associated with fwMenu of menu.
     *
     * If a menuItem has attribut start="no", the associated menu won't be started.
     * If a menuItem has attribut start="yes", the associated menu will be started
     *
     * @pre MenuRegistrar must be initialized before.
     * @pre sub menu items must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::container::fwMenu::sptr > menus );

    /**
     * @brief Stopping menu items manager.
     * All services managed in local menu items will be stopped.
     */
    FWGUI_API virtual void unmanage();

    /**
     * @brief This method is called when an action is clicked.
     */
    FWGUI_API virtual void onItemAction();

    /**
     * @brief Returns callbacks associate with menu items.
     */
    FWGUI_API virtual CallbacksType getCallbacks(){ return m_callbacks;};

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<unsigned int, bool> > SIDMenuMapType;

    /**
     * @brief All menu services ID (sid) managed is associated with pair containing:
     * an action's position in the menu and boolean describing if is started by the manager.
     */
    SIDMenuMapType m_actionSids;

    /**
     * @brief All toolBar services ID managed and associated with pair containing:
     * menu's index vector and boolean describing if is started by the manager.
     */
    SIDMenuMapType m_menuSids;

    /// Main service ID associate with this MenuRegistrar
    std::string m_sid;

    /// Callback associate with menu items
    CallbacksType m_callbacks;
};

} // namespace registrar
} // namespace fwGui

#endif /*_FWGUI_REGISTRAR_MENUREGISTRYMANAGER_HPP_*/


