/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_ASPECT_DEFAULTMENUSRV_HPP_
#define GUI_ASPECT_DEFAULTMENUSRV_HPP_

#include <fwGui/IMenuSrv.hpp>

#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief   Defines the default menu for standard application
 * @class   DefaultMenuSrv.
 * @author  IRCAD (Research and Development Team)

 * @date    2009.
 */
class GUI_CLASS_API DefaultMenuSrv : public ::fwGui::IMenuSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (DefaultMenuSrv)(::fwGui::IMenuSrv) ) ;

    /// Constructor. Do nothing.
    GUI_API DefaultMenuSrv() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~DefaultMenuSrv() throw();

protected :

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Configuring method allows to configure a menu with several actions.
     * Here a sample of the DefaultMenuSrv service declaration with three actions:
     * @verbatim
            <service name="File" uid="menu_File" type="::fwGui::IMenuSrv" implementation="::gui::aspect::DefaultMenuSrv" autoComChannel="no">
                <layout>
                    <menuItem name="Load file" shortcut="Ctrl+O"/>
                    <menuItem uid="Save file"/>
                    <separator />
                    <menuItem uid="Quit" />
                </layout>
                <registrar>
                    <menuItem sid="action_loadFile"/>
                    <menuItem sid="action_saveFile"/>
                    <menuItem sid="action_quit" />
                </registrar>
            </service>
       @endverbatim
      * - <separator /> : allows to put a separator in the menu
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

    GUI_API virtual void starting() throw( ::fwTools::Failed ) ;

    /**
     * @brief Stop all actions and separator of this menu and remove menu to menuBar
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

#endif // GUI_ASPECT_DEFAULTMENUSRV_HPP_
