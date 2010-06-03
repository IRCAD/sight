/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_ASPECT_DEFAULT_MENU_HPP_
#define GUI_ASPECT_DEFAULT_MENU_HPP_

#include <set>

#include <wx/menuitem.h>

#include "gui/aspect/IMenu.hpp"
#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief   Defines the default menu for standard application
 * @class   DefaultMenu.
 * @author  IRCAD (Research and Development Team)

 * @date    2009.
 */
class GUI_CLASS_API DefaultMenu : public IMenu
{

public :

    fwCoreServiceClassDefinitionsMacro ( (DefaultMenu)(::gui::aspect::IMenu) ) ;

    /// Constructor. Do nothing.
    GUI_API DefaultMenu() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~DefaultMenu() throw();

protected :

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /// Retrieve DefaultMenu::m_menuName in configuration element
    /**
     * @brief Configuring method allows to configure a menu with several actions.
     * Here a sample of the DefaultMenu service declaration with three actions:
     * @verbatim
            <service name="File" uid="menu_File" type="::gui::aspect::IMenu" implementation="::gui::aspect::DefaultMenu" autoComChannel="no">
                <action uid="action_loadFile"/>
                <action uid="action_saveFile"/>
                <separator />
                <action uid="action_quit" />
            </service>
       @endverbatim
      * - <separator /> : allows to put a separatorin the menu
      * No specific attribute are available for action.
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

    GUI_API virtual void starting() throw( ::fwTools::Failed ) ;

    /**
     * @brief Stop all actions and separator of this menu and remove menu to wxMenuBar
     * @todo ACH : uncomment assert => all actions must be started
     * @todo Menu not empty on MAC with specials Actions like help, Quit...
     */
    GUI_API virtual void stopping() throw( ::fwTools::Failed ) ;

    /// Updating service, do nothing.
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /// Updating service on notification, do nothing.
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
    ///@}

private:

    std::vector< std::string >      m_actionsUID ;
    std::set< wxMenuItem * >        m_separators ;
    std::string                     m_menuName ;
    const static std::string        SEPARATOR_UID;
};

}

}

#endif // GUI_ASPECT_DEFAULT_MENU_HPP_
