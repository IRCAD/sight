/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/panel.h>
#include <wx/sizer.h>

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwWX/convert.hpp>

#include "fwGuiWx/layoutManager/TabLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IViewLayoutManager,
        ::fwGui::TabLayoutManager,
         ::fwGui::layoutManager::TabLayoutManagerBase::RegistryKeyType,
          ::fwGui::layoutManager::TabLayoutManagerBase::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

TabLayoutManager::TabLayoutManager()
{}

//-----------------------------------------------------------------------------

TabLayoutManager::~TabLayoutManager()
{}

//-----------------------------------------------------------------------------

void TabLayoutManager::createLayout( ::fwGui::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiWx::container::WxContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to WxContainer failed", m_parentContainer);

    wxWindow* wxContainer = m_parentContainer->getWxContainer();
    wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
    wxContainer->SetSizer( boxSizer );
    m_notebook = new wxNotebook( wxContainer, wxNewId() );
    boxSizer->Add( m_notebook, 1, wxALL|wxEXPAND, 0);

    std::list< ViewInfo> views = this->getViewsInfo();

    BOOST_FOREACH ( ViewInfo viewInfo, views)
    {
        wxPanel * viewPanel = new wxPanel(  m_notebook, wxNewId());
        ::fwGuiWx::container::WxContainer::NewSptr subContainer;
        subContainer->setWxContainer(viewPanel);
        m_subViews.push_back(subContainer);

        m_notebook->AddPage( viewPanel, ::fwWX::std2wx(viewInfo.m_caption), viewInfo.m_isSelect );
    }
    wxContainer->Layout();
}

//-----------------------------------------------------------------------------

void TabLayoutManager::destroyLayout()
{
    // Remove pages without removing wxPanel created for this pages
    // BE CARREFULL => not use m_notebook->DeleteAllPages() because
    // this method remove wxPanel insides pages but unregisterAllWxContainer
    // remove this panel also.
    for ( int pageNumber = m_notebook->GetPageCount()-1;
            pageNumber >= 0;
            pageNumber-- )
    {
        m_notebook->RemovePage( pageNumber );
    }
    this->destroySubViews();
}

//-----------------------------------------------------------------------------

} // namespace fwGui



