/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/frame.h>
#include <wx/menu.h>

#include <boost/foreach.hpp>

#include "fwGuiWx/container/WxContainer.hpp"
#include "fwGuiWx/container/WxMenuBarContainer.hpp"
#include "fwGuiWx/builder/MenuBarBuilder.hpp"

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

void MenuBarBuilder::createMenuBar( ::fwGui::fwContainer::sptr parent )
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
    this->m_menuBar->clean();
    SLM_ASSERT("Sorry, the parent container is not a WxContainer", m_parent);
    wxFrame *frame = wxDynamicCast( m_parent->getWxContainer() , wxFrame ) ;
    SLM_ASSERT("Sorry, the parent container must be a wxFrame", frame ) ;
    if (frame)
    {
        frame->SetMenuBar( NULL );
    }
}

//-----------------------------------------------------------------------------


} // namespace builder
} // namespace fwGui



