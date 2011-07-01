/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/evtloop.h>
#include <wx/image.h>

#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwServices/registry/ObjectService.hpp>

#include <fwWX/convert.hpp>

#include "fwGuiWx/layoutManager/FrameLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IFrameLayoutManager,
        ::fwGui::FrameLayoutManager,
         ::fwGui::layoutManager::IFrameLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

const std::map< ::fwGui::layoutManager::IFrameLayoutManager::Style, long> FrameLayoutManager::FWSTYLE_TO_WXSTYLE =
        ::boost::assign::map_list_of(::fwGui::layoutManager::IFrameLayoutManager::DEFAULT,wxDEFAULT_FRAME_STYLE)
                                    (::fwGui::layoutManager::IFrameLayoutManager::STAY_ON_TOP, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);

//-----------------------------------------------------------------------------

FrameLayoutManager::FrameLayoutManager()
{}

//-----------------------------------------------------------------------------

FrameLayoutManager::~FrameLayoutManager()
{}

//-----------------------------------------------------------------------------

void FrameLayoutManager::createFrame()
{
    SLM_TRACE_FUNC();
    FrameInfo frameInfo = this->getFrameInfo();

    wxEventLoopBase* eventLoop = wxEventLoop::GetActive();
    if (!eventLoop)
    {
        wxEventLoop::SetActive(new wxEventLoop() );
    }
    // wxWidget initialization
    wxInitAllImageHandlers();

    m_wxFrame = new wxFrame(wxTheApp->GetTopWindow(),
            wxNewId(),
            ::fwWX::std2wx(frameInfo.m_name),
             wxDefaultPosition,
             wxSize(frameInfo.m_minSize.first, frameInfo.m_minSize.second),
             FWSTYLE_TO_WXSTYLE.find(frameInfo.m_style)->second );

    if(!wxTheApp->GetTopWindow())
    {
        wxTheApp->SetTopWindow( m_wxFrame ) ;
    }
    m_wxFrame->SetMinSize(wxSize(frameInfo.m_minSize.first, frameInfo.m_minSize.second));

    if(!frameInfo.m_iconPath.empty())
    {
        wxIcon icon( ::fwWX::std2wx(frameInfo.m_iconPath.string()), wxBITMAP_TYPE_ICO );
        OSLM_ASSERT("Sorry, unable to create an icon instance from " << frameInfo.m_iconPath.string(), icon.Ok());
        m_wxFrame->SetIcon( icon );
    }
    m_wxFrame->Move( wxPoint(frameInfo.m_position.first, frameInfo.m_position.second) );
    m_wxFrame->SetSize( wxSize( frameInfo.m_size.first, frameInfo.m_size.second) );
    this->setState(frameInfo.m_state);

    m_wxFrame->Bind( wxEVT_CLOSE_WINDOW, &FrameLayoutManager::onCloseFrame, this,  m_wxFrame->GetId());
    m_wxFrame->Show();
    m_wxFrame->Refresh();

    ::fwGuiWx::container::WxContainer::NewSptr frameContainer;
    frameContainer->setWxContainer(m_wxFrame);
    m_frame = frameContainer;


    wxPanel *panel = new wxPanel(m_wxFrame, wxNewId());
    wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
    m_wxFrame->SetSizer(boxSizer);
    boxSizer->Add(panel, 1, wxALL|wxEXPAND);
    m_wxFrame->Layout();

    ::fwGuiWx::container::WxContainer::NewSptr container;
    container->setWxContainer(panel);
    m_container = container;
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::destroyFrame()
{
    this->getRefFrameInfo().m_state = this->getState();
    this->getRefFrameInfo().m_size.first = m_wxFrame->GetSize().GetWidth();
    this->getRefFrameInfo().m_size.second = m_wxFrame->GetSize().GetHeight();
    this->getRefFrameInfo().m_position.first = m_wxFrame->GetPosition().x;
    this->getRefFrameInfo().m_position.second = m_wxFrame->GetPosition().y;
    this->writeConfig();

    m_wxFrame->Show(false);
    m_wxFrame->Unbind( wxEVT_CLOSE_WINDOW, &FrameLayoutManager::onCloseFrame, this,  m_wxFrame->GetId());

    if ( wxTheApp->GetTopWindow() != m_wxFrame )
    {
        m_wxFrame->Reparent(NULL);
    }

    m_container->destroyContainer();
    m_frame->clean();
    m_frame->destroyContainer();

    m_frame.reset();
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::onCloseFrame(wxCloseEvent& event)
{
    SLM_TRACE_FUNC();
    this->m_closeCallback();
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::setState( FrameState state )
{
    // Updates the window state.
    switch( state )
    {
    case ICONIZED:
        m_wxFrame->Iconize();
        break;

    case MAXIMIZED:
        m_wxFrame->Maximize();
        break;

    case FULL_SCREEN:
        m_wxFrame->ShowFullScreen( true );
        break;
    }
}

//-----------------------------------------------------------------------------

::fwGui::layoutManager::IFrameLayoutManager::FrameState FrameLayoutManager::getState()
{
    FrameState state( UNKNOWN );

    if( m_wxFrame->IsIconized() )
    {
        state = ICONIZED;
    }
    else if( m_wxFrame->IsMaximized() )
    {
        state = MAXIMIZED;
    }
    else if( m_wxFrame->IsFullScreen() )
    {
        state = FULL_SCREEN;
    }
    return state;
}

//-----------------------------------------------------------------------------

} // namespace fwGui



