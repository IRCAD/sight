/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <wx/window.h>
#include <wx/colour.h>

#include <fwServices/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "gui/Manager.hpp"
#include "gui/view/MultiSizerView.hpp"

namespace gui
{
namespace view
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::view::IView , ::gui::view::MultiSizerView , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

MultiSizerView::MultiSizerView() throw() : m_orient(wxVERTICAL)
{}

//-----------------------------------------------------------------------------

MultiSizerView::~MultiSizerView() throw()
{}

//-----------------------------------------------------------------------------

void MultiSizerView::configuring() throw( ::fwTools::Failed )
{
        assert( m_configuration->getName() == "service" );
        SLM_FATAL_IF( "missing win configuration" , ! m_configuration->findConfigurationElement("win") );

        std::string guiContainerId =  m_configuration->findConfigurationElement("win")->getExistingAttributeValue("guiContainerId") ;
        OSLM_TRACE("Configuring win identified by " << guiContainerId ) ;
        m_guiContainerId = ::boost::lexical_cast<int >(guiContainerId) ;

        SLM_FATAL_IF( "missing views configuration" , !m_configuration->findConfigurationElement("views") );
        ::fwRuntime::ConfigurationElement::sptr viewsCfgElt = m_configuration->findConfigurationElement("views");
        if(viewsCfgElt->hasAttribute("orientation") )
        {
                std::string orient = viewsCfgElt->getExistingAttributeValue("orientation") ;
                if(orient == "vertical")
                {
                        m_orient = wxVERTICAL  ;
                }
                else if(orient == "horizontal")
                {
                        m_orient = wxHORIZONTAL   ;
                }
        }

        std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = viewsCfgElt->find("view");
        assert(!vectConfig.empty());

        ::fwRuntime::ConfigurationElementContainer::Iterator iter;
        for (iter = vectConfig.begin() ; iter != vectConfig.end() ; ++iter)
        {
                SLM_FATAL_IF("<views> node can only contain <view> node", (*iter)->getName()!="view" );
                ViewInfo vi;

                SLM_FATAL_IF(" <view> node must contain guiContainerId attribute", !(*iter)->hasAttribute("guiContainerId") );
                vi.m_guid = ::boost::lexical_cast<int >( (*iter)->getExistingAttributeValue("guiContainerId") );
                if( (*iter)->hasAttribute("proportion") )
                {
                        std::string proportion = (*iter)->getExistingAttributeValue("proportion") ;
                        vi.m_proportion = ::boost::lexical_cast< int >(proportion) ;
                }

                if( (*iter)->hasAttribute("border") )
                {
                        std::string border = (*iter)->getExistingAttributeValue("border") ;
                        vi.m_border = ::boost::lexical_cast< int >(border) ;
                }

                m_views.push_back(vi);
        }
}

//-----------------------------------------------------------------------------

void MultiSizerView::reconfiguring() throw( ::fwTools::Failed )
{
        SLM_FATAL("ACH : This method is not implemented, does nothing, why is called ?");
}

//-----------------------------------------------------------------------------

void MultiSizerView::info(std::ostream &_sstream )
{
        _sstream << "GUI View with ID = " <<  m_guiContainerId;
}

//-----------------------------------------------------------------------------

void MultiSizerView::starting() throw(::fwTools::Failed)
{
        OSLM_ASSERT("Gui container must exist", wxWindow::FindWindowById( m_guiContainerId )) ;

        assert( wxTheApp->GetTopWindow() );

        wxWindow * wxContainer = wxWindow::FindWindowById( m_guiContainerId );
        m_sizer = new wxBoxSizer( m_orient );
        wxContainer->SetSizer( m_sizer );

        std::list<ViewInfo>::iterator pi = m_views.begin();
        for ( pi; pi!= m_views.end() ; ++pi )
        {
                wxPanel * viewPanel = new wxPanel(  wxContainer, pi->m_guid , wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL );

                // Set the panel
                pi->m_panel = viewPanel;
                m_sizer->Add( viewPanel, pi->m_proportion, wxALL|wxEXPAND, pi->m_border);
        }
}
//-----------------------------------------------------------------------------

void MultiSizerView::updating() throw(::fwTools::Failed)
{
        // ACH Comment because udapte is made on service on layout switching in ConfigVisuActionService
        // SLM_FATAL("ACH : This method is not implemented because it does nothing, why is called ?");
}

//-----------------------------------------------------------------------------

void MultiSizerView::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
        SLM_FATAL("ACH : This method is not implemented because it does nothing, why is called ?");
}
//-----------------------------------------------------------------------------

void MultiSizerView::stopping() throw(::fwTools::Failed)
{
        std::list<ViewInfo>::iterator pi = m_views.begin();
        for ( pi; pi!= m_views.end() ; ++pi )
        {
                if ( pi->m_panel )
                {
                        pi->m_panel->Destroy() ;
                        pi->m_panel = 0 ;
                }
        }

        wxWindow * wxContainer = wxWindow::FindWindowById( m_guiContainerId );
}

//-----------------------------------------------------------------------------

void MultiSizerView::swappping() throw( ::fwTools::Failed )
{
        SLM_FATAL("ACH : This method is not implemented, why is called ?");
}

//-----------------------------------------------------------------------------

}

}

