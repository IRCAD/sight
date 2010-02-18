/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <wx/frame.h>
#include <wx/app.h>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwWX/convert.hpp"
#include "fwWX/IGuiContainer.hpp"

namespace fwWX
{

IGuiContainer::ContainerMapType IGuiContainer::m_globalUIDToWxContainer;

IGuiContainer::IGuiContainer() throw() :
    m_container( 0 ),
    m_isContainerLocallyCreated( false )
{
}

//-----------------------------------------------------------------------------

IGuiContainer::~IGuiContainer() throw()
{
}

//-----------------------------------------------------------------------------

wxWindow * IGuiContainer::getWxContainer()
{
    SLM_FATAL_IF("Sorry, parent wxContainer not yet initialized, please initGuiParentContainer before", !m_container ) ;
    return m_container ;
}

//-----------------------------------------------------------------------------

void IGuiContainer::initGuiParentContainer()
{
    if(m_globalUIDToWxContainer.find(this->getUUID()) != m_globalUIDToWxContainer.end())
    {
        OSLM_TRACE( "GuiContainer fund for this Service " <<  this->getUUID() );
        m_container = m_globalUIDToWxContainer[this->getUUID()];
        SLM_ASSERT("Sorry, wxContainer is not correctly initialized", m_container);
    }
    else
    {
        OSLM_TRACE( "GuiContainer not fund for this Service " <<  this->getUUID() );
        m_container = new wxFrame(wxTheApp->GetTopWindow(), wxNewId(), ::fwWX::std2wx( "IGuiContainer" ));
        m_container->Show();
        m_container->Refresh();
        m_isContainerLocallyCreated = true ;
    }
}

//-----------------------------------------------------------------------------

void IGuiContainer::resetGuiParentContainer()
{
    if(m_container != wxTheApp->GetTopWindow())
    {
        m_container->SetSizer(NULL);
        m_container->DestroyChildren();
    }
    if( m_isContainerLocallyCreated && m_container != 0 )
    {
        SLM_DEBUG("Destroying container") ;
        m_container->Destroy();
        m_container = 0 ;
    }
}

//-----------------------------------------------------------------------------

void IGuiContainer::registerWxContainer(std::string uid , wxWindow * container)
{
    OSLM_ASSERT("Sorry, wxContainer for "<<uid<<" already exists in SubUIDToWxContainer map.",
            m_subUIDToWxContainer.find(uid) == m_subUIDToWxContainer.end());
    m_subUIDToWxContainer[uid] = container;

    ::fwWX::IGuiContainer::registerGlobalWxContainer(uid, container);
}

//-----------------------------------------------------------------------------

void IGuiContainer::unregisterWxContainer(std::string uid)
{
    bool service_exists = ::fwTools::UUID::exist(uid, ::fwTools::UUID::SIMPLE );
    OSLM_INFO_IF("Service "<<uid <<" not exists.",!service_exists );
    if(service_exists)
    {
        ::fwServices::IService::sptr service = ::fwServices::get( uid ) ;
        service->stop();
    }

    OSLM_ASSERT("Sorry, wxContainer for "<<uid<<" not exists in SubUIDToWxContainer map.",
            m_subUIDToWxContainer.find(uid) != m_subUIDToWxContainer.end());

    wxWindow* container = m_subUIDToWxContainer[uid];
    SLM_ASSERT("Sorry, wxContainer is not correctly initialized", container);
    // Destroys the window safely
    container->Destroy();
    container = 0;
    // Removes container in internal map
    m_subUIDToWxContainer.erase(uid);

    ::fwWX::IGuiContainer::unregisterGlobalWxContainer(uid);
}

//-----------------------------------------------------------------------------

void IGuiContainer::unregisterAllWxContainer()
{
    ContainerMapType subUIDToWxContainer = m_subUIDToWxContainer;
    BOOST_FOREACH( ContainerMapType::value_type uidContainerElmt, subUIDToWxContainer)
    {
        this->unregisterWxContainer(uidContainerElmt.first);
    }
    m_subUIDToWxContainer.clear();
}

//-----------------------------------------------------------------------------

void IGuiContainer::registerGlobalWxContainer(std::string uid, wxWindow * container)
{
    OSLM_ASSERT("Sorry, wxContainer for "<<uid<<" already exists in GlobalUIDToWxContainer map.",
            m_globalUIDToWxContainer.find(uid) == m_globalUIDToWxContainer.end());
    m_globalUIDToWxContainer[uid] = container;
}

//-----------------------------------------------------------------------------

void IGuiContainer::unregisterGlobalWxContainer(std::string uid)
{
    OSLM_ASSERT("Sorry, wxContainer for "<<uid<<" not exists in GlobalUIDToWxContainer map.",
            m_globalUIDToWxContainer.find(uid) != m_globalUIDToWxContainer.end());
    // Removes container in global map
    m_globalUIDToWxContainer.erase(uid);
}

//-----------------------------------------------------------------------------

wxWindow* IGuiContainer::getWxContainer(std::string uid)
{
    OSLM_ASSERT("Sorry, wxContainer for "<<uid<<" not exists in GlobalUIDToWxContainer map.",
            m_globalUIDToWxContainer.find(uid) != m_globalUIDToWxContainer.end());
    // returns container in global map
    return  m_globalUIDToWxContainer[uid];
}

}
