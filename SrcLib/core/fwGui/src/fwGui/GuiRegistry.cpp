/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/GuiRegistry.hpp"

namespace fwGui
{

GuiRegistry::ContainerMapType GuiRegistry::m_globalSIDToFwContainer;
GuiRegistry::ContainerMapType GuiRegistry::m_globalWIDToFwContainer;

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
        OSLM_ASSERT("Service "<<sid<<" must be stopped before unregister container.",service->isStopped())
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

}
