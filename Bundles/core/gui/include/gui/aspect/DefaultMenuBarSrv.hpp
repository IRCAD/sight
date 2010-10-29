/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_ASPECT_DEFAULTMENUBARSRV_HPP_
#define GUI_ASPECT_DEFAULTMENUBARSRV_HPP_

#include <fwGui/IMenuBarSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace aspect
{

/**
 * @brief   Defines the default menubar for standard application
 * @class   DefaultMenuBarSrv.
 * @author  IRCAD (Research and Development Team)

 * @date    2009-2010.
 */
class GUI_CLASS_API DefaultMenuBarSrv : public ::fwGui::IMenuBarSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (DefaultMenuBarSrv)(::fwGui::IMenuBarSrv) ) ;

    /// Constructor. Do nothing.
    GUI_API DefaultMenuBarSrv() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~DefaultMenuBarSrv() throw();

protected :

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Configuring method allows to configure an application with menu bar.
     * Here a sample of the DefaultMenuBarSrv service declaration with two menus:
     * @verbatim
        <service uid="myMenuBar" type="::fwGui::IMenuBarSrv" implementation="::gui::aspect::DefaultMenuBarSrv" autoComChannel="no">
            <layout>
                <menu name="File"/>
                <menu name="Mesher" />
            </layout>
            <registrar>
                <menu sid="menu_File" strart="yes"/>
                <menu sid="menu_Mesher" strart="yes"/>
            </registrar>
        </service>
       @endverbatim
      * - <layout> defines the layout configuration : name of the menus.
      * - <registrar> defines the services configuration :
      *   - sid of the menu services
      *   - start or not the menu service automatically
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

    GUI_API virtual void starting() throw( ::fwTools::Failed ) ;

    /**
     * @brief Stop all actions and separator of this menu and remove menu to wxMenuBar
     */
    GUI_API virtual void stopping() throw( ::fwTools::Failed ) ;

    /// Updating service, do nothing.
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /// Updating service on notification, do nothing.
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
    ///@}
};

}

}

#endif // GUI_ASPECT_DEFAULTMENUBARSRV_HPP_
