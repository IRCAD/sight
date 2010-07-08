/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwWX/convert.hpp>

#include "fwGuiWx/container/WxMenuContainer.hpp"
#include "fwGuiWx/layoutManager/MenuBarLayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuBarLayoutManager::MenuBarLayoutManager()
{}

//-----------------------------------------------------------------------------

MenuBarLayoutManager::~MenuBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::createLayout( ::fwGui::fwMenuBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiWx::container::WxMenuBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenuBar to WxMenuBarContainer failed", m_parent);

    wxMenuBar* menuBar = m_parent->getWxMenuBar();

    BOOST_FOREACH ( std::string name, m_menuNames)
    {
        ::fwGuiWx::container::WxMenuContainer::NewSptr menu;
        wxMenu *menuWx = new wxMenu(::fwWX::std2wx( name ));
        menu->setWxMenu(menuWx);
        menuBar->Append( menuWx , ::fwWX::std2wx( name ));
        m_menus.push_back(menu);
    }
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::destroyLayout()
{
    wxMenuBar* menuBar = m_parent->getWxMenuBar();

    ///@todo Menu not empty on MAC with specials Actions like help, Quit...
#ifndef __MACOSX__
    while (menuBar->GetMenuCount() > 0)
    {
        SLM_ASSERT( "Menu must be empty before his destruction.", menuBar->GetMenu(0)->GetMenuItems().size() == 0 );
        menuBar->Remove(0);
    }
#endif
}

//-----------------------------------------------------------------------------


void MenuBarLayoutManager::menuIsVisible(::fwGui::fwMenu::sptr fwMenu, bool isVisible)
{
    SLM_FATAL("TODO : MenuBarLayoutManager::menuIsVisible not yet implemented.")
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsEnabled(::fwGui::fwMenu::sptr fwMenu, bool isEnabled)
{
    ::fwGuiWx::container::WxMenuContainer::sptr menuContainer = ::fwGuiWx::container::WxMenuContainer::dynamicCast(fwMenu);
    wxMenu *menu = menuContainer->getWxMenu();
    wxString menuName = menu->GetTitle();
    wxMenuBar *menuBar = m_parent->getWxMenuBar();
    int menuId = menuBar->FindMenu(menuName);
    menuBar->EnableTop(menuId, isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



