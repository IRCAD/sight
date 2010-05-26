/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_ASPECT_DEFAULT_MENUBAR_HPP_
#define GUI_ASPECT_DEFAULT_MENUBAR_HPP_

#include "gui/aspect/IMenuBar.hpp"
#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief   Defines the default menubar for standard application
 * @class   DefaultMenuBar.
 * @author  IRCAD (Research and Development Team)

 * @date    2009-2010.
 */
class GUI_CLASS_API DefaultMenuBar : public IMenuBar
{

public :

    fwCoreServiceClassDefinitionsMacro ( (DefaultMenuBar)(::gui::aspect::IMenuBar) ) ;

    /// Constructor. Do nothing.
    GUI_API DefaultMenuBar() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~DefaultMenuBar() throw();

protected :

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /// Retrieve DefaultMenuBar::m_menuName in configuration element
    /**
     * @brief Configuring method allows to configure an application with several tab views.
     * Here a sample of the DefaultMenuBar service declaration with two menus:
     * @verbatim
            <service uid="myMenuBar" type="::gui::aspect::IMenuBar" implementation="::gui::aspect::DefaultMenuBar" autoComChannel="no">
                <menus>
                    <menu uid="menu_File" />
                    <menu uid="menu_Edition" />
                </menus>
            </service>
       @endverbatim
      * <menus> node is mandatory even if there is only one <menu.../> item.
      * No specific attribute are available.
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
    /// Service vector of ::gui::aspect::IMenu UUID.  List of the menus declared in the aspect config.
    std::vector< std::string >  m_menusUID ;
};

}

}

#endif // GUI_ASPECT_DEFAULT_MENUBAR_HPP_
