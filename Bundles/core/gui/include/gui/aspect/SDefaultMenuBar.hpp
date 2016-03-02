/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ASPECT_SDEFAULTMENUBAR_HPP__
#define __GUI_ASPECT_SDEFAULTMENUBAR_HPP__

#include <fwGui/IMenuBarSrv.hpp>

#include "gui/config.hpp"

namespace gui
{
namespace aspect
{

/**
 * @brief   Defines the default menubar for standard application
 */
class GUI_CLASS_API SDefaultMenuBar : public ::fwGui::IMenuBarSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SDefaultMenuBar)(::fwGui::IMenuBarSrv) );

    /// Constructor. Do nothing.
    GUI_API SDefaultMenuBar() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~SDefaultMenuBar() throw();

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Configuring method allows to configure an application with menu bar.
     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="::fwGui::IMenuBarSrv" impl="::gui::aspect::SDefaultMenuBar" autoConnect="no" >
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
       @endcode
     *  - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *  - \<registry\> \</registry\> : (mandatory) describe the service management.
     *   - sid of the menu services
     *   - start or not the menu service automatically
     *
     * @warning
     * - The number of item in the gui section must be equal or greater than in the registry section.
     * - The order of the menu in each section (gui and registry) must be the same.\n
     *   For example: the menu named "My Menu" will be connected with the service which have the sid = "myMenu".
     * - A menu bar can't have the same service connected on two different menu.
     *
     *  @see ::fwGui::IMenuBarSrv::initialize(), ::fwGui::layoutManager::IMenuBarLayoutManager::initialize()
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed );

    /**
     * @brief Create the menus and start the managed services.
     * @see ::fwGui::IMenuBarSrv::create()
     */
    GUI_API virtual void starting() throw( ::fwTools::Failed );

    /**
     * @brief Stop all menu of this menuBar and destroy MenuBar
     * @see ::fwGui::IMenuBarSrv::destroy()
     */
    GUI_API virtual void stopping() throw( ::fwTools::Failed );

    /// Updating service, do nothing.
    GUI_API virtual void updating() throw(::fwTools::Failed);

    ///@}
};

}

}

#endif // __GUI_ASPECT_SDEFAULTMENUBAR_HPP__
