/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <wx/sizer.h>
#include <wx/aui/aui.h>

#include "fwGuiWx/container/WxContainer.hpp"

namespace fwGuiWx
{
namespace container
{

//-----------------------------------------------------------------------------

WxContainer::WxContainer() throw() : m_container(0)
{}

//-----------------------------------------------------------------------------

WxContainer::~WxContainer() throw()
{
    SLM_ASSERT( "Error during destruction : The wx container included in this class is still allocated, please call destroyContainer() before.", m_container == 0 );
}

//-----------------------------------------------------------------------------

void WxContainer::clean()
{
    SLM_ASSERT("Sorry, wxWindows not yet initialized, cleaning impossible", m_container);

    m_container->SetSizer(NULL);
    m_container->DestroyChildren();
}

//-----------------------------------------------------------------------------

void WxContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, wxWindows not yet initialized", m_container);
    OSLM_ASSERT("Container must be empty ( " << m_container->GetChildren().GetCount() << " children).", m_container->GetChildren().IsEmpty());
    m_container->Destroy();
    m_container = 0;
}

//-----------------------------------------------------------------------------

void WxContainer::setWxContainer(wxWindow* container)
{
    this->m_container = container;
}

//-----------------------------------------------------------------------------

wxWindow* WxContainer::getWxContainer()
{
    return this->m_container;
}

//-----------------------------------------------------------------------------

bool WxContainer::isShownOnScreen()
{
    SLM_ASSERT("Sorry, wxWindow not yet initialized, cleaning impossible", m_container);
    return m_container->IsShownOnScreen();
}

//-----------------------------------------------------------------------------

void WxContainer::setVisible(bool isVisible)
{
    SLM_ASSERT("Sorry, WxContainer not yet initialized, cleaning impossible", m_container);
    wxWindow* parent   = m_container->GetParent();

    if (parent && parent->GetSizer() )
    {
        wxAuiManager* aui = wxAuiManager::GetManager(parent);
        if(aui && aui->GetManagedWindow() == parent )
        {
            wxAuiPaneInfo& pane = aui->GetPane(m_container);
            SLM_ASSERT("Sorry, wxAuiPaneInfo not found", pane.IsOk());
            pane.Show(isVisible);
            aui->Update();
        }
        else
        {
            wxSizer* sizer1 = parent->GetSizer();
            bool isFound = sizer1->Show(m_container, isVisible, true);
            if(m_container->GetContainingSizer() && m_container->GetContainingSizer() != sizer1)
            {
                wxSizer* sizer2 = m_container->GetContainingSizer();
                sizer1->Show(sizer2, isVisible, true);
            }
        }
        parent->Layout();
    }

    m_container->Show(isVisible);
    m_container->Layout();
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiWx
