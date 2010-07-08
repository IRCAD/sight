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

    m_menuBar->DestroyChildren();
}

//-----------------------------------------------------------------------------

void WxMenuBarContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, wxMenuBar not yet initialized", m_menuBar);

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
