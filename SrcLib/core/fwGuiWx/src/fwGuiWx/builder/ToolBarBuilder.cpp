/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/frame.h>
#include <wx/toolbar.h>

#include <boost/foreach.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiWx/container/WxContainer.hpp"
#include "fwGuiWx/container/WxToolBarContainer.hpp"
#include "fwGuiWx/builder/ToolBarBuilder.hpp"

REGISTER_BINDING( ::fwGui::builder::IToolBarBuilder,
        ::fwGui::builder::ToolBarBuilder,
         ::fwGui::builder::IToolBarBuilder::RegistryKeyType,
          ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY );

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

ToolBarBuilder::ToolBarBuilder()
{}

//-----------------------------------------------------------------------------

ToolBarBuilder::~ToolBarBuilder()
{}

//-----------------------------------------------------------------------------

void ToolBarBuilder::createToolBar( ::fwGui::fwContainer::sptr parent )
{
    m_parent = ::fwGuiWx::container::WxContainer::dynamicCast(parent);
    SLM_ASSERT("Sorry, the parent container is not a WxContainer", m_parent);
    wxFrame *frame = wxDynamicCast( m_parent->getWxContainer() , wxFrame ) ;
    SLM_ASSERT("Sorry, the parent container must be a wxFrame", frame ) ;
    if (frame)
    {
        ::fwGuiWx::container::WxToolBarContainer::sptr toolBarContainer = ::fwGuiWx::container::WxToolBarContainer::New();
        frame->CreateToolBar(wxNO_BORDER|wxHORIZONTAL|wxTB_FLAT, -1);
        frame->GetToolBar()->SetMargins( 2, 2 );
        frame->GetToolBar()->SetToolBitmapSize( wxSize(m_toolBitmapSize.first, m_toolBitmapSize.second) );
        toolBarContainer->setWxToolBar(frame->GetToolBar());
        this->m_toolBar = toolBarContainer;
    }
}

//-----------------------------------------------------------------------------

void ToolBarBuilder::destroyToolBar()
{
    this->m_toolBar->clean();
    this->m_toolBar->destroyContainer();
    SLM_ASSERT("Sorry, the parent container is not a WxContainer", m_parent);
    wxFrame *frame = wxDynamicCast( m_parent->getWxContainer() , wxFrame ) ;
    SLM_ASSERT("Sorry, the parent container must be a wxFrame", frame ) ;
    if (frame)
    {
        frame->SetToolBar( NULL );
    }
}

//-----------------------------------------------------------------------------


} // namespace builder
} // namespace fwGui



