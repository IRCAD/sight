/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "gui/GuiRegistry.hpp"

#include "gui/IMenuSrv.hpp"
#include "gui/IToolBarSrv.hpp"

#include <core/tools/fwID.hpp>

#include <services/macros.hpp>
#include <services/op/Get.hpp>

namespace sight::gui
{

GuiRegistry::ContainerMapType GuiRegistry::m_globalSIDToFwContainer;
GuiRegistry::ContainerMapType GuiRegistry::m_globalWIDToFwContainer;
GuiRegistry::MenuBarMapType GuiRegistry::m_globalSIDToFwMenuBar;
GuiRegistry::ToolBarMapType GuiRegistry::m_globalSIDToFwToolBar;
GuiRegistry::MenuMapType GuiRegistry::m_globalSIDToFwMenu;
GuiRegistry::ActionToParentMapType GuiRegistry::m_actionSIDToParentSID;

GuiRegistry::GuiRegistry()
{
}

//-----------------------------------------------------------------------------

GuiRegistry::~GuiRegistry()
{
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDContainer(std::string sid, gui::container::fwContainer::sptr container)
{
    SLM_ASSERT("The service '"+sid+"' is already registered in the container map, it cannot be added twice.",
               m_globalSIDToFwContainer.find(sid) == m_globalSIDToFwContainer.end());
    m_globalSIDToFwContainer[sid] = container;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDContainer(std::string sid)
{
    bool service_exists = core::tools::fwID::exist(sid );
    SLM_INFO_IF("The service '"+sid +"' does not exist.", !service_exists );
    if(service_exists)
    {
        services::IService::sptr service = services::get( sid );
        SLM_ASSERT("The service '"+sid+"' must be stopped before unregistering the container.", service->isStopped());
    }

    SLM_ASSERT("The service '"+sid+"' is not registered in the container map, it cannot be removed.",
               m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end());

    // Removes container in SID container map
    m_globalSIDToFwContainer.erase(sid);
}

//-----------------------------------------------------------------------------

gui::container::fwContainer::sptr GuiRegistry::getSIDContainer(std::string sid)
{
    // Returns container in SID container map, null if  not found.
    if (!hasSIDContainer(sid))
    {
        return nullptr;
    }
    return m_globalSIDToFwContainer[sid];
}

//-----------------------------------------------------------------------------

bool GuiRegistry::hasSIDContainer(std::string sid)
{
    // Returns 'true' if the specified 'sid' is found in the SID container map, else 'false'.
    return m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end();
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerWIDContainer(std::string wid, gui::container::fwContainer::sptr container)
{
    SLM_ASSERT("The wid container '"+wid+"' is already registered in the container map, it cannot be added twice.",
               m_globalWIDToFwContainer.find(wid) == m_globalWIDToFwContainer.end());
    m_globalWIDToFwContainer[wid] = container;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterWIDContainer(std::string wid)
{
    SLM_ASSERT("The wid container '"+wid+"' is not registered in the container map.",
               m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end());

    // Removes container in WID container map
    m_globalWIDToFwContainer.erase(wid);
}

//-----------------------------------------------------------------------------

gui::container::fwContainer::sptr GuiRegistry::getWIDContainer(std::string wid)
{
    // Returns container in WID container map, null if  not found.
    if (!hasWIDContainer(wid))
    {
        return nullptr;
    }
    return m_globalWIDToFwContainer[wid];
}

//-----------------------------------------------------------------------------

bool GuiRegistry::hasWIDContainer(std::string wid)
{
    // Returns 'true' if the specified 'wid' is found in the WID container map, else 'false'.
    return m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end();
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDMenuBar(std::string sid, gui::container::fwMenuBar::sptr menuBar)
{
    SLM_ASSERT("The menuBar '" + sid + "' is already registered in the container map, it cannot be added twice.",
               m_globalSIDToFwMenuBar.find(sid) == m_globalSIDToFwMenuBar.end());
    m_globalSIDToFwMenuBar[sid] = menuBar;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDMenuBar(std::string sid)
{
    bool service_exists = core::tools::fwID::exist(sid );
    SLM_INFO_IF("The menubar '" + sid + "' does not exist.", !service_exists );
    if(service_exists)
    {
        services::IService::sptr service = services::get( sid );
        SLM_ASSERT("The service '" + sid + "' must be stopped before unregistering the menuBar.",
                   service->isStopped());
    }

    SLM_ASSERT("The menuBar '"+sid+"' is not registered in the container map, it cannot be removed.",
               m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end());

    // Removes container in SID container map
    m_globalSIDToFwMenuBar.erase(sid);
}

//-----------------------------------------------------------------------------

gui::container::fwMenuBar::sptr GuiRegistry::getSIDMenuBar(std::string sid)
{
    SLM_ASSERT("The menuBar '"+sid+"' is not registered in the container map.",
               m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end());
    // returns container in SID container map
    return m_globalSIDToFwMenuBar[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDToolBar(std::string sid, gui::container::fwToolBar::sptr toolBar)
{
    SLM_ASSERT("The toolBar '" + sid + "' is already registered in the container map, it cannot be added twice.",
               m_globalSIDToFwToolBar.find(sid) == m_globalSIDToFwToolBar.end());
    m_globalSIDToFwToolBar[sid] = toolBar;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDToolBar(std::string sid)
{
    bool service_exists = core::tools::fwID::exist(sid );
    SLM_INFO_IF("The toolBar '" + sid + "' does not exist.", !service_exists );
    if(service_exists)
    {
        services::IService::sptr service = services::get( sid );
        SLM_ASSERT("The service '" + sid + "' must be stopped before unregistering the menuBar.",
                   service->isStopped());
    }

    SLM_ASSERT("The toolBar '"+sid+"' is not registered in the container map, it cannot be removed.",
               m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end());

    // Removes container in SID container map
    m_globalSIDToFwToolBar.erase(sid);
}

//-----------------------------------------------------------------------------

gui::container::fwToolBar::sptr GuiRegistry::getSIDToolBar(std::string sid)
{
    SLM_ASSERT("The toolBar '"+sid+"' is not registered in the container map.",
               m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end());
    // returns container in SID container map
    return m_globalSIDToFwToolBar[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDMenu(std::string sid, gui::container::fwMenu::sptr menu)
{
    SLM_ASSERT("The menu '" + sid + "' is already registered in the container map, it cannot be added twice.",
               m_globalSIDToFwMenu.find(sid) == m_globalSIDToFwMenu.end());
    m_globalSIDToFwMenu[sid] = menu;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDMenu(std::string sid)
{
    bool service_exists = core::tools::fwID::exist(sid );
    SLM_INFO_IF("The menu "<<sid <<" does not exist.", !service_exists );
    if(service_exists)
    {
        services::IService::sptr service = services::get( sid );
        SLM_ASSERT("The service '" + sid + "' must be stopped before unregistering the menu.", service->isStopped());
    }

    SLM_ASSERT("The menu '"+sid+"' is not registered in the container map, it cannot be removed.",
               m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end());

    // Removes container in SID container map
    m_globalSIDToFwMenu.erase(sid);
}

//-----------------------------------------------------------------------------

gui::container::fwMenu::sptr GuiRegistry::getSIDMenu(std::string sid)
{
    SLM_ASSERT("The menu '"+sid+"' is not registered in the container map.",
               m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end());
    // returns container in SID container map
    return m_globalSIDToFwMenu[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerActionSIDToParentSID(std::string actionSid, std::string parentSid)
{
    if ( m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        // Action already exist in map
        SLM_ASSERT("The action '" + actionSid + "' already exists for the parent '" + parentSid+"'",
                   std::find(m_actionSIDToParentSID[actionSid].begin(), m_actionSIDToParentSID[actionSid].end(),
                             parentSid) == m_actionSIDToParentSID[actionSid].end());
    }
    m_actionSIDToParentSID[actionSid].push_back(parentSid);
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterActionSIDToParentSID(std::string actionSid, std::string parentSid)
{

    SLM_ASSERT("The action '"+actionSid+"' is not registered.",
               m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    if (m_actionSIDToParentSID[actionSid].size() == 1)
    {
        // Action has one parent
        bool service_exists = core::tools::fwID::exist(actionSid );
        SLM_INFO_IF("The service '" + actionSid + "' does not exist.", !service_exists );
        if(service_exists)
        {
            services::IService::sptr service = services::get( actionSid );
            SLM_ASSERT("The service '" + actionSid + "' must be stopped before unregistering the action.",
                       service->isStopped());
        }
        m_actionSIDToParentSID.erase(actionSid);
    }
    else
    {
        // Action has several parents
        ParentSidsType::iterator iter =
            std::find(m_actionSIDToParentSID[actionSid].begin(), m_actionSIDToParentSID[actionSid].end(), parentSid);
        SLM_ASSERT("The action with the sid '"+actionSid+"' has no parent named '"+ parentSid +"'",
                   iter != m_actionSIDToParentSID[actionSid].end());
        m_actionSIDToParentSID[actionSid].erase(iter);
    }
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceStopping(std::string actionSid)
{
    if( m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end() )
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(std::string parentSid :  parentSids)
        {
            bool service_exists = core::tools::fwID::exist(parentSid );
            SLM_INFO_IF("The service '"+parentSid +"' managing the action '" + actionSid+"' does not exist.",
                        !service_exists );
            if(service_exists)
            {
                services::IService::sptr service  = services::get( parentSid );
                gui::IMenuSrv::sptr menuSrv       = gui::IMenuSrv::dynamicCast(service);
                gui::IToolBarSrv::sptr toolbarSrv = gui::IToolBarSrv::dynamicCast(service);
                if (menuSrv)
                {
                    menuSrv->actionServiceStopping(actionSid);
                }
                else if (toolbarSrv)
                {
                    toolbarSrv->actionServiceStopping(actionSid);
                }
                else
                {
                    SLM_FATAL(
                        "The service '"+parentSid+"' managing the action '"+actionSid+
                        "' must be managed by a menu or a toolBar");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceStarting(std::string actionSid)
{
    if( m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end() )
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(std::string parentSid :  parentSids)
        {
            bool service_exists = core::tools::fwID::exist(parentSid );
            SLM_INFO_IF("The service '"+parentSid +"' managing the action '" + actionSid+"' does not exist.",
                        !service_exists );
            if(service_exists)
            {
                services::IService::sptr service  = services::get( parentSid );
                gui::IMenuSrv::sptr menuSrv       = gui::IMenuSrv::dynamicCast(service);
                gui::IToolBarSrv::sptr toolbarSrv = gui::IToolBarSrv::dynamicCast(service);
                if (menuSrv)
                {
                    menuSrv->actionServiceStarting(actionSid);
                }
                else if (toolbarSrv)
                {
                    toolbarSrv->actionServiceStarting(actionSid);
                }
                else
                {
                    SLM_FATAL(
                        "The service '"+parentSid+"' managing the action '"+actionSid+
                        "' must be managed by a menu or a toolBar");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceSetActive(std::string actionSid, bool isActive)
{
    if( m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end() )
    {

        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(std::string parentSid :  parentSids)
        {
            bool service_exists = core::tools::fwID::exist(parentSid );
            SLM_INFO_IF("The service '"+parentSid +"' managing the action '" + actionSid+"' does not exist.",
                        !service_exists );
            if(service_exists)
            {
                services::IService::sptr service  = services::get( parentSid );
                gui::IMenuSrv::sptr menuSrv       = gui::IMenuSrv::dynamicCast(service);
                gui::IToolBarSrv::sptr toolbarSrv = gui::IToolBarSrv::dynamicCast(service);
                if (menuSrv)
                {
                    menuSrv->actionServiceSetActive(actionSid, isActive);
                }
                else if (toolbarSrv)
                {
                    toolbarSrv->actionServiceSetActive(actionSid, isActive);
                }
                else
                {
                    SLM_FATAL(
                        "The service '"+parentSid+"' managing the action '"+actionSid+
                        "' must be managed by a menu or a toolBar");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceSetExecutable(std::string actionSid, bool isExecutable)
{
    if( m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end() )
    {

        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(std::string parentSid :  parentSids)
        {
            bool service_exists = core::tools::fwID::exist(parentSid );
            SLM_INFO_IF("The service '"+parentSid +"' managing the action '" + actionSid+"' does not exist.",
                        !service_exists );
            if(service_exists)
            {
                services::IService::sptr service  = services::get( parentSid );
                gui::IMenuSrv::sptr menuSrv       = gui::IMenuSrv::dynamicCast(service);
                gui::IToolBarSrv::sptr toolbarSrv = gui::IToolBarSrv::dynamicCast(service);
                if (menuSrv)
                {
                    menuSrv->actionServiceSetExecutable(actionSid, isExecutable);
                }
                else if (toolbarSrv)
                {
                    toolbarSrv->actionServiceSetExecutable(actionSid, isExecutable);
                }
                else
                {
                    SLM_FATAL(
                        "The service '"+parentSid+"' managing the action '"+actionSid+
                        "' must be managed by a menu or a toolBar");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceSetVisible(std::string actionSid, bool isVisible)
{
    if( m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end() )
    {
        ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];

        for(std::string parentSid :  parentSids)
        {
            bool service_exists = core::tools::fwID::exist(parentSid );
            SLM_INFO_IF("The service '"+parentSid +"' managing the action '" + actionSid+"' does not exist.",
                        !service_exists );
            if(service_exists)
            {
                services::IService::sptr service  = services::get( parentSid );
                gui::IMenuSrv::sptr menuSrv       = gui::IMenuSrv::dynamicCast(service);
                gui::IToolBarSrv::sptr toolbarSrv = gui::IToolBarSrv::dynamicCast(service);
                if (menuSrv)
                {
                    menuSrv->actionServiceSetVisible(actionSid, isVisible);
                }
                else if (toolbarSrv)
                {
                    toolbarSrv->actionServiceSetVisible(actionSid, isVisible);
                }
                else
                {
                    SLM_FATAL(
                        "The service '"+parentSid+"' managing the action '"+actionSid+
                        "' must be managed by a menu or a toolBar");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

}
