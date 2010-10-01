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
          ::fwGui::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY );

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

void MenuBarLayoutManager::createLayout( ::fwGui::container::fwMenuBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiWx::container::WxMenuBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenuBar to WxMenuBarContainer failed", m_parent);

    wxMenuBar* menuBar = m_parent->getWxMenuBar();

    BOOST_FOREACH ( std::string name, m_menuNames)
    {
        ::fwGuiWx::container::WxMenuContainer::NewSptr menu;
        wxMenu *menuWx = new wxMenu();
        menu->setWxMenu(menuWx);
        menuBar->Append( menuWx , ::fwWX::std2wx( name ));
        m_menus.push_back(menu);
    }
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::destroyLayout()
{
    wxMenuBar* menuBar = m_parent->getWxMenuBar();
    BOOST_FOREACH(::fwGui::container::fwMenu::sptr menu, m_menus)
    {
        std::string name = m_menuNames[this->getMenuPosition(menu)];
        int index = menuBar->FindMenu(::fwWX::std2wx(name));
        SLM_ASSERT("Menu " << name << " not found", index != wxNOT_FOUND);
        menuBar->Remove(index);
    }
    m_menus.clear();
}

//-----------------------------------------------------------------------------


void MenuBarLayoutManager::menuIsVisible(::fwGui::container::fwMenu::sptr fwMenu, bool isVisible)
{
    SLM_FATAL("TODO : MenuBarLayoutManager::menuIsVisible not yet implemented.")
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsEnabled(::fwGui::container::fwMenu::sptr fwMenu, bool isEnabled)
{
    wxMenuBar* menuBar = m_parent->getWxMenuBar();
    std::string name = m_menuNames[this->getMenuPosition(fwMenu)];
    int index = menuBar->FindMenu(::fwWX::std2wx(name));
    SLM_ASSERT("Menu " << name << " not found", index != wxNOT_FOUND);
    menuBar->EnableTop(index, isEnabled);
}

//-----------------------------------------------------------------------------

int MenuBarLayoutManager::getMenuPosition(::fwGui::container::fwMenu::sptr fwMenu)
{
    int index = 0;
    BOOST_FOREACH(::fwGui::container::fwMenu::sptr menu, m_menus)
    {
        if (menu == fwMenu)
        {
            break;
        }
        index++;
    }
    return index;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



