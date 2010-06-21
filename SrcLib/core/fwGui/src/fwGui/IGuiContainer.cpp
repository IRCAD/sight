/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IGuiContainer.hpp"

namespace fwGui
{

IGuiContainer::ContainerMapType IGuiContainer::m_globalUIDToContainer;

IGuiContainer::IGuiContainer()
{}

//-----------------------------------------------------------------------------

IGuiContainer::~IGuiContainer()
{}

//-----------------------------------------------------------------------------

::fwGui::fwContainer::sptr IGuiContainer::getContainer()
{
    SLM_FATAL_IF("Sorry, parent fwContainer not yet initialized, please initGuiParentContainer before", !m_container ) ;
    return m_container ;
}

//-----------------------------------------------------------------------------

void IGuiContainer::initGuiParentContainer()
{
    SLM_FATAL_IF("GuiContainer not fund for this Service ",
            m_globalUIDToContainer.find(this->getUUID()) == m_globalUIDToContainer.end());

    OSLM_TRACE( "GuiContainer fund for this Service " <<  this->getUUID() );
    m_container = m_globalUIDToContainer[this->getUUID()];
    SLM_ASSERT("Sorry, fwContainer is not correctly initialized", m_container);
}

//-----------------------------------------------------------------------------

void IGuiContainer::resetGuiParentContainer()
{
    if(m_container)
    {
        SLM_TRACE("Cleaning container");
        m_container->clean();
    }
}

//-----------------------------------------------------------------------------

void IGuiContainer::registerContainer(std::string uid , ::fwGui::fwContainer::sptr container)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<uid<<" already exists in SubUIDToContainer map.",
            m_subUIDToContainer.find(uid) == m_subUIDToContainer.end());
    m_subUIDToContainer[uid] = container;

    ::fwGui::IGuiContainer::registerGlobalContainer(uid, container);
}

//-----------------------------------------------------------------------------

void IGuiContainer::unregisterContainer(std::string uid)
{
    bool service_exists = ::fwTools::UUID::exist(uid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<uid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( uid ) ;
        service->stop();
    }

    OSLM_ASSERT("Sorry, fwContainer for "<<uid<<" not exists in SubUIDToContainer map.",
            m_subUIDToContainer.find(uid) != m_subUIDToContainer.end());

    ::fwGui::fwContainer::sptr container = m_subUIDToContainer[uid];
    SLM_ASSERT("Sorry, fwContainer is not correctly initialized", container);

    // Destroys the container safely
    container->destroyContainer();

    // Removes container in internal map
    m_subUIDToContainer.erase(uid);

    ::fwGui::IGuiContainer::unregisterGlobalContainer(uid);
}

//-----------------------------------------------------------------------------

void IGuiContainer::unregisterAllContainer()
{
    ContainerMapType subUIDToContainer = m_subUIDToContainer;
    BOOST_FOREACH( ContainerMapType::value_type uidContainerElmt, subUIDToContainer)
    {
        this->unregisterContainer(uidContainerElmt.first);
    }
    m_subUIDToContainer.clear();
}

//-----------------------------------------------------------------------------

void IGuiContainer::registerGlobalContainer(std::string uid, ::fwGui::fwContainer::sptr container)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<uid<<" already exists in GlobalUIDToContainer map.",
            m_globalUIDToContainer.find(uid) == m_globalUIDToContainer.end());
    m_globalUIDToContainer[uid] = container;
}

//-----------------------------------------------------------------------------

void IGuiContainer::unregisterGlobalContainer(std::string uid)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<uid<<" not exists in GlobalUIDToContainer map.",
            m_globalUIDToContainer.find(uid) != m_globalUIDToContainer.end());
    // Removes container in global map
    m_globalUIDToContainer.erase(uid);
}

//-----------------------------------------------------------------------------

::fwGui::fwContainer::sptr IGuiContainer::getContainer(std::string uid)
{
    OSLM_ASSERT("Sorry, fwContainer for "<<uid<<" not exists in GlobalUIDToContainer map.",
            m_globalUIDToContainer.find(uid) != m_globalUIDToContainer.end());
    // returns container in global map
    return  m_globalUIDToContainer[uid];
}

}
