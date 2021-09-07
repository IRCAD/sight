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

/**
 * @brief This file defines the interface of the base class for managing a menubar.
 */

#pragma once

#include "ui/base/config.hpp"
#include "ui/base/container/fwMenu.hpp"
#include "ui/base/container/fwMenuBar.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <core/runtime/ConfigurationElement.hpp>

namespace sight::ui::base
{

namespace layoutManager
{

/**
 * @brief   Defines the menu bar layout manager for IHM.
 */
class UI_BASE_CLASS_API IMenuBarLayoutManager : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IMenuBarLayoutManager, ui::base::GuiBaseObject);

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;

    UI_BASE_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    UI_BASE_API IMenuBarLayoutManager();

    /// Destructor. Do nothing.
    UI_BASE_API virtual ~IMenuBarLayoutManager();

    /**
     * @brief Returns the vector of fwMenu managed by this layout.
     */
    UI_BASE_API virtual std::vector<ui::base::container::fwMenu::sptr> getMenus();

    /**
     * @brief Initialize layout managers.
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
              <menu sid="myMenu" start="true" />
              <menu sid="myMenu2" start="true" />
          </registry>
       </service>
       @endcode
     * This method analyzes the gui section of the configuration.
     *  - \<layout\> (mandatory) : give the list of the menu that will appear in the menu bar.
     *  - \<menu name="My Menu"/\> :
     *   - \b name (mandatory) : give the name of the menu that will appear in the interface.
     */

    UI_BASE_API virtual void initialize(ConfigurationType configuration);

    /**
     * @brief Instantiate menu with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    UI_BASE_API virtual void createLayout(ui::base::container::fwMenuBar::sptr parent) = 0;

    /**
     * @brief Destroy local menus.
     * @pre services using this menus must be stopped before.
     */
    UI_BASE_API virtual void destroyLayout() = 0;

    /**
     * @brief Set the menu visibility.
     */
    UI_BASE_API virtual void menuIsVisible(ui::base::container::fwMenu::sptr, bool isVisible) = 0;

    /**
     * @brief Set the menu enable or not.
     */
    UI_BASE_API virtual void menuIsEnabled(ui::base::container::fwMenu::sptr, bool isEnabled) = 0;

protected:

    /**
     * @brief Helper to destroy local sub views.
     */
    UI_BASE_API virtual void destroyMenus();

    /// All sub containers managed by this layout.
    std::vector<ui::base::container::fwMenu::sptr> m_menus;

    /// Save menu name from configuration.
    std::vector<std::string> m_menuNames;
};

} // namespace layoutManager

} // namespace sight::ui::base
