/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_ITOOLBARSRV_HPP__
#define __FWGUI_ITOOLBARSRV_HPP__

#include "fwGui/config.hpp"
#include "fwGui/layoutManager/IToolBarLayoutManager.hpp"
#include "fwGui/registrar/ToolBarRegistrar.hpp"

#include <fwServices/IService.hpp>

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the toolBar.
 */
class FWGUI_CLASS_API IToolBarSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IToolBarSrv)(::fwServices::IService) );

    /// Method called when an action service is stopping
    FWGUI_API void actionServiceStopping(std::string actionSrvSID);

    /// Method called when an action service is starting
    FWGUI_API void actionServiceStarting(std::string actionSrvSID);

    /// Method called when the action service is activated
    FWGUI_API void actionServiceSetActive(std::string actionSrvSID, bool isActive);

    /// Method called when the action service is executable
    FWGUI_API void actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable);

    /// Method called when the action service is visible
    FWGUI_API void actionServiceSetVisible(std::string actionSrvSID, bool isVisible);

protected:

    FWGUI_API IToolBarSrv();

    FWGUI_API virtual ~IToolBarSrv();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="::fwGui::IToolBarSrv" impl="::gui::aspect::SDefaultToolBar" autoConnect="no" >
           <gui>
               <layout hideAction="false">
                   <menuItem name="My item 2" style="radio" icon="TutoGui-0.1/icons/system.png"/>
                   <menuItem name="My item 3" style="radio" icon="TutoGui-0.1/icons/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
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
       @endcode
     * - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *   - \<layout\> \</layout\> : (mandatory) describe the layout of the service
     *     - \b hideAction: (optional, default=false): if true, the actions are hidden when they are stopped.
     * - \<registry\> \</registry\> : (mandatory) describe the service management.
     *   - \<menuItem\> represents IActionSrv
     *   - \<menu\> represents IMenuSrv
     *   - \<editor\> represents container service (IEditor, IView, ...)
     *
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the item in each section (gui and registry) must be the same.\n
     *  For example: the item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *  @see ::fwGui::registrar::ToolBarRegistrar::initialize(),
     *::fwGui::layoutManager::IToolBarLayoutManager::initialize()
     */
    FWGUI_API void initialize();

    /// Create the layout and start the managed services.
    FWGUI_API void create();

    /// Destroy the layout and stop the managed services.
    FWGUI_API void destroy();

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );

    ::fwGui::layoutManager::IToolBarLayoutManager::sptr m_layoutManager;
    ::fwGui::registrar::ToolBarRegistrar::sptr m_registrar;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the actions if the service is stopped
    bool m_hideActions;
};

} // namespace fwGui

#endif /*__FWGUI_ITOOLBARSRV_HPP__*/

