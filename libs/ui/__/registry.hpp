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

typedef std::map<std::string, ui::container::widget::sptr> container_map_t;
typedef std::map<std::string, ui::container::menubar::sptr> menu_bar_map_t;
typedef std::map<std::string, ui::container::toolbar::sptr> tool_bar_map_t;
typedef std::map<std::string, ui::container::menu::sptr> menu_map_t;
typedef std::vector<std::string> parent_sids_t;
typedef std::map<std::string, parent_sids_t> ActionToParentMapType;

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-container methods
 */
///@{

/**
 * @brief Registers container associate with service ID.
 */
UI_API void register_sid_container(std::string _sid, ui::container::widget::sptr _container);

/**
 * @brief Unregisters container associate with service ID.
 */
UI_API void unregister_sid_container(std::string _sid);

/**
 * @brief Returns widget associate with service ID, null if  not found.
 */
UI_API ui::container::widget::sptr get_sid_container(std::string _sid);

/**
 * @brief Verifies if a SID exists in the global SID container.
 */
UI_API bool has_sid_container(std::string _sid);

///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper WID-container methods
 */
///@{

/**
 * @brief Registers container associate with window ID.
 */
UI_API void register_wid_container(std::string _wid, ui::container::widget::sptr _container);

/**
 * @brief Unregisters container associate with window ID.
 */
UI_API void unregister_wid_container(std::string _wid);

/**
 * @brief Returns widget associate with window ID, null if  not found.
 */
UI_API ui::container::widget::sptr get_wid_container(std::string _wid);

/**
 * @brief Verifies if a WID exists in the global WID container.
 */
UI_API bool has_wid_container(std::string _wid);

///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-menu_bar methods
 */
///@{

/**
 * @brief Registers menubar associate with service ID.
 */
UI_API void register_sid_menu_bar(std::string _sid, ui::container::menubar::sptr _menu_bar);

/**
 * @brief Unregisters menubar associate with service ID.
 */
UI_API void unregister_sid_menu_bar(std::string _sid);
/**
 * @brief Returns menubar associate with service ID.
 */
UI_API ui::container::menubar::sptr get_sid_menu_bar(std::string _sid);
///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-tool_bar methods
 */
///@{

/**
 * @brief Registers toolbar associate with service ID.
 */
UI_API void register_sid_tool_bar(std::string _sid, ui::container::toolbar::sptr _tool_bar);

/**
 * @brief Unregisters toolbar associate with service ID.
 */
UI_API void unregister_sid_tool_bar(std::string _sid);
/**
 * @brief Returns toolbar associate with service ID.
 */
UI_API ui::container::toolbar::sptr get_sid_tool_bar(std::string _sid);
///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper SID-menu methods
 */
///@{

/**
 * @brief Registers menu associate with service ID.
 */
UI_API void register_sid_menu(std::string _sid, ui::container::menu::sptr _menu);

/**
 * @brief Unregisters menu associate with service ID.
 */
UI_API void unregister_sid_menu(std::string _sid);
/**
 * @brief Returns menu associate with service ID.
 */
UI_API ui::container::menu::sptr get_sid_menu(std::string _sid);
///@}

//-----------------------------------------------------------------------------

/**
 * @name    Helper action-menu methods
 */
///@{

/**
 * @brief Registers action sid associated with a parent sid.
 */
UI_API void register_action_sid_to_parent_sid(std::string _action_sid, std::string _parent_sid);

/**
 * @brief Unregisters action sid associated with a parent sid.
 */
UI_API void unregister_action_sid_to_parent_sid(std::string _action_sid, std::string _parent_sid);

/**
 * @brief Method called when the action service is stopping.
 * Call parent service actionServiceStopping() method
 */
UI_API void action_service_stopping(std::string _action_sid);

/**
 * @brief Method called when the action service is starting.
 * Call parent service actionServiceStarting() method
 */
UI_API void action_service_starting(std::string _action_sid);

/**
 * @brief Method called when the action service is activated or not.
 * Call parent service actionServiceSetChecked() method
 */
UI_API void action_service_set_checked(std::string _action_sid, bool _is_checked);

/**
 * @brief Method called when the action service is executable or not.
 * Call parent service actionServiceSetEnabled() method
 */
UI_API void action_service_set_enabled(std::string _action_sid, bool _is_enabled);

/**
 * @brief Method called when the action service is visible or not.
 * Call parent service actionServiceSetVisible() method
 */
UI_API void action_service_set_visible(std::string _action_sid, bool _is_visible);
///@}

} // namespace sight::ui::registry
