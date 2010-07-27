/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiWx/container/WxMenuItemContainer.hpp"

namespace fwGuiWx
{
namespace container
{

//-----------------------------------------------------------------------------

WxMenuItemContainer::WxMenuItemContainer() throw() : m_menuItem(0)
{}

//-----------------------------------------------------------------------------

WxMenuItemContainer::~WxMenuItemContainer() throw()
{}

//-----------------------------------------------------------------------------

void WxMenuItemContainer::clean()
{
    SLM_ASSERT("Sorry, wxMenuItem not yet initialized, cleaning impossible", m_menuItem);
}

//-----------------------------------------------------------------------------

void WxMenuItemContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, wxMenuItem not yet initialized, cleaning impossible", m_menuItem);
}

//-----------------------------------------------------------------------------

void WxMenuItemContainer::setWxMenuItem(wxMenuItem* menuItem)
{
    this->m_menuItem = menuItem;
}

//-----------------------------------------------------------------------------

wxMenuItem* WxMenuItemContainer::getWxMenuItem()
{
    return this->m_menuItem;
}

//-----------------------------------------------------------------------------

void WxMenuItemContainer::setWxToolItem(wxToolBarToolBase* menuItem)
{
    this->m_toolBarToolBase = menuItem;
}

//-----------------------------------------------------------------------------

wxToolBarToolBase* WxMenuItemContainer::getWxToolItem()
{
    return this->m_toolBarToolBase;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiWx
