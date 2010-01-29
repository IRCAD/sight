/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <wx/window.h>
#include <wx/colour.h>

#include <fwServices/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "gui/Manager.hpp"
#include "gui/view/MultiView.hpp"

namespace gui
{
namespace view
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::view::IView , ::gui::view::MultiView , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

MultiView::MultiView() throw()
{}

//-----------------------------------------------------------------------------

MultiView::~MultiView() throw()
{}

//-----------------------------------------------------------------------------

void MultiView::configuring() throw( ::fwTools::Failed )
{


	assert( m_configuration->getName() == "service" );
	SLM_FATAL_IF( "missing win configuration" , ! m_configuration->findConfigurationElement("win") );

	std::string guiContainerId =  m_configuration->findConfigurationElement("win")->getExistingAttributeValue("guiContainerId") ;
	OSLM_TRACE("Configuring win identified by " << guiContainerId ) ;
	m_guiContainerId = ::boost::lexical_cast<int >(guiContainerId) ;



	SLM_FATAL_IF( "missing views configuration" , !m_configuration->findConfigurationElement("views") );
	::fwRuntime::ConfigurationElement::sptr viewsCfgElt = m_configuration->findConfigurationElement("views");

	std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = viewsCfgElt->find("view");
	assert(!vectConfig.empty());

	::fwRuntime::ConfigurationElementContainer::Iterator iter;
	for (iter = vectConfig.begin() ; iter != vectConfig.end() ; ++iter)
	{
		SLM_FATAL_IF("<views> node can only contain <view> node", (*iter)->getName()!="view" );
		ViewInfo vi;
		int guid=-1;

		SLM_FATAL_IF(" <view> node must contain guiContainerId attribute", !(*iter)->hasAttribute("guiContainerId") );
		guid = ::boost::lexical_cast<int >( (*iter)->getExistingAttributeValue("guiContainerId") );
		if( (*iter)->hasAttribute("align") )
		{
			vi.m_align = (*iter)->getExistingAttributeValue("align");
		}

		if( (*iter)->hasAttribute("minWidth") )
		{
			std::string width = (*iter)->getExistingAttributeValue("minWidth") ;
			vi.m_minSize.first = ::boost::lexical_cast< int >(width) ;
		}

		if( (*iter)->hasAttribute("minHeight") )
		{
			std::string height = (*iter)->getExistingAttributeValue("minHeight") ;
			vi.m_minSize.second = ::boost::lexical_cast< int >(height) ;
		}

		if( (*iter)->hasAttribute("resizable") )
		{
			std::string resizable = (*iter)->getExistingAttributeValue("resizable") ;
			vi.m_isResizable = (resizable=="yes") ;
		}
		m_panels[guid] = vi;
	}
}

//-----------------------------------------------------------------------------

void MultiView::reconfiguring() throw( ::fwTools::Failed )
{
	SLM_FATAL("ACH : This method is not implemented, does nothing, why is called ?");
}

//-----------------------------------------------------------------------------

void MultiView::info(std::ostream &_sstream )
{
	_sstream << "GUI View with ID = " <<  m_guiContainerId;
}

//-----------------------------------------------------------------------------

void MultiView::starting() throw(::fwTools::Failed)
{
	OSLM_ASSERT("Gui container must exist", wxWindow::FindWindowById( m_guiContainerId )) ;

	assert( wxTheApp->GetTopWindow() );

	wxWindow * wxContainer = wxWindow::FindWindowById( m_guiContainerId );
	m_manager = new wxAuiManager(  wxContainer  );
	PanelContainer::iterator pi = m_panels.begin();
	for ( pi; pi!= m_panels.end() ; ++pi )
	{
		wxPanel * viewPanel = new wxPanel(  wxContainer, pi->first , wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL );

		// Set the panel
		pi->second.m_panel = viewPanel;

		// Pane info configuration
		::wxAuiPaneInfo paneInfo;
		if(pi->second.m_align=="center")		{ paneInfo.Center(); }
		else if(pi->second.m_align=="right")	{ paneInfo.Right(); }
		else if(pi->second.m_align=="left")		{ paneInfo.Left(); }
		else if(pi->second.m_align=="bottom")	{ paneInfo.Bottom();}
		else if(pi->second.m_align=="top")		{ paneInfo.Top(); }
		if(!pi->second.m_isResizable)			{ paneInfo.Fixed(); }
		paneInfo.CloseButton( false );
		paneInfo.Floatable( false );
		paneInfo.MaximizeButton( true );
		paneInfo.CaptionVisible( false );
		paneInfo.PaneBorder( false );
		paneInfo.MinSize( wxSize( pi->second.m_minSize.first, pi->second.m_minSize.second ) );

		// rempli paneInfo avec pi->second
		m_manager->AddPane( viewPanel, paneInfo);
	}

	m_manager->Update();

}
//-----------------------------------------------------------------------------

void MultiView::updating() throw(::fwTools::Failed)
{
	// ACH Comment because udapte is made on service on layout switching in ConfigVisuActionService
	// SLM_FATAL("ACH : This method is not implemented because it does nothing, why is called ?");
}

//-----------------------------------------------------------------------------

void MultiView::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
	SLM_FATAL("ACH : This method is not implemented because it does nothing, why is called ?");
}
//-----------------------------------------------------------------------------

void MultiView::stopping() throw(::fwTools::Failed)
{
	// Destroy wxAuiManager
	if( m_manager )
	{
		m_manager->UnInit() ;
		delete m_manager;
		m_manager = 0 ;
	}

	PanelContainer::iterator pi = m_panels.begin();
	for ( pi; pi!= m_panels.end() ; ++pi )
	{
		if ( pi->second.m_panel )
		{
			pi->second.m_panel->Destroy() ;
			pi->second.m_panel = 0 ;
		}
	}

	wxWindow * wxContainer = wxWindow::FindWindowById( m_guiContainerId );
}

//-----------------------------------------------------------------------------

void MultiView::swappping() throw( ::fwTools::Failed )
{
	SLM_FATAL("ACH : This method is not implemented, why is called ?");
}

//-----------------------------------------------------------------------------

}

}

