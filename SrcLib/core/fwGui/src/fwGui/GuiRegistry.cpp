/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IMenuSrv.hpp"
#include "fwGui/IToolBarSrv.hpp"
#include "fwGui/GuiRegistry.hpp"

namespace fwGui
{

GuiRegistry::ContainerMapType       GuiRegistry::m_globalSIDToFwContainer;
GuiRegistry::ContainerMapType       GuiRegistry::m_globalWIDToFwContainer;
GuiRegistry::MenuBarMapType         GuiRegistry::m_globalSIDToFwMenuBar;
GuiRegistry::ToolBarMapType         GuiRegistry::m_globalSIDToFwToolBar;
GuiRegistry::MenuMapType            GuiRegistry::m_globalSIDToFwMenu;
GuiRegistry::ActionToParentMapType  GuiRegistry::m_actionSIDToParentSID;

GuiRegistry::GuiRegistry()
{}

//-----------------------------------------------------------------------------

GuiRegistry::~GuiRegistry()
{}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDContainer(std::string sid , ::fwGui::container::fwContainer::sptr container)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<sid<<" already exists in SID container map.",
            m_globalSIDToFwContainer.find(sid) == m_globalSIDToFwContainer.end());
    m_globalSIDToFwContainer[sid] = container;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDContainer(std::string sid)
{
    bool service_exists = ::fwTools::UUID::exist(sid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<sid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid ) ;
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregister container.",service->isStopped());
    }

    OSLM_ASSERT("Sorry, fwContainer for "<<sid<<" not exists in SID container map.",
            m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end());

    // Removes container in SID container map
    m_globalSIDToFwContainer.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr GuiRegistry::getSIDContainer(std::string sid)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<sid<<" not exists in SID container map.",
            m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end());
    // returns container in SID container map
    return  m_globalSIDToFwContainer[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerWIDContainer(std::string wid , ::fwGui::container::fwContainer::sptr container)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<wid<<" already exists in WID container map.",
            m_globalWIDToFwContainer.find(wid) == m_globalWIDToFwContainer.end());
    m_globalWIDToFwContainer[wid] = container;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterWIDContainer(std::string wid)
{
    OSLM_ASSERT("Sorry, fwContainer with wid "<<wid<<" not exists in WID container map.",
            m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end());

    // Removes container in WID container map
    m_globalWIDToFwContainer.erase(wid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr GuiRegistry::getWIDContainer(std::string wid)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<wid<<" not exists in WID container map.",
            m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end());
    // returns container in WID container map
    return  m_globalWIDToFwContainer[wid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDMenuBar(std::string sid , ::fwGui::container::fwMenuBar::sptr menuBar)
{
    OSLM_ASSERT("Sorry, fwMenuBar for "<<sid<<" already exists in SID menuBar map.",
            m_globalSIDToFwMenuBar.find(sid) == m_globalSIDToFwMenuBar.end());
    m_globalSIDToFwMenuBar[sid] = menuBar;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDMenuBar(std::string sid)
{
    bool service_exists = ::fwTools::UUID::exist(sid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<sid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid ) ;
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregister menuBar.",service->isStopped());
    }

    OSLM_ASSERT("Sorry, fwMenuBar for "<<sid<<" not exists in SID menuBar map.",
                m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end());

    // Removes container in SID container map
    m_globalSIDToFwMenuBar.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwMenuBar::sptr GuiRegistry::getSIDMenuBar(std::string sid)
{
    OSLM_ASSERT("Sorry, fwMenuBar for "<<sid<<" not exists in SID menuBar map.",
                m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end());
    // returns container in SID container map
    return  m_globalSIDToFwMenuBar[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDToolBar(std::string sid , ::fwGui::container::fwToolBar::sptr toolBar)
{
    OSLM_ASSERT("Sorry, fwToolBar for "<<sid<<" already exists in SID toolBar map.",
            m_globalSIDToFwToolBar.find(sid) == m_globalSIDToFwToolBar.end());
    m_globalSIDToFwToolBar[sid] = toolBar;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDToolBar(std::string sid)
{
    bool service_exists = ::fwTools::UUID::exist(sid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<sid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid ) ;
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregister toolBar.",service->isStopped());
    }

    OSLM_ASSERT("Sorry, fwToolBar for "<<sid<<" not exists in SID toolBar map.",
                m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end());

    // Removes container in SID container map
    m_globalSIDToFwToolBar.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwToolBar::sptr GuiRegistry::getSIDToolBar(std::string sid)
{
    OSLM_ASSERT("Sorry, fwToolBar for "<<sid<<" not exists in SID toolBar map.",
                m_globalSIDToFwToolBar.find(sid) != m_globalSIDToFwToolBar.end());
    // returns container in SID container map
    return  m_globalSIDToFwToolBar[sid];
}


//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDMenu(std::string sid , ::fwGui::container::fwMenu::sptr menu)
{
    OSLM_ASSERT("Sorry, fwMenu for "<<sid<<" already exists in SID menu map.",
            m_globalSIDToFwMenu.find(sid) == m_globalSIDToFwMenu.end());
    m_globalSIDToFwMenu[sid] = menu;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterSIDMenu(std::string sid)
{
    bool service_exists = ::fwTools::UUID::exist(sid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<sid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( sid ) ;
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregister menu.",service->isStopped());
    }

    OSLM_ASSERT("Sorry, fwMenu for "<<sid<<" not exists in SID menu map.",
                m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end());

    // Removes container in SID container map
    m_globalSIDToFwMenu.erase(sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwMenu::sptr GuiRegistry::getSIDMenu(std::string sid)
{
    OSLM_ASSERT("Sorry, fwMenu for "<<sid<<" not exists in SID menu map.",
                m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end());
    // returns container in SID container map
    return  m_globalSIDToFwMenu[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerActionSIDToParentSID(std::string actionSid , std::string parentSid)
{
    if ( m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end())
    {
        // Action already exist in map
        OSLM_ASSERT("Sorry, action " << actionSid << " already exists for parent " << parentSid,
                    std::find(m_actionSIDToParentSID[actionSid].begin(), m_actionSIDToParentSID[actionSid].end(), parentSid) == m_actionSIDToParentSID[actionSid].end());
    }
    m_actionSIDToParentSID[actionSid].push_back(parentSid);
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterActionSIDToParentSID(std::string actionSid, std::string parentSid)
{

    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                    m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    if (m_actionSIDToParentSID[actionSid].size() == 1)
    {
        // Action has one parent
        bool service_exists = ::fwTools::UUID::exist(actionSid, ::fwTools::UUID::SIMPLE );
        OSLM_INFO_IF("Service "<<actionSid <<" not exists.",!service_exists );
        if(service_exists)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( actionSid ) ;
            OSLM_ASSERT("Service "<<actionSid<<" must be stopped before unregister action.",service->isStopped());
        }
        m_actionSIDToParentSID.erase(actionSid);
    }
    else
    {
        // Action has several parents
        ParentSidsType::iterator iter = std::find(m_actionSIDToParentSID[actionSid].begin(), m_actionSIDToParentSID[actionSid].end(), parentSid);
        OSLM_ASSERT("Sorry, action for "<<actionSid<<" has no parent "<< parentSid, iter != m_actionSIDToParentSID[actionSid].end());
        m_actionSIDToParentSID[actionSid].erase(iter);
    }
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceStopping(std::string actionSid)
{
    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                    m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];


    BOOST_FOREACH(std::string parentSid, parentSids)
    {
        bool service_exists = ::fwTools::UUID::exist(parentSid, ::fwTools::UUID::SIMPLE );
        OSLM_INFO_IF("Service "<<parentSid <<" not exists.",!service_exists );
        if(service_exists)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( parentSid ) ;
            ::fwGui::IMenuSrv::sptr menuSrv = ::fwGui::IMenuSrv::dynamicCast(service);
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

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceStarting(std::string actionSid)
{
    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];


    BOOST_FOREACH(std::string parentSid, parentSids)
    {
        bool service_exists = ::fwTools::UUID::exist(parentSid, ::fwTools::UUID::SIMPLE );
        OSLM_INFO_IF("Service "<<parentSid <<" not exists.",!service_exists );
        if(service_exists)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( parentSid ) ;
            ::fwGui::IMenuSrv::sptr menuSrv = ::fwGui::IMenuSrv::dynamicCast(service);
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

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceSetActive(std::string actionSid, bool isActive)
{
    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];


    BOOST_FOREACH(std::string parentSid, parentSids)
    {
        bool service_exists = ::fwTools::UUID::exist(parentSid, ::fwTools::UUID::SIMPLE );
        OSLM_INFO_IF("Service "<<parentSid <<" not exists.",!service_exists );
        if(service_exists)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( parentSid ) ;
            ::fwGui::IMenuSrv::sptr menuSrv = ::fwGui::IMenuSrv::dynamicCast(service);
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

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceSetExecutable(std::string actionSid, bool isExecutable)
{
    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    ParentSidsType parentSids = m_actionSIDToParentSID[actionSid];


    BOOST_FOREACH(std::string parentSid, parentSids)
    {
        bool service_exists = ::fwTools::UUID::exist(parentSid, ::fwTools::UUID::SIMPLE );
        OSLM_INFO_IF("Service "<<parentSid <<" not exists.",!service_exists );
        if(service_exists)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( parentSid ) ;
            ::fwGui::IMenuSrv::sptr menuSrv = ::fwGui::IMenuSrv::dynamicCast(service);
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

//-----------------------------------------------------------------------------


}
