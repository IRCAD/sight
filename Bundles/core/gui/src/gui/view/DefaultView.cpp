/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>

#include <fwTools/UUID.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwData/Object.hpp>

#include "gui/view/DefaultView.hpp"

namespace gui
{
namespace view
{

REGISTER_SERVICE( ::gui::view::IView , ::gui::view::DefaultView , ::fwTools::Object ) ;


DefaultView::DefaultView() throw()
{}

//-----------------------------------------------------------------------------

DefaultView::~DefaultView() throw()
{}

//-----------------------------------------------------------------------------

void DefaultView::configuring() throw( ::fwTools::Failed )
{
    assert( m_configuration->getName() == "service" );
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
        if((*iter)->hasAttribute("autoStart"))
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

void DefaultView::starting() throw(::fwTools::Failed)
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
            if(pi == m_panels.begin())
            {
                paneInfo.Center();
            }
            else
            {
                paneInfo.Right();
            }
            paneInfo.CloseButton( false );
            paneInfo.Floatable( false );
            paneInfo.MaximizeButton( true );
            paneInfo.CaptionVisible( false );
            paneInfo.PaneBorder( false );
            paneInfo.MinSize( wxSize( pi->second.m_minSize.first, pi->second.m_minSize.second ) );
            paneInfo.Show(true);
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

void DefaultView::stopping() throw(::fwTools::Failed)
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

void DefaultView::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultView::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultView::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}
}

