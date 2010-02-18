/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/UUID.hpp>

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
    SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );

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
    m_views.clear();
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = vectConfig.begin() ; iter != vectConfig.end() ; ++iter)
    {
        SLM_FATAL_IF("<views> node can only contain <view> node", (*iter)->getName()!="view" );
        ViewInfo vi;

        SLM_FATAL_IF("<view> node must contain uid attribute", !(*iter)->hasAttribute("uid") );
        vi.m_uid = (*iter)->getExistingAttributeValue("uid");
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
        if( (*iter)->hasAttribute("autoStart") )
        {
            std::string autostart = (*iter)->getExistingAttributeValue("autoStart");
            OSLM_ASSERT("Sorry, value "<<autostart<<" is not correct for attribute autoStart.",
                    autostart == "yes" || autostart == "no");
            vi.m_autostart = (autostart == "yes");
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
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void MultiSizerView::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initGuiParentContainer();

    assert( wxTheApp->GetTopWindow() );

    wxWindow * wxContainer = this->getWxContainer();
    m_sizer = new wxBoxSizer( m_orient );
    wxContainer->SetSizer( m_sizer );
    wxContainer->Layout();

    std::list<ViewInfo>::iterator pi = m_views.begin();
    for ( pi; pi!= m_views.end() ; ++pi )
    {
        wxPanel * viewPanel = new wxPanel(  wxContainer, wxNewId() , wxDefaultPosition, wxSize( pi->m_minSize.first, pi->m_minSize.second ), wxNO_BORDER | wxTAB_TRAVERSAL );

        // Set the panel
        pi->m_panel = viewPanel;
        m_sizer->Add( viewPanel, pi->m_proportion, wxALL|wxEXPAND, pi->m_border);
        this->registerWxContainer(pi->m_uid, pi->m_panel);

        if(pi->m_autostart)
        {
            OSLM_ASSERT("Service "<<pi->m_uid<<" doesn't exist.", ::fwTools::UUID::exist(pi->m_uid, ::fwTools::UUID::SIMPLE ));
            ::fwServices::IService::sptr service = ::fwServices::get( pi->m_uid ) ;
            service->start();
        }
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
    SLM_TRACE_FUNC();
    this->unregisterAllWxContainer();
    this->resetGuiParentContainer();
}

//-----------------------------------------------------------------------------

void MultiSizerView::swappping() throw( ::fwTools::Failed )
{
    SLM_FATAL("ACH : This method is not implemented, why is called ?");
}

//-----------------------------------------------------------------------------

}

}

