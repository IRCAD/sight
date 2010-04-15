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

#include <fwWX/convert.hpp>

#include "gui/view/MultiTabView.hpp"

namespace gui
{
namespace view
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::view::IView , ::gui::view::MultiTabView , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

MultiTabView::MultiTabView() throw()
{}

//-----------------------------------------------------------------------------

MultiTabView::~MultiTabView() throw()
{}

//-----------------------------------------------------------------------------

void MultiTabView::configuring() throw( ::fwTools::Failed )
{
    assert( m_configuration->getName() == "service" );
    SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );

    SLM_FATAL_IF( "missing views configuration" , !m_configuration->findConfigurationElement("views") );
    ::fwRuntime::ConfigurationElement::sptr viewsCfgElt = m_configuration->findConfigurationElement("views");

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

        if( (*iter)->hasAttribute("name") )
        {
            vi.m_name = (*iter)->getExistingAttributeValue("name") ;
        }
        if( (*iter)->hasAttribute("selected") )
        {
            std::string isSelected = (*iter)->getExistingAttributeValue("selected");
            OSLM_ASSERT("Sorry, value "<<isSelected<<" is not correct for attribute isSelected.",
                    isSelected == "yes" || isSelected == "no");
            vi.m_isSelect = (isSelected == "yes");
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

void MultiTabView::reconfiguring() throw( ::fwTools::Failed )
{
    SLM_FATAL("ACH : This method is not implemented, does nothing, why is called ?");
}

//-----------------------------------------------------------------------------

void MultiTabView::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void MultiTabView::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initGuiParentContainer();

    assert( wxTheApp->GetTopWindow() );

    wxWindow * wxContainer = this->getWxContainer();
    wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
    wxContainer->SetSizer( boxSizer );
    m_notebook = new wxNotebook( wxContainer, wxNewId() );

    boxSizer->Add( m_notebook, 0, wxALL|wxEXPAND, 0);

    std::list<ViewInfo>::iterator pi = m_views.begin();
    for ( pi; pi!= m_views.end() ; ++pi )
    {
        wxPanel * viewPanel = new wxPanel(  m_notebook, wxNewId());
        // Set the panel
        pi->m_panel = viewPanel;
        m_notebook->AddPage( viewPanel, ::fwWX::std2wx(pi->m_name), pi->m_isSelect );
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

void MultiTabView::updating() throw(::fwTools::Failed)
{
    // ACH Comment because udapte is made on service on layout switching in ConfigVisuActionService
    // SLM_FATAL("ACH : This method is not implemented because it does nothing, why is called ?");
}

//-----------------------------------------------------------------------------

void MultiTabView::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_FATAL("ACH : This method is not implemented because it does nothing, why is called ?");
}
//-----------------------------------------------------------------------------

void MultiTabView::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

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

    this->unregisterAllWxContainer();
    this->resetGuiParentContainer();
}

//-----------------------------------------------------------------------------

void MultiTabView::swappping() throw( ::fwTools::Failed )
{
    SLM_FATAL("ACH : This method is not implemented, why is called ?");
}

//-----------------------------------------------------------------------------

}

}

