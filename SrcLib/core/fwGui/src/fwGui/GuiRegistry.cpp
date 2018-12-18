/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGui/GuiRegistry.hpp"

#include "fwGui/IMenuSrv.hpp"
#include "fwGui/IToolBarSrv.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/fwID.hpp>

namespace fwGui
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

void GuiRegistry::registerSIDContainer(std::string sid, ::fwGui::container::fwContainer::sptr container)
{
    OSLM_ASSERT("A fwContainer with the sid "<<sid<<" already exists in the SID container map.",
                m_globalSIDToFwContainer.find(sid) == m_globalSIDToFwContainer.end());
    m_globalSIDToFwContainer[sid] = container;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDContainer(std::string sid)
{
    bool service_exists = ::fwTools::fwID::exist(sid );
    OSLM_INFO_IF("Service "<<sid <<" does not exist.", !service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid );
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregistering the container.", service->isStopped());
    }

    OSLM_ASSERT("No fwContainer with the sid "<<sid<<" exists in the SID container map.",
                m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end());

    // Removes container in SID container map
    m_globalSIDToFwContainer.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr GuiRegistry::getSIDContainer(std::string sid)
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

void GuiRegistry::registerWIDContainer(std::string wid, ::fwGui::container::fwContainer::sptr container)
{
    OSLM_ASSERT("A fwContainer with the wid "<<wid<<" already exists in the WID container map.",
                m_globalWIDToFwContainer.find(wid) == m_globalWIDToFwContainer.end());
    m_globalWIDToFwContainer[wid] = container;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterWIDContainer(std::string wid)
{
    OSLM_ASSERT("No fwContainer with the wid "<<wid<<" exists in the WID container map.",
                m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end());

    // Removes container in WID container map
    m_globalWIDToFwContainer.erase(wid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr GuiRegistry::getWIDContainer(std::string wid)
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

void GuiRegistry::registerSIDMenuBar(std::string sid, ::fwGui::container::fwMenuBar::sptr menuBar)
{
    OSLM_ASSERT("A fwMenuBar with the sid "<<sid<<" already exists in the SID menuBar map.",
                m_globalSIDToFwMenuBar.find(sid) == m_globalSIDToFwMenuBar.end());
    m_globalSIDToFwMenuBar[sid] = menuBar;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDMenuBar(std::string sid)
{
    bool service_exists = ::fwTools::fwID::exist(sid );
    OSLM_INFO_IF("Service "<<sid <<" does not exist.", !service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid );
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregistering the menuBar.", service->isStopped());
    }

    OSLM_ASSERT("No fwMenuBar with the sid "<<sid<<" exists in the SID menuBar map.",
                m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end());

    // Removes container in SID container map
    m_globalSIDToFwMenuBar.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwMenuBar::sptr GuiRegistry::getSIDMenuBar(std::string sid)
{
    OSLM_ASSERT("No fwMenuBar with the sid "<<sid<<" exists in the SID menuBar map.",
                m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end());
    // returns container in SID container map
    return m_globalSIDToFwMenuBar[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDToolBar(std::string sid, ::fwGui::container::fwToolBar::sptr toolBar)
{
    OSLM_ASSERT("A fwToolBar with the sid "<<sid<<" already exists in the SID toolBar map.",
                m_globalSIDToFwToolBar.find(sid) == m_globalSIDToFwToolBar.end());
    m_globalSIDToFwToolBar[sid] = toolBar;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDToolBar(std::string sid)
{
    bool service_exists = ::fwTools::fwID::exist(sid );
    OSLM_INFO_IF("Service "<<sid <<" does not exist.", !service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid );
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregistering the toolBar.", service->isStopped());
    }

    OSLM_ASSERT("No fwToolBar with the sid "<<sid<<"  exists in the SID toolBar map.",
                m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end());

    // Removes container in SID container map
    m_globalSIDToFwToolBar.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwToolBar::sptr GuiRegistry::getSIDToolBar(std::string sid)
{
    OSLM_ASSERT("No fwToolBar with the sid "<<sid<<" exists in the SID toolBar map.",
                m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end());
    // returns container in SID container map
    return m_globalSIDToFwToolBar[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDMenu(std::string sid, ::fwGui::container::fwMenu::sptr menu)
{
    OSLM_ASSERT("A fwMenu with the "<<sid<<" already exists in the SID menu map.",
                m_globalSIDToFwMenu.find(sid) == m_globalSIDToFwMenu.end());
    m_globalSIDToFwMenu[sid] = menu;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDMenu(std::string sid)
{
    bool service_exists = ::fwTools::fwID::exist(sid );
    OSLM_INFO_IF("Service "<<sid <<" does not exist.", !service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid );
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregistering the menu.", service->isStopped());
    }

    OSLM_ASSERT("No fwMenu with the sid "<<sid<<" exists in the SID menu map.",
                m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end());

    // Removes container in SID container map
    m_globalSIDToFwMenu.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwMenu::sptr GuiRegistry::getSIDMenu(std::string sid)
{
    OSLM_ASSERT("No fwMenu with the sid "<<sid<<" exists in the SID menu map.",
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
        OSLM_ASSERT("An action with the sid " << actionSid << " already exists for the parent " << parentSid,
                    std::find(m_actionSIDToParentSID[actionSid].begin(), m_actionSIDToParentSID[actionSid].end(),
                              parentSid) == m_actionSIDToParentSID[actionSid].end());
    }
    m_actionSIDToParentSID[actionSid].push_back(parentSid);
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterActionSIDToParentSID(std::string actionSid, std::string parentSid)
{

    OSLM_ASSERT("No action with the sid "<<actionSid<<" exists in the SID action map.",
                m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    if (m_actionSIDToParentSID[actionSid].size() == 1)
    {
        // Action has one parent
        bool service_exists = ::fwTools::fwID::exist(actionSid );
        OSLM_INFO_IF("Service "<<actionSid <<" does not exist.", !service_exists );
        if(service_exists)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( actionSid );
            OSLM_WARN_IF("Service "<<actionSid<<" must be stopped before unregistering the action.",
                         !service->isStopped());
        }
        m_actionSIDToParentSID.erase(actionSid);
    }
    else
    {
        // Action has several parents
        ParentSidsType::iterator iter =
            std::find(m_actionSIDToParentSID[actionSid].begin(), m_actionSIDToParentSID[actionSid].end(), parentSid);
        OSLM_ASSERT("The action with the sid "<<actionSid<<" has no parent named "<< parentSid,
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
            bool service_exists = ::fwTools::fwID::exist(parentSid );
            OSLM_INFO_IF("Service "<<parentSid <<" does not exist.", !service_exists );
            if(service_exists)
            {
                ::fwServices::IService::sptr service  = ::fwServices::get( parentSid );
                ::fwGui::IMenuSrv::sptr menuSrv       = ::fwGui::IMenuSrv::dynamicCast(service);
                ::fwGui::IToolBarSrv::sptr toolbarSrv = ::fwGui::IToolBarSrv::dynamicCast(service);
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
                    SLM_FATAL("Unknown service");
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
            bool service_exists = ::fwTools::fwID::exist(parentSid );
            OSLM_INFO_IF("Service "<<parentSid <<" does not exist.", !service_exists );
            if(service_exists)
            {
                ::fwServices::IService::sptr service  = ::fwServices::get( parentSid );
                ::fwGui::IMenuSrv::sptr menuSrv       = ::fwGui::IMenuSrv::dynamicCast(service);
                ::fwGui::IToolBarSrv::sptr toolbarSrv = ::fwGui::IToolBarSrv::dynamicCast(service);
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
                    SLM_FATAL("Unknown service");
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
            bool service_exists = ::fwTools::fwID::exist(parentSid );
            OSLM_INFO_IF("Service "<<parentSid <<" does not exist.", !service_exists );
            if(service_exists)
            {
                ::fwServices::IService::sptr service  = ::fwServices::get( parentSid );
                ::fwGui::IMenuSrv::sptr menuSrv       = ::fwGui::IMenuSrv::dynamicCast(service);
                ::fwGui::IToolBarSrv::sptr toolbarSrv = ::fwGui::IToolBarSrv::dynamicCast(service);
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
                    SLM_FATAL("Unknown service");
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
            bool service_exists = ::fwTools::fwID::exist(parentSid );
            OSLM_INFO_IF("Service "<<parentSid <<" does not exist.", !service_exists );
            if(service_exists)
            {
                ::fwServices::IService::sptr service  = ::fwServices::get( parentSid );
                ::fwGui::IMenuSrv::sptr menuSrv       = ::fwGui::IMenuSrv::dynamicCast(service);
                ::fwGui::IToolBarSrv::sptr toolbarSrv = ::fwGui::IToolBarSrv::dynamicCast(service);
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
                    SLM_FATAL("Unknown service");
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
            bool service_exists = ::fwTools::fwID::exist(parentSid );
            OSLM_INFO_IF("Service "<<parentSid <<" does not exist.", !service_exists );
            if(service_exists)
            {
                ::fwServices::IService::sptr service  = ::fwServices::get( parentSid );
                ::fwGui::IMenuSrv::sptr menuSrv       = ::fwGui::IMenuSrv::dynamicCast(service);
                ::fwGui::IToolBarSrv::sptr toolbarSrv = ::fwGui::IToolBarSrv::dynamicCast(service);
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
                    SLM_FATAL("Unknown service");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

}
