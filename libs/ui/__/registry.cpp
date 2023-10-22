/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/__/registry.hpp"

#include "ui/__/menu.hpp"
#include "ui/__/toolbar.hpp"

#include <core/tools/id.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

namespace sight::ui::registry
{

static container_map_t m_global_sid_to_fw_container;
static container_map_t m_global_wid_to_fw_container;
static menu_bar_map_t m_global_sid_to_fw_menu_bar;
static tool_bar_map_t m_global_sid_to_fw_tool_bar;
static menu_map_t m_global_sid_to_fw_menu;

/// Parent sid can be menu sid or tool_bar sid
static ActionToParentMapType m_action_sid_to_parent_sid;

//-----------------------------------------------------------------------------

void register_sid_container(std::string _sid, ui::container::widget::sptr _container)
{
    SIGHT_ASSERT(
        "The service '" + _sid + "' is already registered in the container map, it cannot be added twice.",
        m_global_sid_to_fw_container.find(_sid) == m_global_sid_to_fw_container.end()
    );
    m_global_sid_to_fw_container[_sid] = _container;
}

//-----------------------------------------------------------------------------

void unregister_sid_container(std::string _sid)
{
    bool service_exists = core::tools::id::exist(_sid);
    SIGHT_INFO_IF("The service '" + _sid + "' does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(_sid);
        SIGHT_ASSERT(
            "The service '" + _sid + "' must be stopped before unregistering the container.",
            service->stopped()
        );
    }

    SIGHT_ASSERT(
        "The service '" + _sid + "' is not registered in the container map, it cannot be removed.",
        m_global_sid_to_fw_container.find(_sid) != m_global_sid_to_fw_container.end()
    );

    // Removes container in SID container map
    m_global_sid_to_fw_container.erase(_sid);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr get_sid_container(std::string _sid)
{
    // Returns container in SID container map, null if  not found.
    if(!has_sid_container(_sid))
    {
        return nullptr;
    }

    return m_global_sid_to_fw_container[_sid];
}

//-----------------------------------------------------------------------------

bool has_sid_container(std::string _sid)
{
    // Returns 'true' if the specified 'sid' is found in the SID container map, else 'false'.
    return m_global_sid_to_fw_container.find(_sid) != m_global_sid_to_fw_container.end();
}

//-----------------------------------------------------------------------------

void register_wid_container(std::string _wid, ui::container::widget::sptr _container)
{
    SIGHT_ASSERT(
        "The wid container '" + _wid + "' is already registered in the container map, it cannot be added twice.",
        m_global_wid_to_fw_container.find(_wid) == m_global_wid_to_fw_container.end()
    );
    m_global_wid_to_fw_container[_wid] = _container;
}

//-----------------------------------------------------------------------------

void unregister_wid_container(std::string _wid)
{
    SIGHT_ASSERT(
        "The wid container '" + _wid + "' is not registered in the container map.",
        m_global_wid_to_fw_container.find(_wid) != m_global_wid_to_fw_container.end()
    );

    // Removes container in WID container map
    m_global_wid_to_fw_container.erase(_wid);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr get_wid_container(std::string _wid)
{
    // Returns container in WID container map, null if  not found.
    if(!has_wid_container(_wid))
    {
        return nullptr;
    }

    return m_global_wid_to_fw_container[_wid];
}

//-----------------------------------------------------------------------------

bool has_wid_container(std::string _wid)
{
    // Returns 'true' if the specified 'wid' is found in the WID container map, else 'false'.
    return m_global_wid_to_fw_container.find(_wid) != m_global_wid_to_fw_container.end();
}

//-----------------------------------------------------------------------------

void register_sid_menu_bar(std::string _sid, ui::container::menubar::sptr _menu_bar)
{
    SIGHT_ASSERT(
        "The menuBar '" + _sid + "' is already registered in the container map, it cannot be added twice.",
        m_global_sid_to_fw_menu_bar.find(_sid) == m_global_sid_to_fw_menu_bar.end()
    );
    m_global_sid_to_fw_menu_bar[_sid] = _menu_bar;
}

//-----------------------------------------------------------------------------

void unregister_sid_menu_bar(std::string _sid)
{
    bool service_exists = core::tools::id::exist(_sid);
    SIGHT_INFO_IF("The menubar '" + _sid + "' does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(_sid);
        SIGHT_ASSERT(
            "The service '" + _sid + "' must be stopped before unregistering the menuBar.",
            service->stopped()
        );
    }

    SIGHT_ASSERT(
        "The menuBar '" + _sid + "' is not registered in the container map, it cannot be removed.",
        m_global_sid_to_fw_menu_bar.find(_sid) != m_global_sid_to_fw_menu_bar.end()
    );

    // Removes container in SID container map
    m_global_sid_to_fw_menu_bar.erase(_sid);
}

//-----------------------------------------------------------------------------

ui::container::menubar::sptr get_sid_menu_bar(std::string _sid)
{
    SIGHT_ASSERT(
        "The menuBar '" + _sid + "' is not registered in the container map.",
        m_global_sid_to_fw_menu_bar.find(_sid) != m_global_sid_to_fw_menu_bar.end()
    );
    // returns container in SID container map
    return m_global_sid_to_fw_menu_bar[_sid];
}

//-----------------------------------------------------------------------------

void register_sid_tool_bar(std::string _sid, ui::container::toolbar::sptr _tool_bar)
{
    SIGHT_ASSERT(
        "The toolBar '" + _sid + "' is already registered in the container map, it cannot be added twice.",
        m_global_sid_to_fw_tool_bar.find(_sid) == m_global_sid_to_fw_tool_bar.end()
    );
    m_global_sid_to_fw_tool_bar[_sid] = _tool_bar;
}

//-----------------------------------------------------------------------------

void unregister_sid_tool_bar(std::string _sid)
{
    bool service_exists = core::tools::id::exist(_sid);
    SIGHT_INFO_IF("The toolBar '" + _sid + "' does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(_sid);
        SIGHT_ASSERT(
            "The service '" + _sid + "' must be stopped before unregistering the menuBar.",
            service->stopped()
        );
    }

    SIGHT_ASSERT(
        "The toolBar '" + _sid + "' is not registered in the container map, it cannot be removed.",
        m_global_sid_to_fw_tool_bar.find(_sid) != m_global_sid_to_fw_tool_bar.end()
    );

    // Removes container in SID container map
    m_global_sid_to_fw_tool_bar.erase(_sid);
}

//-----------------------------------------------------------------------------

ui::container::toolbar::sptr get_sid_tool_bar(std::string _sid)
{
    SIGHT_ASSERT(
        "The toolBar '" + _sid + "' is not registered in the container map.",
        m_global_sid_to_fw_tool_bar.find(_sid) != m_global_sid_to_fw_tool_bar.end()
    );
    // returns container in SID container map
    return m_global_sid_to_fw_tool_bar[_sid];
}

//-----------------------------------------------------------------------------

void register_sid_menu(std::string _sid, ui::container::menu::sptr _menu)
{
    SIGHT_ASSERT(
        "The menu '" + _sid + "' is already registered in the container map, it cannot be added twice.",
        m_global_sid_to_fw_menu.find(_sid) == m_global_sid_to_fw_menu.end()
    );
    m_global_sid_to_fw_menu[_sid] = _menu;
}

//-----------------------------------------------------------------------------

void unregister_sid_menu(std::string _sid)
{
    bool service_exists = core::tools::id::exist(_sid);
    SIGHT_INFO_IF("The menu " << _sid << " does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(_sid);
        SIGHT_ASSERT("The service '" + _sid + "' must be stopped before unregistering the menu.", service->stopped());
    }

    SIGHT_ASSERT(
        "The menu '" + _sid + "' is not registered in the container map, it cannot be removed.",
        m_global_sid_to_fw_menu.find(_sid) != m_global_sid_to_fw_menu.end()
    );

    // Removes container in SID container map
    m_global_sid_to_fw_menu.erase(_sid);
}

//-----------------------------------------------------------------------------

ui::container::menu::sptr get_sid_menu(std::string _sid)
{
    SIGHT_ASSERT(
        "The menu '" + _sid + "' is not registered in the container map.",
        m_global_sid_to_fw_menu.find(_sid) != m_global_sid_to_fw_menu.end()
    );
    // returns container in SID container map
    return m_global_sid_to_fw_menu[_sid];
}

//-----------------------------------------------------------------------------

void register_action_sid_to_parent_sid(std::string _action_sid, std::string _parent_sid)
{
    if(m_action_sid_to_parent_sid.find(_action_sid) != m_action_sid_to_parent_sid.end())
    {
        // action already exist in map
        SIGHT_ASSERT(
            std::string("The action '") + _action_sid + "' already exists for the parent '" + _parent_sid + "'",
            std::find(
                m_action_sid_to_parent_sid[_action_sid].begin(),
                m_action_sid_to_parent_sid[_action_sid].end(),
                _parent_sid
            ) == m_action_sid_to_parent_sid[_action_sid].end()
        );
    }

    m_action_sid_to_parent_sid[_action_sid].push_back(_parent_sid);
}

//-----------------------------------------------------------------------------

void unregister_action_sid_to_parent_sid(std::string _action_sid, std::string _parent_sid)
{
    SIGHT_ASSERT(
        "The action '" + _action_sid + "' is not registered.",
        m_action_sid_to_parent_sid.find(_action_sid) != m_action_sid_to_parent_sid.end()
    );

    if(m_action_sid_to_parent_sid[_action_sid].size() == 1)
    {
        // action has one parent
        bool service_exists = core::tools::id::exist(_action_sid);
        SIGHT_INFO_IF("The service '" + _action_sid + "' does not exist.", !service_exists);
        if(service_exists)
        {
            service::base::sptr service = service::get(_action_sid);
            SIGHT_ASSERT(
                "The service '" + _action_sid + "' must be stopped before unregistering the action.",
                service->stopped()
            );
        }

        m_action_sid_to_parent_sid.erase(_action_sid);
    }
    else
    {
        // action has several parents
        auto iter =
            std::find(
                m_action_sid_to_parent_sid[_action_sid].begin(),
                m_action_sid_to_parent_sid[_action_sid].end(),
                _parent_sid
            );
        SIGHT_ASSERT(
            std::string("The action with the sid '") + _action_sid + "' has no parent named '" + _parent_sid + "'",
            iter != m_action_sid_to_parent_sid[_action_sid].end()
        );
        m_action_sid_to_parent_sid[_action_sid].erase(iter);
    }
}

//-----------------------------------------------------------------------------

void action_service_stopping(std::string _action_sid)
{
    if(m_action_sid_to_parent_sid.find(_action_sid) != m_action_sid_to_parent_sid.end())
    {
        parent_sids_t parent_sids = m_action_sid_to_parent_sid[_action_sid];

        for(const std::string& parent_sid : parent_sids)
        {
            bool service_exists = core::tools::id::exist(parent_sid);
            SIGHT_INFO_IF(
                std::string("The service '") + parent_sid + "' managing the action '" + _action_sid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service   = service::get(parent_sid);
                ui::menu::sptr menu_srv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbar_srv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menu_srv)
                {
                    menu_srv->actionServiceStopping(_action_sid);
                }
                else if(toolbar_srv)
                {
                    toolbar_srv->actionServiceStopping(_action_sid);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parent_sid + "' managing the action '"
                        + _action_sid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void action_service_starting(std::string _action_sid)
{
    if(m_action_sid_to_parent_sid.find(_action_sid) != m_action_sid_to_parent_sid.end())
    {
        parent_sids_t parent_sids = m_action_sid_to_parent_sid[_action_sid];

        for(const std::string& parent_sid : parent_sids)
        {
            bool service_exists = core::tools::id::exist(parent_sid);
            SIGHT_INFO_IF(
                std::string("The service '") + parent_sid + "' managing the action '" + _action_sid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service   = service::get(parent_sid);
                ui::menu::sptr menu_srv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbar_srv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menu_srv)
                {
                    menu_srv->actionServiceStarting(_action_sid);
                }
                else if(toolbar_srv)
                {
                    toolbar_srv->actionServiceStarting(_action_sid);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parent_sid + "' managing the action '"
                        + _action_sid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void action_service_set_checked(std::string _action_sid, bool _is_checked)
{
    if(m_action_sid_to_parent_sid.find(_action_sid) != m_action_sid_to_parent_sid.end())
    {
        parent_sids_t parent_sids = m_action_sid_to_parent_sid[_action_sid];

        for(const std::string& parent_sid : parent_sids)
        {
            bool service_exists = core::tools::id::exist(parent_sid);
            SIGHT_INFO_IF(
                std::string("The service '") + parent_sid + "' managing the action '" + _action_sid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service   = service::get(parent_sid);
                ui::menu::sptr menu_srv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbar_srv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menu_srv)
                {
                    menu_srv->actionServiceSetChecked(_action_sid, _is_checked);
                }
                else if(toolbar_srv)
                {
                    toolbar_srv->actionServiceSetChecked(_action_sid, _is_checked);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parent_sid + "' managing the action '"
                        + _action_sid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void action_service_set_enabled(std::string _action_sid, bool _is_enabled)
{
    if(m_action_sid_to_parent_sid.find(_action_sid) != m_action_sid_to_parent_sid.end())
    {
        parent_sids_t parent_sids = m_action_sid_to_parent_sid[_action_sid];

        for(const std::string& parent_sid : parent_sids)
        {
            bool service_exists = core::tools::id::exist(parent_sid);
            SIGHT_INFO_IF(
                std::string("The service '") + parent_sid + "' managing the action '" + _action_sid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service   = service::get(parent_sid);
                ui::menu::sptr menu_srv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbar_srv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menu_srv)
                {
                    menu_srv->actionServiceSetEnabled(_action_sid, _is_enabled);
                }
                else if(toolbar_srv)
                {
                    toolbar_srv->actionServiceSetEnabled(_action_sid, _is_enabled);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parent_sid + "' managing the action '"
                        + _action_sid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void action_service_set_visible(std::string _action_sid, bool _is_visible)
{
    if(m_action_sid_to_parent_sid.find(_action_sid) != m_action_sid_to_parent_sid.end())
    {
        parent_sids_t parent_sids = m_action_sid_to_parent_sid[_action_sid];

        for(const std::string& parent_sid : parent_sids)
        {
            bool service_exists = core::tools::id::exist(parent_sid);
            SIGHT_INFO_IF(
                std::string("The service '") + parent_sid + "' managing the action '" + _action_sid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service   = service::get(parent_sid);
                ui::menu::sptr menu_srv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbar_srv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menu_srv)
                {
                    menu_srv->actionServiceSetVisible(_action_sid, _is_visible);
                }
                else if(toolbar_srv)
                {
                    toolbar_srv->actionServiceSetVisible(_action_sid, _is_visible);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parent_sid + "' managing the action '"
                        + _action_sid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::registry
