/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "ui/base/container/fwMenu.hpp"
#include "ui/base/container/fwMenuBar.hpp"
#include "ui/base/container/fwMenuItem.hpp"
#include "ui/base/container/fwToolBar.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base
{

/**
 * @brief   This class contains helpers to manage all fwContainer in the application.
 */
class UI_BASE_CLASS_API GuiRegistry : public ui::base::GuiBaseObject
{

public:

    typedef std::map< std::string, ui::base::container::fwContainer::sptr > ContainerMapType;
    typedef std::map< std::string, ui::base::container::fwMenuBar::sptr >   MenuBarMapType;
    typedef std::map< std::string, ui::base::container::fwToolBar::sptr >   ToolBarMapType;
    typedef std::map< std::string, ui::base::container::fwMenu::sptr >      MenuMapType;
    typedef std::vector< std::string >                           ParentSidsType;
    typedef std::map< std::string, ParentSidsType > ActionToParentMapType;

    fwCoreClassMacro(GuiRegistry, ui::base::GuiBaseObject)

    /**
     * @brief   Constructor.
     */
    UI_BASE_API GuiRegistry();

    /**
     * @brief   Destructor. Do nothing.
     */
    UI_BASE_API virtual ~GuiRegistry();

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-container methods
     */
    ///@{

    /**
     * @brief Registers container associate with service ID.
     */
    UI_BASE_API static void registerSIDContainer(std::string sid, ui::base::container::fwContainer::sptr container);

    /**
     * @brief Unregisters container associate with service ID.
     */
    UI_BASE_API static void unregisterSIDContainer(std::string sid);

    /**
     * @brief Returns fwContainer associate with service ID, null if  not found.
     */
    UI_BASE_API static ui::base::container::fwContainer::sptr getSIDContainer(std::string sid);

    /**
     * @brief Verifies if a SID exists in the global SID container.
     */
    UI_BASE_API static bool hasSIDContainer(std::string sid);

    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper WID-container methods
     */
    ///@{

    /**
     * @brief Registers container associate with window ID.
     */
    UI_BASE_API static void registerWIDContainer(std::string wid, ui::base::container::fwContainer::sptr container);

    /**
     * @brief Unregisters container associate with window ID.
     */
    UI_BASE_API static void unregisterWIDContainer(std::string wid);

    /**
     * @brief Returns fwContainer associate with window ID, null if  not found.
     */
    UI_BASE_API static ui::base::container::fwContainer::sptr getWIDContainer(std::string wid);

    /**
     * @brief Verifies if a WID exists in the global WID container.
     */
    UI_BASE_API static bool hasWIDContainer(std::string wid);

    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-MenuBar methods
     */
    ///@{

    /**
     * @brief Registers fwMenuBar associate with service ID.
     */
    UI_BASE_API static void registerSIDMenuBar(std::string sid, ui::base::container::fwMenuBar::sptr menuBar);

    /**
     * @brief Unregisters fwMenuBar associate with service ID.
     */
    UI_BASE_API static void unregisterSIDMenuBar(std::string sid);
    /**
     * @brief Returns fwMenuBar associate with service ID.
     */
    UI_BASE_API static ui::base::container::fwMenuBar::sptr getSIDMenuBar(std::string sid);
    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-ToolBar methods
     */
    ///@{

    /**
     * @brief Registers fwToolBar associate with service ID.
     */
    UI_BASE_API static void registerSIDToolBar(std::string sid, ui::base::container::fwToolBar::sptr toolBar);

    /**
     * @brief Unregisters fwToolBar associate with service ID.
     */
    UI_BASE_API static void unregisterSIDToolBar(std::string sid);
    /**
     * @brief Returns fwToolBar associate with service ID.
     */
    UI_BASE_API static ui::base::container::fwToolBar::sptr getSIDToolBar(std::string sid);
    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-Menu methods
     */
    ///@{

    /**
     * @brief Registers fwMenu associate with service ID.
     */
    UI_BASE_API static void registerSIDMenu(std::string sid, ui::base::container::fwMenu::sptr menu);

    /**
     * @brief Unregisters fwMenu associate with service ID.
     */
    UI_BASE_API static void unregisterSIDMenu(std::string sid);
    /**
     * @brief Returns fwMenu associate with service ID.
     */
    UI_BASE_API static ui::base::container::fwMenu::sptr getSIDMenu(std::string sid);
    ///@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper Action-Menu methods
     */
    ///@{

    /**
     * @brief Registers action sid associted with a parent sid.
     */
    UI_BASE_API static void registerActionSIDToParentSID(std::string actionSid, std::string parentSid);

    /**
     * @brief Unregisters action sid associted with a parent sid.
     */
    UI_BASE_API static void unregisterActionSIDToParentSID(std::string actionSid, std::string parentSid);

    /**
     * @brief Method called when the action service is stopping.
     * Call parent service actionServiceStopping() method
     */
    UI_BASE_API static void actionServiceStopping(std::string actionSid);

    /**
     * @brief Method called when the action service is starting.
     * Call parent service actionServiceStarting() method
     */
    UI_BASE_API static void actionServiceStarting(std::string actionSid);

    /**
     * @brief Method called when the action service is activated or not.
     * Call parent service actionServiceSetActive() method
     */
    UI_BASE_API static void actionServiceSetActive(std::string actionSid, bool isActive);

    /**
     * @brief Method called when the action service is executable or not.
     * Call parent service actionServiceSetExecutable() method
     */
    UI_BASE_API static void actionServiceSetExecutable(std::string actionSid, bool isExecutable);

    /**
     * @brief Method called when the action service is visible or not.
     * Call parent service actionServiceSetVisible() method
     */
    UI_BASE_API static void actionServiceSetVisible(std::string actionSid, bool isVisible);
    ///@}

protected:

    static ContainerMapType m_globalSIDToFwContainer;
    static ContainerMapType m_globalWIDToFwContainer;
    static MenuBarMapType m_globalSIDToFwMenuBar;
    static ToolBarMapType m_globalSIDToFwToolBar;
    static MenuMapType m_globalSIDToFwMenu;

    /// Parent sid can be Menu sid or ToolBar sid
    static ActionToParentMapType m_actionSIDToParentSID;

};

} // namespace sight::ui::base
