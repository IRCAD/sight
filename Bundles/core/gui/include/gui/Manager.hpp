/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_MANAGER_HPP_
#define _GUI_MANAGER_HPP_

#include <vector>

#include <wx/aui/aui.h>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>
#include <fwServices/IService.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "gui/export.hpp"
#include "gui/aspect/IAspect.hpp"
#include "gui/action/IAction.hpp"
#include "gui/view/IView.hpp"

namespace gui
{

/**
 * @brief   Implements the controller
 * @note it is equivalent to a controller between application (basicDocument) and specific visualization bundle.
 * @class   Manager.
 * @author  IRCAD (Research and Development Team)

 * @date    2009.
 */
class GUI_CLASS_API Manager : public ::fwCore::BaseObject
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((Manager)(::fwCore::BaseObject::Baseclass), (( )) , new Manager );

    /**
     * @brief   Retrieves the singleton basicVisuCtrl instance.
     * @return  a shared pointer to the basicVisuCtrl instance
     */
    GUI_API static const ::gui::Manager::sptr getDefault() throw();

    /**
     * @brief   destructor
     */
    GUI_API virtual ~Manager() throw() ;

    /**
     * @brief set application top window and its menu handler, initialize general gui aspect and initialize application root object (and service from configuration)
     */
    GUI_API static void initialize() ;

    /**
     * @brief invoke by Destroy of top window : clear root object (uninstall all services), and clear the aui manager then
     */
    GUI_API static void uninitialize() ;

    /**
     * @brief Associates an id with a service callBack
     * @note the callBack is an IAction having installed a new menu item in the application. This action is typically provided by a starting service wanting to have a entry from app menu
     * @note the map id to IAction is used by the menu handler (placed on the app top window) to indirectly trigger the action
     */
    GUI_API static void registerAction( ::gui::action::IAction::sptr _action ) ;

    /**
     * @brief Remove id to service callBack association
     */
    GUI_API static void unregisterAction( ::gui::action::IAction::sptr _action ) ;


    GUI_API static wxAuiManager *getTopAuiManager();

    /**
     * @brief Invoked by the menu event handler observing user actions on application menu
     */
    static void processAction(int id ) ;

    static void registerShortcuts();

private:

    /**
     * @brief   a shared pointer to the kernel instance
     */
    static ::gui::Manager::sptr m_instance;

    /**
     * @brief Ctor
     */
    Manager() throw();

    typedef std::map< int , ::gui::action::IAction::wptr > ActionContainer;

    /**
     * @brief by menu handler to trigger update on associated service
     */
    ActionContainer m_serviceCallBacks;
    std::vector< wxAcceleratorEntry > m_vAccelEntries;

    /**
     * @brief aui manager used to manage integration of subframes as children of the application top window
     */
    wxAuiManager * m_wxmanager ;

};

} // namespace gui

#endif //_GUI_MANAGER_HPP_
