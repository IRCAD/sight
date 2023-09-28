/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/container/menu.hpp"
#include "ui/__/container/menu_item.hpp"
#include "ui/__/container/menubar.hpp"
#include "ui/__/container/toolbar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

// contains helpers to manage all widget in the application.
namespace sight::ui::registry
{

typedef std::map<std::string, ui::container::widget::sptr> ContainerMapType;
typedef std::map<std::string, ui::container::menubar::sptr> MenuBarMapType;
typedef std::map<std::string, ui::container::toolbar::sptr> ToolBarMapType;
typedef std::map<std::string, ui::container::menu::sptr> MenuMapType;
typedef std::vector<std::string> ParentSidsType;
typedef std::map<std::string, ParentSidsType> ActionToParentMapType;

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-container methods
 */
///@{

/**
 * @brief Registers container associate with service ID.
 */
UI_API void registerSIDContainer(std::string sid, ui::container::widget::sptr container);

/**
 * @brief Unregisters container associate with service ID.
 */
UI_API void unregisterSIDContainer(std::string sid);

/**
 * @brief Returns widget associate with service ID, null if  not found.
 */
UI_API ui::container::widget::sptr getSIDContainer(std::string sid);

/**
 * @brief Verifies if a SID exists in the global SID container.
 */
UI_API bool hasSIDContainer(std::string sid);

///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper WID-container methods
 */
///@{

/**
 * @brief Registers container associate with window ID.
 */
UI_API void registerWIDContainer(std::string wid, ui::container::widget::sptr container);

/**
 * @brief Unregisters container associate with window ID.
 */
UI_API void unregisterWIDContainer(std::string wid);

/**
 * @brief Returns widget associate with window ID, null if  not found.
 */
UI_API ui::container::widget::sptr getWIDContainer(std::string wid);

/**
 * @brief Verifies if a WID exists in the global WID container.
 */
UI_API bool hasWIDContainer(std::string wid);

///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-MenuBar methods
 */
///@{

/**
 * @brief Registers menubar associate with service ID.
 */
UI_API void registerSIDMenuBar(std::string sid, ui::container::menubar::sptr menuBar);

/**
 * @brief Unregisters menubar associate with service ID.
 */
UI_API void unregisterSIDMenuBar(std::string sid);
/**
 * @brief Returns menubar associate with service ID.
 */
UI_API ui::container::menubar::sptr getSIDMenuBar(std::string sid);
///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-ToolBar methods
 */
///@{

/**
 * @brief Registers toolbar associate with service ID.
 */
UI_API void registerSIDToolBar(std::string sid, ui::container::toolbar::sptr toolBar);

/**
 * @brief Unregisters toolbar associate with service ID.
 */
UI_API void unregisterSIDToolBar(std::string sid);
/**
 * @brief Returns toolbar associate with service ID.
 */
UI_API ui::container::toolbar::sptr getSIDToolBar(std::string sid);
///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-Menu methods
 */
///@{

/**
 * @brief Registers menu associate with service ID.
 */
UI_API void registerSIDMenu(std::string sid, ui::container::menu::sptr menu);

/**
 * @brief Unregisters menu associate with service ID.
 */
UI_API void unregisterSIDMenu(std::string sid);
/**
 * @brief Returns menu associate with service ID.
 */
UI_API ui::container::menu::sptr getSIDMenu(std::string sid);
///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper Action-Menu methods
 */
///@{

/**
 * @brief Registers action sid associated with a parent sid.
 */
UI_API void registerActionSIDToParentSID(std::string actionSid, std::string parentSid);

/**
 * @brief Unregisters action sid associated with a parent sid.
 */
UI_API void unregisterActionSIDToParentSID(std::string actionSid, std::string parentSid);

/**
 * @brief Method called when the action service is stopping.
 * Call parent service actionServiceStopping() method
 */
UI_API void actionServiceStopping(std::string actionSid);

/**
 * @brief Method called when the action service is starting.
 * Call parent service actionServiceStarting() method
 */
UI_API void actionServiceStarting(std::string actionSid);

/**
 * @brief Method called when the action service is activated or not.
 * Call parent service actionServiceSetChecked() method
 */
UI_API void actionServiceSetChecked(std::string actionSid, bool isChecked);

/**
 * @brief Method called when the action service is executable or not.
 * Call parent service actionServiceSetEnabled() method
 */
UI_API void actionServiceSetEnabled(std::string actionSid, bool isEnabled);

/**
 * @brief Method called when the action service is visible or not.
 * Call parent service actionServiceSetVisible() method
 */
UI_API void actionServiceSetVisible(std::string actionSid, bool isVisible);
///@}

} // namespace sight::ui::registry
