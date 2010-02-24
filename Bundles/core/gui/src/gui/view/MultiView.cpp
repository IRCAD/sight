/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <wx/window.h>
#include <wx/colour.h>

#include <fwTools/UUID.hpp>

#include <fwServices/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwWX/convert.hpp>

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
    SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );

    SLM_FATAL_IF( "missing views configuration" , !m_configuration->findConfigurationElement("views") );

    ::fwRuntime::ConfigurationElement::sptr viewsCfgElt = m_configuration->findConfigurationElement("views");

    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = viewsCfgElt->find("view");
    assert(!vectConfig.empty());

    m_panels.clear();
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = vectConfig.begin() ; iter != vectConfig.end() ; ++iter)
    {
        SLM_FATAL_IF("<views> node can only contain <view> node", (*iter)->getName()!="view" );
        ViewInfo vi;
        std::string uid;

        SLM_FATAL_IF("<view> node must contain uid attribute", !(*iter)->hasAttribute("uid") );
        uid = (*iter)->getExistingAttributeValue("uid");
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
            OSLM_ASSERT("Incorrect value for \"resizable\" attribute "<<resizable,
                               (resizable == "yes") || (resizable == "no"));
            vi.m_isResizable = (resizable=="yes") ;
        }

        if( (*iter)->hasAttribute("position") )
        {
            std::string position = (*iter)->getExistingAttributeValue("position") ;
            vi.m_position = ::boost::lexical_cast< int >(position);
        }

        if( (*iter)->hasAttribute("layer") )
        {
            std::string layer = (*iter)->getExistingAttributeValue("layer") ;
            vi.m_layer = ::boost::lexical_cast< int >(layer);
        }

        if( (*iter)->hasAttribute("row") )
        {
            std::string row = (*iter)->getExistingAttributeValue("row") ;
            vi.m_row = ::boost::lexical_cast< int >(row);
        }

        if( (*iter)->hasAttribute("visible") )
        {
            std::string visible = (*iter)->getExistingAttributeValue("visible") ;
            OSLM_ASSERT("Incorrect value for \"visible\" attribute "<<visible,
                    (visible == "true") || (visible == "false") ||
                    (visible == "yes") || (visible == "no"));
            vi.m_visible = ((visible == "true") || (visible == "yes"));
        }

        if( (*iter)->hasAttribute("caption") )
        {
            vi.m_caption.first = true;
            vi.m_caption.second = (*iter)->getExistingAttributeValue("caption") ;
        }

        if( (*iter)->hasAttribute("autoStart") )
        {
            std::string autostart = (*iter)->getExistingAttributeValue("autoStart");
            OSLM_ASSERT("Sorry, value "<<autostart<<" is not correct for attribute autoStart.",
                    autostart == "yes" || autostart == "no");
            vi.m_autostart = (autostart == "yes");
        }
        m_panels[uid] = vi;
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
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void MultiView::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initGuiParentContainer();

    assert( wxTheApp->GetTopWindow() );

    wxWindow * wxContainer = this->getWxContainer();
    m_manager = new wxAuiManager(  wxContainer  );
    PanelContainer::iterator pi = m_panels.begin();
    for ( pi; pi!= m_panels.end() ; ++pi )
    {
        wxPanel * viewPanel = new wxPanel(  wxContainer, wxNewId() , wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL );

        // Set the panel
        pi->second.m_panel = viewPanel;

        // Pane info configuration
        ::wxAuiPaneInfo paneInfo;
        if(pi->second.m_align=="center")        { paneInfo.Center(); }
        else if(pi->second.m_align=="right")    { paneInfo.Right(); }
        else if(pi->second.m_align=="left")     { paneInfo.Left(); }
        else if(pi->second.m_align=="bottom")   { paneInfo.Bottom();}
        else if(pi->second.m_align=="top")      { paneInfo.Top(); }
        if(!pi->second.m_isResizable)           { paneInfo.Fixed(); }
        paneInfo.CloseButton( false );
        paneInfo.Floatable( false );
        paneInfo.MaximizeButton( true );
        if(pi->second.m_caption.first)
        {
            paneInfo.CaptionVisible( true );
            paneInfo.Caption( ::fwWX::std2wx(pi->second.m_caption.second) );
        }
        else
        {
            paneInfo.CaptionVisible( false );
        }
        paneInfo.PaneBorder( false );
        paneInfo.MinSize( wxSize( pi->second.m_minSize.first, pi->second.m_minSize.second ) );
        paneInfo.Position(pi->second.m_position);
        paneInfo.Layer(pi->second.m_layer);
        paneInfo.Row(pi->second.m_row);
        paneInfo.Show(pi->second.m_visible);
        // rempli paneInfo avec pi->second
        m_manager->AddPane( viewPanel, paneInfo);

        this->registerWxContainer(pi->first, viewPanel);

        if(pi->second.m_autostart)
        {
            m_manager->Update();
            OSLM_ASSERT("Service "<<pi->first<<" doesn't exist.", ::fwTools::UUID::exist(pi->first, ::fwTools::UUID::SIMPLE ));
            ::fwServices::IService::sptr service = ::fwServices::get( pi->first ) ;
            service->start();
        }
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
    SLM_TRACE_FUNC();
    this->unregisterAllWxContainer();

    // Destroy wxAuiManager
    if( m_manager )
    {
        m_manager->UnInit() ;
        delete m_manager;
        m_manager = 0 ;
    }
    this->resetGuiParentContainer();
}

//-----------------------------------------------------------------------------

void MultiView::swappping() throw( ::fwTools::Failed )
{
    SLM_FATAL("ACH : This method is not implemented, why is called ?");
}

//-----------------------------------------------------------------------------

}

}

