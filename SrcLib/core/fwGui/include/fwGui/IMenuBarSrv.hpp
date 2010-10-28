/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IMENUBARSRV_HPP_
#define _FWGUI_IMENUBARSRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"

#include "fwGui/registrar/MenuBarRegistrar.hpp"
#include "fwGui/layoutManager/IMenuBarLayoutManager.hpp"

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the menu bar.
 * @class   IMenuBarSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMenuBarSrv : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IMenuBarSrv)(::fwServices::IService) ) ;

    /// Method called when a menu service is stopping
    FWGUI_API void menuServiceStopping(std::string menuSrvSID);

    /// Method called when a menu service is starting
    FWGUI_API void menuServiceStarting(std::string menuSrvSID);

protected :
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IMenuBarSrv() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IMenuBarSrv() ;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @verbatim
      <service uid="menuBar" type="::fwGui::IMenuBarSrv" implementation="::gui::aspect::DefaultMenuBarSrv" autoComChannel="no" >
          <gui>
              <layout>
                  <menu name="My Menu"/>
                  <menu name="My Menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" start="yes" />
              <menu sid="myMenu2" start="yes" />
          </registry>
      </service>
       @endverbatim
     *  <gui> </gui> : (mandatory) describe the interface of the service.
     *
     *  <registry> </registry> : (mandatory) describe the service management.
     *   @warning
     *   - The number of item in the gui section must be equal or greater than in the registry section.
     *   - The order of the menu in each section (gui and registry) must be the same.\n
     *  For example: the menu named "My Menu" will be connected with the service which have the sid = "myMenu".
     *   - A menu bar can't have the same service connected on two different menu.
     *  @see ::fwGui::registrar::MenuBarRegistrar::initialize(), ::fwGui::layoutManager::IMenuBarLayoutManager::initialize()
     */
    FWGUI_API void initialize();

    /// Create the layout and start the managed services.
    FWGUI_API void create();

    /// Destroy the layout and stop the managed services.
    FWGUI_API void destroy();

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );

    ::fwGui::layoutManager::IMenuBarLayoutManager::sptr m_layoutManager;
    ::fwGui::registrar::MenuBarRegistrar::sptr    m_registrar;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the menu if the service is stopped
    bool m_hideMenus;
};

} // namespace fwGui

#endif /*_FWGUI_IMENUBARSRV_HPP_*/


