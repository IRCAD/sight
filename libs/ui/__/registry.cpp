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

static ContainerMapType m_globalSIDToFwContainer;
static ContainerMapType m_globalWIDToFwContainer;
static MenuBarMapType m_globalSIDToFwMenuBar;
static ToolBarMapType m_globalSIDToFwToolBar;
static MenuMapType m_globalSIDToFwMenu;

/// Parent sid can be Menu sid or ToolBar sid
static ActionToParentMapType m_actionSIDToParentSID;

//-----------------------------------------------------------------------------

void registerSIDContainer(std::string sid, ui::container::widget::sptr container)
{
    SIGHT_ASSERT(
        "The service '" + sid + "' is already registered in the container map, it cannot be added twice.",
        m_globalSIDToFwContainer.find(sid) == m_globalSIDToFwContainer.end()
    );
    m_globalSIDToFwContainer[sid] = container;
}

//-----------------------------------------------------------------------------

void unregisterSIDContainer(std::string sid)
{
    bool service_exists = core::tools::id::exist(sid);
    SIGHT_INFO_IF("The service '" + sid + "' does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(sid);
        SIGHT_ASSERT(
            "The service '" + sid + "' must be stopped before unregistering the container.",
            service->stopped()
        );
    }

    SIGHT_ASSERT(
        "The service '" + sid + "' is not registered in the container map, it cannot be removed.",
        m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end()
    );

    // Removes container in SID container map
    m_globalSIDToFwContainer.erase(sid);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr getSIDContainer(std::string sid)
{
    // Returns container in SID container map, null if  not found.
    if(!hasSIDContainer(sid))
    {
        return nullptr;
    }

    return m_globalSIDToFwContainer[sid];
}

//-----------------------------------------------------------------------------

bool hasSIDContainer(std::string sid)
{
    // Returns 'true' if the specified 'sid' is found in the SID container map, else 'false'.
    return m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end();
}

//-----------------------------------------------------------------------------

void registerWIDContainer(std::string wid, ui::container::widget::sptr container)
{
    SIGHT_ASSERT(
        "The wid container '" + wid + "' is already registered in the container map, it cannot be added twice.",
        m_globalWIDToFwContainer.find(wid) == m_globalWIDToFwContainer.end()
    );
    m_globalWIDToFwContainer[wid] = container;
}

//-----------------------------------------------------------------------------

void unregisterWIDContainer(std::string wid)
{
    SIGHT_ASSERT(
        "The wid container '" + wid + "' is not registered in the container map.",
        m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end()
    );

    // Removes container in WID container map
    m_globalWIDToFwContainer.erase(wid);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr getWIDContainer(std::string wid)
{
    // Returns container in WID container map, null if  not found.
    if(!hasWIDContainer(wid))
    {
        return nullptr;
    }

    return m_globalWIDToFwContainer[wid];
}

//-----------------------------------------------------------------------------

bool hasWIDContainer(std::string wid)
{
    // Returns 'true' if the specified 'wid' is found in the WID container map, else 'false'.
    return m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end();
}

//-----------------------------------------------------------------------------

void registerSIDMenuBar(std::string sid, ui::container::menubar::sptr menuBar)
{
    SIGHT_ASSERT(
        "The menuBar '" + sid + "' is already registered in the container map, it cannot be added twice.",
        m_globalSIDToFwMenuBar.find(sid) == m_globalSIDToFwMenuBar.end()
    );
    m_globalSIDToFwMenuBar[sid] = menuBar;
}

//-----------------------------------------------------------------------------

void unregisterSIDMenuBar(std::string sid)
{
    bool service_exists = core::tools::id::exist(sid);
    SIGHT_INFO_IF("The menubar '" + sid + "' does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(sid);
        SIGHT_ASSERT(
            "The service '" + sid + "' must be stopped before unregistering the menuBar.",
            service->stopped()
        );
    }

    SIGHT_ASSERT(
        "The menuBar '" + sid + "' is not registered in the container map, it cannot be removed.",
        m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end()
    );

    // Removes container in SID container map
    m_globalSIDToFwMenuBar.erase(sid);
}

//-----------------------------------------------------------------------------

ui::container::menubar::sptr getSIDMenuBar(std::string sid)
{
    SIGHT_ASSERT(
        "The menuBar '" + sid + "' is not registered in the container map.",
        m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end()
    );
    // returns container in SID container map
    return m_globalSIDToFwMenuBar[sid];
}

//-----------------------------------------------------------------------------

void registerSIDToolBar(std::string sid, ui::container::toolbar::sptr toolBar)
{
    SIGHT_ASSERT(
        "The toolBar '" + sid + "' is already registered in the container map, it cannot be added twice.",
        m_globalSIDToFwToolBar.find(sid) == m_globalSIDToFwToolBar.end()
    );
    m_globalSIDToFwToolBar[sid] = toolBar;
}

//-----------------------------------------------------------------------------

void unregisterSIDToolBar(std::string sid)
{
    bool service_exists = core::tools::id::exist(sid);
    SIGHT_INFO_IF("The toolBar '" + sid + "' does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(sid);
        SIGHT_ASSERT(
            "The service '" + sid + "' must be stopped before unregistering the menuBar.",
            service->stopped()
        );
    }

    SIGHT_ASSERT(
        "The toolBar '" + sid + "' is not registered in the container map, it cannot be removed.",
        m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end()
    );

    // Removes container in SID container map
    m_globalSIDToFwToolBar.erase(sid);
}

//-----------------------------------------------------------------------------

ui::container::toolbar::sptr getSIDToolBar(std::string sid)
{
    SIGHT_ASSERT(
        "The toolBar '" + sid + "' is not registered in the container map.",
        m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end()
    );
    // returns container in SID container map
    return m_globalSIDToFwToolBar[sid];
}

//-----------------------------------------------------------------------------

void registerSIDMenu(std::string sid, ui::container::menu::sptr menu)
{
    SIGHT_ASSERT(
        "The menu '" + sid + "' is already registered in the container map, it cannot be added twice.",
        m_globalSIDToFwMenu.find(sid) == m_globalSIDToFwMenu.end()
    );
    m_globalSIDToFwMenu[sid] = menu;
}

//-----------------------------------------------------------------------------

void unregisterSIDMenu(std::string sid)
{
    bool service_exists = core::tools::id::exist(sid);
    SIGHT_INFO_IF("The menu " << sid << " does not exist.", !service_exists);
    if(service_exists)
    {
        service::base::sptr service = service::get(sid);
        SIGHT_ASSERT("The service '" + sid + "' must be stopped before unregistering the menu.", service->stopped());
    }

    SIGHT_ASSERT(
        "The menu '" + sid + "' is not registered in the container map, it cannot be removed.",
        m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end()
    );

    // Removes container in SID container map
    m_globalSIDToFwMenu.erase(sid);
}

//-----------------------------------------------------------------------------

ui::container::menu::sptr getSIDMenu(std::string sid)
{
    SIGHT_ASSERT(
        "The menu '" + sid + "' is not registered in the container map.",
        m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end()
    );
    // returns container in SID container map
    return m_globalSIDToFwMenu[sid];
}

//-----------------------------------------------------------------------------

void registerActionSIDToParentSID(std::string actionSid, std::string parentSid)
{
    if(m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        // Action already exist in map
        SIGHT_ASSERT(
            std::string("The action '") + actionSid + "' already exists for the parent '" + parentSid + "'",
            std::find(
                m_actionSIDToParentSID[actionSid].begin(),
                m_actionSIDToParentSID[actionSid].end(),
                parentSid
            ) == m_actionSIDToParentSID[actionSid].end()
        );
    }

    m_actionSIDToParentSID[actionSid].push_back(parentSid);
}

//-----------------------------------------------------------------------------

void unregisterActionSIDToParentSID(std::string actionSid, std::string parentSid)
{
    SIGHT_ASSERT(
        "The action '" + actionSid + "' is not registered.",
        m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end()
    );

    if(m_actionSIDToParentSID[actionSid].size() == 1)
    {
        // Action has one parent
        bool service_exists = core::tools::id::exist(actionSid);
        SIGHT_INFO_IF("The service '" + actionSid + "' does not exist.", !service_exists);
        if(service_exists)
        {
            service::base::sptr service = service::get(actionSid);
            SIGHT_ASSERT(
                "The service '" + actionSid + "' must be stopped before unregistering the action.",
                service->stopped()
            );
        }

        m_actionSIDToParentSID.erase(actionSid);
    }
    else
    {
        // Action has several parents
        auto iter =
            std::find(m_actionSIDToParentSID[actionSid].begin(), m_actionSIDToParentSID[actionSid].end(), parentSid);
        SIGHT_ASSERT(
            std::string("The action with the sid '") + actionSid + "' has no parent named '" + parentSid + "'",
            iter != m_actionSIDToParentSID[actionSid].end()
        );
        m_actionSIDToParentSID[actionSid].erase(iter);
    }
}

//-----------------------------------------------------------------------------

void actionServiceStopping(std::string actionSid)
{
    if(m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(const std::string& parentSid : parentSids)
        {
            bool service_exists = core::tools::id::exist(parentSid);
            SIGHT_INFO_IF(
                std::string("The service '") + parentSid + "' managing the action '" + actionSid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service  = service::get(parentSid);
                ui::menu::sptr menuSrv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbarSrv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menuSrv)
                {
                    menuSrv->actionServiceStopping(actionSid);
                }
                else if(toolbarSrv)
                {
                    toolbarSrv->actionServiceStopping(actionSid);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parentSid + "' managing the action '"
                        + actionSid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void actionServiceStarting(std::string actionSid)
{
    if(m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(const std::string& parentSid : parentSids)
        {
            bool service_exists = core::tools::id::exist(parentSid);
            SIGHT_INFO_IF(
                std::string("The service '") + parentSid + "' managing the action '" + actionSid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service  = service::get(parentSid);
                ui::menu::sptr menuSrv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbarSrv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menuSrv)
                {
                    menuSrv->actionServiceStarting(actionSid);
                }
                else if(toolbarSrv)
                {
                    toolbarSrv->actionServiceStarting(actionSid);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parentSid + "' managing the action '"
                        + actionSid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void actionServiceSetChecked(std::string actionSid, bool isChecked)
{
    if(m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(const std::string& parentSid : parentSids)
        {
            bool service_exists = core::tools::id::exist(parentSid);
            SIGHT_INFO_IF(
                std::string("The service '") + parentSid + "' managing the action '" + actionSid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service  = service::get(parentSid);
                ui::menu::sptr menuSrv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbarSrv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menuSrv)
                {
                    menuSrv->actionServiceSetChecked(actionSid, isChecked);
                }
                else if(toolbarSrv)
                {
                    toolbarSrv->actionServiceSetChecked(actionSid, isChecked);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parentSid + "' managing the action '"
                        + actionSid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void actionServiceSetEnabled(std::string actionSid, bool isEnabled)
{
    if(m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(const std::string& parentSid : parentSids)
        {
            bool service_exists = core::tools::id::exist(parentSid);
            SIGHT_INFO_IF(
                std::string("The service '") + parentSid + "' managing the action '" + actionSid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service  = service::get(parentSid);
                ui::menu::sptr menuSrv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbarSrv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menuSrv)
                {
                    menuSrv->actionServiceSetEnabled(actionSid, isEnabled);
                }
                else if(toolbarSrv)
                {
                    toolbarSrv->actionServiceSetEnabled(actionSid, isEnabled);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parentSid + "' managing the action '"
                        + actionSid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void actionServiceSetVisible(std::string actionSid, bool isVisible)
{
    if(m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(const std::string& parentSid : parentSids)
        {
            bool service_exists = core::tools::id::exist(parentSid);
            SIGHT_INFO_IF(
                std::string("The service '") + parentSid + "' managing the action '" + actionSid
                + "' does not exist.",
                !service_exists
            );
            if(service_exists)
            {
                service::base::sptr service  = service::get(parentSid);
                ui::menu::sptr menuSrv       = std::dynamic_pointer_cast<ui::menu>(service);
                ui::toolbar::sptr toolbarSrv = std::dynamic_pointer_cast<ui::toolbar>(service);
                if(menuSrv)
                {
                    menuSrv->actionServiceSetVisible(actionSid, isVisible);
                }
                else if(toolbarSrv)
                {
                    toolbarSrv->actionServiceSetVisible(actionSid, isVisible);
                }
                else
                {
                    SIGHT_FATAL(
                        std::string("The service '") + parentSid + "' managing the action '"
                        + actionSid + "' must be managed by a menu or a toolBar"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::registry
