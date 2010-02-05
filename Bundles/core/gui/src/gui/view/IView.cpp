/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>

#include <fwServices/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>


#include "gui/view/IView.hpp"
#include "gui/Manager.hpp"

namespace gui
{
namespace view
{


IView::IView() throw(): m_minWidth(-1), m_minHeight(-1)
{
}

//-----------------------------------------------------------------------------

IView::~IView() throw()
{
}

//-----------------------------------------------------------------------------

void IView::configuring() throw( ::fwTools::Failed )
{
    assert( m_configuration->getName() == "view" );
    assert( m_configuration->hasAttribute("guiContainerId")) ;
    std::string guiContainerId = m_configuration->getExistingAttributeValue("guiContainerId") ;
    OSLM_TRACE("Configuring view identified by " << guiContainerId ) ;
    m_guiContainerId = ::boost::lexical_cast<int >(guiContainerId) ;
    if(m_configuration->hasAttribute("minWidth"))
    {
        std::string width = m_configuration->getExistingAttributeValue("minWidth") ;
        m_minWidth = ::boost::lexical_cast<int >(width) ;
    }
    if(m_configuration->hasAttribute("minHeight"))
    {
        std::string height = m_configuration->getExistingAttributeValue("minHeight") ;
        m_minHeight = ::boost::lexical_cast<int >(height) ;
    }
}

//-----------------------------------------------------------------------------

void IView::info(std::ostream &_sstream )
{
    _sstream << "GUI View with ID = " <<  m_guiContainerId;
}

//-----------------------------------------------------------------------------

void IView::starting() throw(::fwTools::Failed)
{
    OSLM_ASSERT("Register a view with already defined id "
            << m_guiContainerId << " check your configuration",
            !wxWindow::FindWindowById( m_guiContainerId )) ;

    assert( wxTheApp->GetTopWindow() );
    // If first registered view, one considers it is associated to the centre pane, else side pane
    if( ::gui::Manager::getTopAuiManager()->GetAllPanes().IsEmpty() )
    {
        wxAuiPaneInfo win;
        win.MinSize( m_minWidth, m_minHeight ).CentrePane().PaneBorder( false );
        wxPanel *panel = new wxPanel( wxTheApp->GetTopWindow() ,  m_guiContainerId ) ;
        ::gui::Manager::getTopAuiManager()->AddPane( panel , win ) ;
    }
    else
    {
        wxAuiPaneInfo winSide;
        wxPanel *sidePanel = new wxPanel( wxTheApp->GetTopWindow() , m_guiContainerId ) ;
        winSide.MinSize( m_minWidth, m_minHeight ).Right().PaneBorder( false );
        ::gui::Manager::getTopAuiManager()->AddPane( sidePanel , winSide ) ;
    }

    ::gui::Manager::getTopAuiManager()->Update();

}
//-----------------------------------------------------------------------------

void IView::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("IView::updating");
}

//-----------------------------------------------------------------------------

void IView::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_FATAL("An IView service does not received a message." );
}
//-----------------------------------------------------------------------------

void IView::stopping() throw(::fwTools::Failed)
{
    OSLM_ASSERT("stopping a view with unknown id " << m_guiContainerId,
            wxWindow::FindWindowById( m_guiContainerId ) );

    assert( ::gui::Manager::getTopAuiManager() );
    ::gui::Manager::getTopAuiManager()->DetachPane(wxWindow::FindWindowById( m_guiContainerId ));
    wxWindow::FindWindowById( m_guiContainerId )->Destroy();
    ::gui::Manager::getTopAuiManager()->Update();
}

//-----------------------------------------------------------------------------

int IView::getGuiContainerId()
{
    return m_guiContainerId ;
}

//-----------------------------------------------------------------------------

wxWindow * IView::getWxContainer() const
{
    return wxWindow::FindWindowById( m_guiContainerId );
}

//-----------------------------------------------------------------------------

}

}

