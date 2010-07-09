/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/menu.h>

#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwWX/convert.hpp>

#include "fwGuiWx/container/WxMenuContainer.hpp"
#include "fwGuiWx/container/WxMenuItemContainer.hpp"
#include "fwGuiWx/layoutManager/MenuLayoutManager.hpp"
#include "fwGuiWx/Shortcut.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IMenuLayoutManager,
        ::fwGui::layoutManager::MenuLayoutManager,
         ::fwGui::layoutManager::IMenuLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IMenuLayoutManager::REGISTRAR_KEY );

namespace fwGui
{
namespace layoutManager
{

const std::map< ::fwGui::layoutManager::IMenuLayoutManager::ActionType, int> MenuLayoutManager::SPECIAL_ACTION_TO_WXID =
                                                ::boost::assign::map_list_of(::fwGui::layoutManager::IMenuLayoutManager::QUIT,wxID_EXIT)
                                                                            (::fwGui::layoutManager::IMenuLayoutManager::ABOUT,wxID_ABOUT)
                                                                            (::fwGui::layoutManager::IMenuLayoutManager::HELP,wxID_HELP)
                                                                            (::fwGui::layoutManager::IMenuLayoutManager::NEW,wxID_NEW)
                                                                            (::fwGui::layoutManager::IMenuLayoutManager::SEPARATOR,wxID_SEPARATOR);

//-----------------------------------------------------------------------------

MenuLayoutManager::MenuLayoutManager()
{}

//-----------------------------------------------------------------------------

MenuLayoutManager::~MenuLayoutManager()
{}

//-----------------------------------------------------------------------------

void MenuLayoutManager::createLayout( ::fwGui::fwMenu::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiWx::container::WxMenuContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenu to WxMenuContainer failed", m_parent);

    wxMenu* menu = m_parent->getWxMenu();

    BOOST_FOREACH ( ::fwGui::layoutManager::IMenuLayoutManager::ActionInfo actionInfo, m_actionInfo)
    {
        ::fwGuiWx::container::WxMenuItemContainer::NewSptr menuItem;

        int actionIdInMenu;
        if (actionInfo.m_type == ::fwGui::layoutManager::IMenuLayoutManager::DEFAULT)
        {
            actionIdInMenu = wxNewId();
        }
        else
        {
            OSLM_ASSERT("Action type "<<actionInfo.m_type<< " is unknown.",
                           SPECIAL_ACTION_TO_WXID.find(actionInfo.m_type) != SPECIAL_ACTION_TO_WXID.end());
            actionIdInMenu = SPECIAL_ACTION_TO_WXID.find(actionInfo.m_type)->second;
        }

        wxMenuItem *menuItemWx;
        wxItemKind kind = wxITEM_NORMAL;
        if(actionInfo.m_isCheckable || actionInfo.m_isRadio)
        {
            kind = actionInfo.m_isRadio ? wxITEM_RADIO : wxITEM_CHECK;

        }

        std::string actionNameInMenu = actionInfo.m_name;
        // create shortcut
        if( !actionInfo.m_shortcut.empty() )
        {
            ::fwGuiWx::Shortcut::sptr shortcut =  ::fwGuiWx::Shortcut::New( actionInfo.m_shortcut );
            actionNameInMenu += "\t" + shortcut->toString();
        }
        menuItemWx = new wxMenuItem(menu, actionIdInMenu , ::fwWX::std2wx( actionNameInMenu ),_(""), kind ) ;
        menu->Append( menuItemWx );
        menuItem->setWxMenuItem(menuItemWx);

        if(!actionInfo.m_isSeparator)
        {
            m_menuItems.push_back(menuItem);
        }
        else
        {
            m_separators.push_back(menuItem);
        }
    }
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::destroyLayout()
{
    wxMenu* menu = m_parent->getWxMenu();

    BOOST_FOREACH(::fwGui::fwMenuItem::sptr menuItem, m_menuItems)
    {
        ::fwGuiWx::container::WxMenuItemContainer::sptr menuItemContainer = ::fwGuiWx::container::WxMenuItemContainer::dynamicCast(menuItem);
        wxMenuItem *item = menuItemContainer->getWxMenuItem();
        menu->Destroy(item);
    }

    BOOST_FOREACH(::fwGui::fwMenuItem::sptr menuItem, m_separators)
    {
        ::fwGuiWx::container::WxMenuItemContainer::sptr menuItemContainer = ::fwGuiWx::container::WxMenuItemContainer::dynamicCast(menuItem);
        wxMenuItem *item = menuItemContainer->getWxMenuItem();
        menu->Destroy(item);
    }
    m_menuItems.clear();
    m_separators.clear();
}

//-----------------------------------------------------------------------------


void MenuLayoutManager::actionIsVisible(::fwGui::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    SLM_FATAL("TODO : MenuLayoutManager::actionIsVisible not yet implemented.")
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::actionIsEnabled(::fwGui::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ::fwGuiWx::container::WxMenuItemContainer::sptr menuItemContainer = ::fwGuiWx::container::WxMenuItemContainer::dynamicCast(fwMenuItem);
    wxMenuItem *menuItem = menuItemContainer->getWxMenuItem();
    menuItem->Enable(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



