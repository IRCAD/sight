/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "ui/base/config.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/layoutManager/IMenuBarLayoutManager.hpp"
#include "ui/base/registry/MenuBar.hpp"

#include <service/IService.hpp>

namespace sight::ui::base
{

/**
 * @brief   Defines the service interface managing the menu bar.
 */
class UI_BASE_CLASS_API IMenuBar : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IMenuBar, service::IService);

    /// Method called when a menu service is stopping
    UI_BASE_API void menuServiceStopping(std::string menuSrvSID);

    /// Method called when a menu service is starting
    UI_BASE_API void menuServiceStarting(std::string menuSrvSID);

protected:

    UI_BASE_API IMenuBar();

    UI_BASE_API virtual ~IMenuBar();

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize the layout and registry managers.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="ui::base::IMenuBar" impl="::sight::module::ui::base::SMenuBar"
     * autoConnect="false"
     *>
          <gui>
              <layout>
                  <menu name="My Menu"/>
                  <menu name="My Menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" start=true />
              <menu sid="myMenu2" start=true />
          </registry>
       </service>
       @endcode
     *  - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *  - \<registry\> \</registry\> : (mandatory) describe the service management.
     *
     * @warning
     * - The number of item in the gui section must be equal or greater than in the registry section.
     * - The order of the menu in each section (gui and registry) must be the same.\n
     *   For example: the menu named "My Menu" will be connected with the service which have the sid = "myMenu".
     * - A menu bar can't have the same service connected on two different menu.
     *
     *  @see ui::base::registry::MenuBar::initialize(),
     *::ui::base::layoutManager::IMenuBarLayoutManager::initialize()
     */
    UI_BASE_API void initialize();

    /// Create the layout and start the managed services.
    UI_BASE_API void create();

    /// Destroy the layout and stop the managed services.
    UI_BASE_API void destroy();

private:

    void initializeLayoutManager(core::runtime::ConfigurationElement::sptr layoutConfig);

    ui::base::layoutManager::IMenuBarLayoutManager::sptr m_layoutManager;
    ui::base::registry::MenuBar::sptr m_registry;

    ConfigurationType m_registryConfig;
    ConfigurationType m_layoutConfig;

    /// Flag to hide or disable the menu if the service is stopped
    bool m_hideMenus;
};

} // namespace sight::ui::base
