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

#include "fwGuiWx/layoutManager/CardinalLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IViewLayoutManager,
        ::fwGui::CardinalLayoutManager,
         ::fwGui::layoutManager::CardinalLayoutManagerBase::RegistryKeyType,
          ::fwGui::layoutManager::CardinalLayoutManagerBase::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

CardinalLayoutManager::CardinalLayoutManager()
{}

//-----------------------------------------------------------------------------

CardinalLayoutManager::~CardinalLayoutManager()
{}

//-----------------------------------------------------------------------------

void CardinalLayoutManager::createLayout( ::fwGui::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiWx::container::WxContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to WxContainer failed", m_parentContainer);

    wxWindow* wxContainer = m_parentContainer->getWxContainer();

    // Set no flags on wxAuiManager constructor to disable m_hint_wind internal frame construction
    // this frame is not correctly destroyed
    m_manager = new wxAuiManager(  wxContainer, 0 /* no flag */);
    std::list< ViewInfo> views = this->getViewsInfo();

    BOOST_FOREACH ( ViewInfo viewInfo, views)
    {
        wxPanel * viewPanel = new wxPanel(  wxContainer, wxNewId() , wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL );
        ::fwGuiWx::container::WxContainer::NewSptr subContainer;
        subContainer->setWxContainer(viewPanel);
        m_subViews.push_back(subContainer);

        // Pane info configuration
        ::wxAuiPaneInfo paneInfo;
        if(viewInfo.m_align==CENTER)        { paneInfo.Center(); }
        else if(viewInfo.m_align==RIGHT)    { paneInfo.Right(); }
        else if(viewInfo.m_align==LEFT)     { paneInfo.Left(); }
        else if(viewInfo.m_align==BOTTOM)   { paneInfo.Bottom();}
        else if(viewInfo.m_align==TOP)      { paneInfo.Top(); }

        if(!viewInfo.m_isResizable)
        {
            paneInfo.Fixed();
        }
        paneInfo.CloseButton( false );
        paneInfo.Floatable( false );
        paneInfo.MaximizeButton( true );
        if(viewInfo.m_caption.first)
        {
            paneInfo.CaptionVisible( true );
            paneInfo.Caption( ::fwWX::std2wx(viewInfo.m_caption.second) );
        }
        else
        {
            paneInfo.CaptionVisible( false );
        }
        paneInfo.PaneBorder( false );
        paneInfo.MinSize( wxSize( viewInfo.m_minSize.first, viewInfo.m_minSize.second ) );
        paneInfo.Position(viewInfo.m_position);
        paneInfo.Layer(viewInfo.m_layer);
        paneInfo.Row(viewInfo.m_row);
        paneInfo.Show(viewInfo.m_visible);
        m_manager->AddPane( viewPanel, paneInfo);
    }
    m_manager->Update();
}

//-----------------------------------------------------------------------------

void CardinalLayoutManager::destroyLayout()
{
    m_manager->UnInit() ;
    this->destroySubViews();
    delete m_manager;
}

//-----------------------------------------------------------------------------

} // namespace fwGui



