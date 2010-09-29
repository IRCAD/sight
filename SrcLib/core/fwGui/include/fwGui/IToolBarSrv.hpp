/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_ITOOLBARSRV_HPP_
#define _FWGUI_ITOOLBARSRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"

#include "fwGui/registrar/ToolBarRegistrar.hpp"
#include "fwGui/layoutManager/IToolBarLayoutManager.hpp"


namespace fwGui
{

/**
 * @brief   Defines the service interface managing the toolBar.
 * @class   IToolBarSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IToolBarSrv : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IToolBarSrv)(::fwServices::IService) ) ;

    /// Method called when an action service is stopping
    FWGUI_API void actionServiceStopping(std::string actionSrvSID);

    /// Method called when an action service is starting
    FWGUI_API void actionServiceStarting(std::string actionSrvSID);

    /// Method called when the action service is activated
    FWGUI_API void actionServiceSetActive(std::string actionSrvSID, bool isActive);

    /// Method called when the action service is executable
    FWGUI_API void actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable);

protected :
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IToolBarSrv() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IToolBarSrv() ;

    /**
     * @brief Initialize the layout and registry managers.
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
               </layout>
           </gui>
           <registry>
               <menuItem sid="item2" />
               <menuItem sid="item3" />
               <menuItem sid="item4" />
               <menuItem sid="item5" />
           </registry>
       </service>
       @endverbatim
     *  <gui> </gui> : (mandatory) describe the interface of the service.
     *
     *  <registry> </registry> : (mandatory) describe the service management.
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the menu item in each section (gui and registry) must be the same.\n
     *  For example: the menu item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *  @see ::fwGui::registrar::ToolBarRegistrar::initialize(), ::fwGui::layoutManager::IToolBarLayoutManager::initialize()
     */
    FWGUI_API void initialize();

    /// Create the layout and start the managed services.
    FWGUI_API void create();

    /// Destroy the layout and stop the managed services.
    FWGUI_API void destroy();

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );

    ::fwGui::layoutManager::IToolBarLayoutManager::sptr m_layoutManager;
    ::fwGui::registrar::ToolBarRegistrar::sptr    m_registrar;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hideActions;
};

} // namespace fwGui

#endif /*_FWGUI_ITOOLBARSRV_HPP_*/


