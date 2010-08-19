/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

} // namespace container
} // namespace fwGuiWx
