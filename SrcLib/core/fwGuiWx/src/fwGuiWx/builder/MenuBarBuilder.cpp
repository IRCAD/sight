/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/frame.h>
#include <wx/menu.h>

#include <boost/foreach.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiWx/container/WxContainer.hpp"
#include "fwGuiWx/container/WxMenuBarContainer.hpp"
#include "fwGuiWx/builder/MenuBarBuilder.hpp"

REGISTER_BINDING( ::fwGui::builder::IMenuBarBuilder,
        ::fwGui::builder::MenuBarBuilder,
         ::fwGui::builder::IMenuBarBuilder::RegistryKeyType,
          ::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY );

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

MenuBarBuilder::MenuBarBuilder()
{}

//-----------------------------------------------------------------------------

MenuBarBuilder::~MenuBarBuilder()
{}

//-----------------------------------------------------------------------------

void MenuBarBuilder::createMenuBar( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiWx::container::WxContainer::dynamicCast(parent);
    SLM_ASSERT("Sorry, the parent container is not a WxContainer", m_parent);
    wxFrame *frame = wxDynamicCast( m_parent->getWxContainer() , wxFrame ) ;
    SLM_ASSERT("Sorry, the parent container must be a wxFrame", frame ) ;
    if (frame)
    {
        ::fwGuiWx::container::WxMenuBarContainer::sptr menuBarContainer = ::fwGuiWx::container::WxMenuBarContainer::New();
        wxMenuBar *menuBar = new wxMenuBar();
        menuBarContainer->setWxMenuBar(menuBar);
        frame->SetMenuBar( menuBar );
        this->m_menuBar = menuBarContainer;
    }
}

//-----------------------------------------------------------------------------

void MenuBarBuilder::destroyMenuBar()
{
    SLM_ASSERT("Sorry, the parent container is not a WxContainer", m_parent);
    wxFrame *frame = wxDynamicCast( m_parent->getWxContainer() , wxFrame ) ;
    SLM_ASSERT("Sorry, the parent container must be a wxFrame", frame ) ;
    if (frame)
    {
        OSLM_ASSERT("ToolBar container must be empty ( " << frame->GetMenuBar()->GetMenuCount() << " children).", frame->GetMenuBar()->GetMenuCount() == 0);
        frame->SetMenuBar( NULL );
    }
}

//-----------------------------------------------------------------------------


} // namespace builder
} // namespace fwGui



