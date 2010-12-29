/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiWx/container/WxMenuContainer.hpp"

namespace fwGuiWx
{
namespace container
{

//-----------------------------------------------------------------------------

WxMenuContainer::WxMenuContainer() throw() : m_menu(0)
{}

//-----------------------------------------------------------------------------

WxMenuContainer::~WxMenuContainer() throw()
{}

//-----------------------------------------------------------------------------

void WxMenuContainer::clean()
{
    SLM_ASSERT("Sorry, wxMenu not yet initialized, cleaning impossible", m_menu);
    while( m_menu->GetMenuItemCount() )
    {
        m_menu->Remove(0);
    }
}

//-----------------------------------------------------------------------------

void WxMenuContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, wxMenu not yet initialized, cleaning impossible", m_menu);
}

//-----------------------------------------------------------------------------

void WxMenuContainer::setWxMenu(wxMenu* menu)
{
    this->m_menu = menu;
}

//-----------------------------------------------------------------------------

wxMenu* WxMenuContainer::getWxMenu()
{
    return this->m_menu;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiWx
