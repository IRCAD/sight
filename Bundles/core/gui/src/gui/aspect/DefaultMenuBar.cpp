/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <wx/app.h>
#include <wx/wx.h>

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwWX/convert.hpp>

#include "gui/action/IAction.hpp"
#include "gui/aspect/IMenu.hpp"
#include "gui/aspect/DefaultMenuBar.hpp"


REGISTER_SERVICE( ::gui::aspect::IMenuBar , ::gui::aspect::DefaultMenuBar , ::fwTools::Object );

namespace gui
{

namespace aspect
{


//-----------------------------------------------------------------------------

DefaultMenuBar::DefaultMenuBar() throw()
{}

//-----------------------------------------------------------------------------

DefaultMenuBar::~DefaultMenuBar() throw()
{}

//-----------------------------------------------------------------------------

void DefaultMenuBar::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    SLM_FATAL_IF( "Sorry, missing menus configuration" , !m_configuration->findConfigurationElement("menus") );
    ::fwRuntime::ConfigurationElement::sptr menusCfgElt = m_configuration->findConfigurationElement("menus");
    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = menusCfgElt->find("menu");
    SLM_ASSERT("Sorry, no menu configuration specified", !vectConfig.empty());

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = vectConfig.begin() ; iter != vectConfig.end() ; ++iter)
    {
        SLM_ASSERT("Sorry, uid attribute missing", (*iter)->hasAttribute("uid"));
        m_menusUID.push_back( (*iter)->getExistingAttributeValue("uid") ) ;
    }
}

//-----------------------------------------------------------------------------

void DefaultMenuBar::starting() throw( ::fwTools::Failed )
{
    // Create MenuBar
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    SLM_ASSERT("Sorry, no wxFrame", frame ) ;
    wxMenuBar *menuBar =  frame->GetMenuBar();
    if(menuBar)
    {
        SLM_ASSERT("Sorry, MenuBar must be null or empty", frame->GetMenuBar()->GetMenuCount() == 0 );
    }
    else
    {
        frame->SetMenuBar( new wxMenuBar());
    }
    frame->Update();
    BOOST_FOREACH(std::string menuUID, m_menusUID)
    {
        OSLM_FATAL_IF("Service "<<menuUID<<" doesn't exist.", ! ::fwTools::UUID::exist(menuUID, ::fwTools::UUID::SIMPLE ));
        ::fwServices::IService::sptr service = ::fwServices::get( menuUID ) ;
        ::gui::aspect::IMenu::sptr menu = ::gui::aspect::IMenu::dynamicCast(service);
        OSLM_FATAL_IF("Service "<<menuUID<<" is not a Menu.", !menu);
        menu->start();
    }
}

//-----------------------------------------------------------------------------

void DefaultMenuBar::stopping() throw( ::fwTools::Failed )
{
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    SLM_ASSERT("Sorry, no wxFrame", frame );
    wxMenuBar *menuBar =  frame->GetMenuBar();
    SLM_ASSERT("Sorry, no menu bar", menuBar );

    BOOST_FOREACH(std::string menuUID, m_menusUID)
    {
        OSLM_FATAL_IF("Service "<<menuUID<<" doesn't exist.", ! ::fwTools::UUID::exist(menuUID, ::fwTools::UUID::SIMPLE ));
        ::fwServices::IService::sptr service = ::fwServices::get( menuUID ) ;
        service->stop();
    }
    m_menusUID.clear();
    frame->SetMenuBar(NULL);
}

//-----------------------------------------------------------------------------

void DefaultMenuBar::updating() throw(::fwTools::Failed)
{
    SLM_FATAL("ACH : an DefaultMenuBar service does not be updated." );
}

//-----------------------------------------------------------------------------

void DefaultMenuBar::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_FATAL("ACH : an DefaultMenuBar service does not received a message." );
}

//-----------------------------------------------------------------------------

}

}
