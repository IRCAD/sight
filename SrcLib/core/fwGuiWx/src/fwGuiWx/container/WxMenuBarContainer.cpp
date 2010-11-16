/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiWx/container/WxMenuBarContainer.hpp"

namespace fwGuiWx
{
namespace container
{

//-----------------------------------------------------------------------------

WxMenuBarContainer::WxMenuBarContainer() throw() : m_menuBar(0)
{}

//-----------------------------------------------------------------------------

WxMenuBarContainer::~WxMenuBarContainer() throw()
{}

//-----------------------------------------------------------------------------

void WxMenuBarContainer::clean()
{
    SLM_ASSERT("Sorry, wxMenuBar not yet initialized, cleaning impossible", m_menuBar);
    while( m_menuBar->GetMenuCount() )
    {
        wxMenu* menu = m_menuBar->Remove(0);
        SLM_ASSERT("Menu must be empty", menu->GetMenuItemCount() == 0);
        delete menu;
    }
}

//-----------------------------------------------------------------------------

void WxMenuBarContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, wxMenuBar not yet initialized", m_menuBar);
    OSLM_ASSERT("MenuBar container must be empty ( " << m_menuBar->GetMenuCount() << " menu(s)).", m_menuBar->GetMenuCount() == 0);
    m_menuBar->Destroy();
    m_menuBar = 0;
}

//-----------------------------------------------------------------------------

void WxMenuBarContainer::setWxMenuBar(wxMenuBar* menuBar)
{
    this->m_menuBar = menuBar;
}

//-----------------------------------------------------------------------------

wxMenuBar* WxMenuBarContainer::getWxMenuBar()
{
    return this->m_menuBar;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiWx
