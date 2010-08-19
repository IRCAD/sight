/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/menu.h>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwWX/convert.hpp>

#include "fwGuiWx/ActionCallback.hpp"
#include "fwGuiWx/container/WxToolBarContainer.hpp"
#include "fwGuiWx/container/WxMenuItemContainer.hpp"
#include "fwGuiWx/layoutManager/ToolBarLayoutManager.hpp"
#include "fwGuiWx/Shortcut.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IToolBarLayoutManager,
        ::fwGui::layoutManager::ToolBarLayoutManager,
         ::fwGui::layoutManager::IToolBarLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

ToolBarLayoutManager::ToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

ToolBarLayoutManager::~ToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::createLayout( ::fwGui::fwToolBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiWx::container::WxToolBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwToolBar to WxToolBarContainer failed", m_parent);

    wxToolBar* toolBar = m_parent->getWxToolBar();
    unsigned int menuItemIndex = 0;
    BOOST_FOREACH ( ::fwGui::layoutManager::IToolBarLayoutManager::ActionInfo actionInfo, m_actionInfo)
    {
        ::fwGuiWx::container::WxMenuItemContainer::NewSptr menuItem;

        int actionIdInMenu = wxNewId();
        wxItemKind kind = wxITEM_NORMAL;
        if(actionInfo.m_isCheckable || actionInfo.m_isRadio)
        {
            kind = actionInfo.m_isRadio ? wxITEM_RADIO : wxITEM_CHECK;
        }

        wxImage image = wxNullImage ;
        if(!actionInfo.m_icon.empty())
        {
            if ( !image.LoadFile(::fwWX::std2wx( actionInfo.m_icon )))
            {
                wxLogError(_("Couldn't load image from '%s'."), ::fwWX::std2wx( actionInfo.m_icon ));
            }
        }

        if(!actionInfo.m_isSeparator)
        {
            wxToolBarToolBase * toolBarToolBase = toolBar->AddTool(actionIdInMenu, ::fwWX::std2wx(actionInfo.m_name), wxBitmap(image),  wxBitmap(image.ConvertToGreyscale()), kind, ::fwWX::std2wx( actionInfo.m_name));
            if((actionInfo.m_isCheckable || actionInfo.m_isRadio) && actionInfo.m_isChecked)
            {
                toolBar->ToggleTool(actionIdInMenu, true);
            }

            toolBar->EnableTool(actionIdInMenu, actionInfo.m_isEnabled);

            menuItem->setWxToolItem(toolBarToolBase);
            m_menuItems.push_back(menuItem);
            OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ::fwGuiWx::ActionCallback::sptr wxCallback = ::fwGuiWx::ActionCallback::dynamicCast(callback);
            SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", wxCallback);

            // get MainFrame for binding
            wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
            SLM_ASSERT( "No wxFrame", frame ) ;

            typedef ::boost::function1< void, wxCommandEvent& > MenuItemCallback;
            MenuItemCallback call = ::boost::bind( &::fwGuiWx::ActionCallback::executeWx, wxCallback, _1 );
            frame->Bind( wxEVT_COMMAND_TOOL_CLICKED, call, actionIdInMenu);

            menuItemIndex++;
        }
        else
        {
            toolBar->AddSeparator();
        }
    }
    toolBar->Realize();
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::destroyLayout()
{
    wxToolBar* toolBar = m_parent->getWxToolBar();
    toolBar->ClearTools();
    m_menuItems.clear();
}

//-----------------------------------------------------------------------------


void ToolBarLayoutManager::menuItemSetVisible(::fwGui::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    SLM_FATAL("TODO : ToolBarLayoutManager::actionIsVisible not yet implemented.")
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetEnabled(::fwGui::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    wxToolBar* toolBar = m_parent->getWxToolBar();
    ::fwGuiWx::container::WxMenuItemContainer::sptr menuItemContainer = ::fwGuiWx::container::WxMenuItemContainer::dynamicCast(fwMenuItem);
    wxToolBarToolBase *menuItem = menuItemContainer->getWxToolItem();
    toolBar->EnableTool(menuItem->GetId(), isEnabled);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetChecked(::fwGui::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    wxToolBar* toolBar = m_parent->getWxToolBar();
    ::fwGuiWx::container::WxMenuItemContainer::sptr menuItemContainer = ::fwGuiWx::container::WxMenuItemContainer::dynamicCast(fwMenuItem);
    wxToolBarToolBase *menuItem = menuItemContainer->getWxToolItem();
    if (menuItem->GetKind() == wxITEM_RADIO || menuItem->GetKind() == wxITEM_CHECK)
    {
        toolBar->ToggleTool(menuItem->GetId(), isChecked);
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



