/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IMenuSrv.hpp"
#include "fwGui/GuiRegistry.hpp"

namespace fwGui
{

GuiRegistry::ContainerMapType     GuiRegistry::m_globalSIDToFwContainer;
GuiRegistry::ContainerMapType     GuiRegistry::m_globalWIDToFwContainer;
GuiRegistry::MenuBarMapType       GuiRegistry::m_globalSIDToFwMenuBar;
GuiRegistry::MenuMapType          GuiRegistry::m_globalSIDToFwMenu;
GuiRegistry::ActionToMenuMapType  GuiRegistry::m_actionSIDToParentSID;

GuiRegistry::GuiRegistry()
{}

//-----------------------------------------------------------------------------

GuiRegistry::~GuiRegistry()
{}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDContainer(std::string sid , ::fwGui::fwContainer::sptr container)
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

::fwGui::fwContainer::sptr GuiRegistry::getSIDContainer(std::string sid)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<sid<<" not exists in SID container map.",
            m_globalSIDToFwContainer.find(sid) != m_globalSIDToFwContainer.end());
    // returns container in SID container map
    return  m_globalSIDToFwContainer[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerWIDContainer(std::string wid , ::fwGui::fwContainer::sptr container)
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

::fwGui::fwContainer::sptr GuiRegistry::getWIDContainer(std::string wid)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<wid<<" not exists in WID container map.",
            m_globalWIDToFwContainer.find(wid) != m_globalWIDToFwContainer.end());
    // returns container in WID container map
    return  m_globalWIDToFwContainer[wid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDMenuBar(std::string sid , ::fwGui::fwMenuBar::sptr menuBar)
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

::fwGui::fwMenuBar::sptr GuiRegistry::getSIDMenuBar(std::string sid)
{
    OSLM_ASSERT("Sorry, fwMenuBar for "<<sid<<" not exists in SID menuBar map.",
                m_globalSIDToFwMenuBar.find(sid) != m_globalSIDToFwMenuBar.end());
    // returns container in SID container map
    return  m_globalSIDToFwMenuBar[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerSIDMenu(std::string sid , ::fwGui::fwMenu::sptr menu)
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

::fwGui::fwMenu::sptr GuiRegistry::getSIDMenu(std::string sid)
{
    OSLM_ASSERT("Sorry, fwMenu for "<<sid<<" not exists in SID menu map.",
                m_globalSIDToFwMenu.find(sid) != m_globalSIDToFwMenu.end());
    // returns container in SID container map
    return  m_globalSIDToFwMenu[sid];
}

//-----------------------------------------------------------------------------

void GuiRegistry::registerActionSIDToParentSID(std::string actionSid , std::string parentSidm)
{
    OSLM_ASSERT("Sorry, action for "<<actionSid<<" already exists in SID action map.",
                m_actionSIDToParentSID.find(actionSid) == m_actionSIDToParentSID.end());
    m_actionSIDToParentSID[actionSid] = parentSidm;
}

//-----------------------------------------------------------------------------

void GuiRegistry::unregisterActionSIDToParentSID(std::string actionSid)
{
    bool service_exists = ::fwTools::UUID::exist(actionSid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<actionSid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( actionSid ) ;
        OSLM_ASSERT("Service "<<actionSid<<" must be stopped before unregister action.",service->isStopped());
    }

    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    // Removes container in SID container map
    m_actionSIDToParentSID.erase(actionSid);
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceStopping(std::string actionSid)
{
    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                    m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    std::string parentSid = m_actionSIDToParentSID[actionSid];
    bool service_exists = ::fwTools::UUID::exist(parentSid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<parentSid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( parentSid ) ;
        ::fwGui::IMenuSrv::sptr menuSrv = ::fwGui::IMenuSrv::dynamicCast(service);
        if (menuSrv)
        {
            menuSrv->actionServiceStopping(actionSid);
        }
    }
}

//-----------------------------------------------------------------------------

void GuiRegistry::actionServiceStarting(std::string actionSid)
{
    OSLM_ASSERT("Sorry, action for "<<actionSid<<" not exists in SID action map.",
                    m_actionSIDToParentSID.find(actionSid) != m_actionSIDToParentSID.end());

    std::string parentSid = m_actionSIDToParentSID[actionSid];
    bool service_exists = ::fwTools::UUID::exist(parentSid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<parentSid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( parentSid ) ;
        ::fwGui::IMenuSrv::sptr menuSrv = ::fwGui::IMenuSrv::dynamicCast(service);
        if (menuSrv)
        {
            menuSrv->actionServiceStarting(actionSid);
        }
    }
}

//-----------------------------------------------------------------------------


}
