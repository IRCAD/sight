/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/menu.h>
#include <boost/foreach.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwWX/convert.hpp>

#include "fwGuiWx/container/WxMenuContainer.hpp"
#include "fwGuiWx/layoutManager/MenuBarLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IMenuBarLayoutManager,
        ::fwGui::layoutManager::MenuBarLayoutManager,
         ::fwGui::layoutManager::IMenuBarLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IMenuBarLayoutManager::REGISTRAR_KEY );

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

    BOOST_FOREACH(::fwGui::fwMenu::sptr menu, m_menus)
    {
        ::fwGuiWx::container::WxMenuContainer::sptr menuContainer = ::fwGuiWx::container::WxMenuContainer::dynamicCast(menu);
        wxMenu *menuWx = menuContainer->getWxMenu();
        int menuIndex = menuBar->FindMenu(menuWx->GetTitle());
        menuBar->Remove(menuIndex);
    }
    m_menus.clear();
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



