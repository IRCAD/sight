/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/sizer.h>
#include <wx/panel.h>

#include <boost/foreach.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiWx/container/WxContainer.hpp"
#include "fwGuiWx/builder/ContainerBuilder.hpp"



REGISTER_BINDING( ::fwGui::builder::IContainerBuilder,
        ::fwGui::builder::ContainerBuilder,
        ::fwGui::builder::IContainerBuilder::RegistryKeyType,
        ::fwGui::builder::IContainerBuilder::REGISTRY_KEY );


namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

ContainerBuilder::ContainerBuilder()
{}

//-----------------------------------------------------------------------------

ContainerBuilder::~ContainerBuilder()
{}

//-----------------------------------------------------------------------------

void ContainerBuilder::createContainer( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiWx::container::WxContainer::dynamicCast(parent);
    SLM_ASSERT("Sorry, the parent container is not a WxContainer", m_parent);

    ::fwGuiWx::container::WxContainer::NewSptr wxContainer;
    wxWindow *wxParent = m_parent->getWxContainer();
    wxPanel *panel = new wxPanel(wxParent, wxNewId());
    wxContainer->setWxContainer(panel);

    wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );

    SLM_ASSERT("Sorry, the parent container has already a layout", !wxParent->GetSizer());
    wxParent->SetSizer(boxSizer);
    boxSizer->Add(panel, 1, wxALL|wxEXPAND);
    wxParent->Layout();

    m_container = wxContainer;
}

//-----------------------------------------------------------------------------

void ContainerBuilder::destroyContainer()
{
    SLM_ASSERT("Sorry, Container not initialized", m_container);
    SLM_ASSERT("Sorry, the parent container is not a WxContainer", m_parent);

    m_container->destroyContainer();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void ContainerBuilder::setParent(::fwGui::container::fwContainer::sptr parent)
{
    SLM_ASSERT("Sorry, WxContainer not yet initialized, cleaning impossible", m_container);
    ::fwGuiWx::container::WxContainer::sptr parentContainer = ::fwGuiWx::container::WxContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to WxContainer failed", parentContainer);
    ::fwGuiWx::container::WxContainer::sptr container = ::fwGuiWx::container::WxContainer::dynamicCast(m_container);
    SLM_ASSERT("dynamicCast fwContainer to WxContainer failed", container);

    wxWindow *wxContainer = container->getWxContainer();
    SLM_ASSERT("Sorry, WxContainer not yet initialized", wxContainer);
    wxWindow *wxParent = parentContainer->getWxContainer();
    SLM_ASSERT("Sorry, parent WxContainer not yet initialized", wxParent);

    if(wxParent != m_parent->getWxContainer() )
    {
        if(!wxParent->GetSizer())
        {
            SLM_TRACE("New parent container hasn't layout");

            wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
            wxParent->SetSizer(boxSizer);
        }
        wxContainer->Reparent(wxParent);
        wxWindow *oldWxParent = m_parent->getWxContainer() ;

        oldWxParent->GetSizer()->Detach(wxContainer);
        wxParent->GetSizer()->Add(wxContainer, 1, wxALL|wxEXPAND);

        wxParent->Layout();
        wxContainer->Layout();

        //oldWxParent->SetSizer(NULL);

        m_parent = parentContainer;
    }
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui



