/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>

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
#include "gui/aspect/IAspect.hpp"
#include "gui/aspect/DefaultMenu.hpp"


REGISTER_SERVICE( ::gui::aspect::IMenu , ::gui::aspect::DefaultMenu , ::fwTools::Object );

namespace gui
{

namespace aspect
{
const std::string  DefaultMenu::SEPARATOR_UID = "SEPARATOR_UID";

//-----------------------------------------------------------------------------

DefaultMenu::DefaultMenu() throw()
{}

//-----------------------------------------------------------------------------

DefaultMenu::~DefaultMenu() throw()
{}

//-----------------------------------------------------------------------------

void DefaultMenu::configuring() throw( ::fwTools::Failed )
{
    SLM_ASSERT("name tag missing", m_configuration->hasAttribute("name")) ;
    m_menuName = m_configuration->getExistingAttributeValue("name");

    ::fwRuntime::ConfigurationElementContainer::Iterator iter ;
    for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
    {
        if( (*iter)->getName() == "action" )
        {
            SLM_ASSERT("uid attribute missing", (*iter)->hasAttribute("uid"));
            SLM_ASSERT("uid attribute == SEPARATOR_UID", (*iter)->getExistingAttributeValue("uid")!=::gui::aspect::DefaultMenu::SEPARATOR_UID);
            m_actionsUID.push_back( (*iter)->getExistingAttributeValue("uid") ) ;
        }
        if( (*iter)->getName() == "separator" )
        {
            m_actionsUID.push_back( ::gui::aspect::DefaultMenu::SEPARATOR_UID ) ;
        }
    }

}

//-----------------------------------------------------------------------------

void DefaultMenu::starting() throw( ::fwTools::Failed )
{
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    SLM_ASSERT( "No wxFrame", frame ) ;

    // get MenuBar
    SLM_ASSERT( "No menu bar: MenuBar must be created by IAspect", frame->GetMenuBar() );
    wxMenuBar *menuBar =  frame->GetMenuBar();

    // create Menu
    SLM_ASSERT("Menu must not be yet created", menuBar->FindMenu( ::fwWX::std2wx( m_menuName ) ) == wxNOT_FOUND );
    OSLM_TRACE("No menu named " << m_menuName << " :  creating it") ;
    menuBar->Append( new wxMenu , ::fwWX::std2wx( m_menuName ));

    // get Menu
    SLM_ASSERT("Menu must exist now", menuBar->FindMenu( ::fwWX::std2wx( m_menuName ) ) != wxNOT_FOUND );
    wxMenu *menuFile = menuBar->GetMenu( menuBar->FindMenu( ::fwWX::std2wx( m_menuName ) ) ) ;


    // starting appropriate actions
    std::vector< ::gui::action::IAction::sptr > allActions = ::fwServices::OSR::getServices< ::gui::action::IAction >() ;
    for(std::vector< std::string >::iterator iterUUID = m_actionsUID.begin() ; iterUUID != m_actionsUID.end() ; ++iterUUID )
    {
        if((*iterUUID) == ::gui::aspect::DefaultMenu::SEPARATOR_UID)
        {
            m_separators.insert( menuFile->AppendSeparator() ) ;
        }
        else
        {
            bool actionIsFound = false;
            for(    std::vector< ::gui::action::IAction::sptr >::iterator iterAction = allActions.begin();
                    iterAction != allActions.end() && ! actionIsFound ;
                    ++iterAction )
            {
                if( (*iterAction)->getUUID() == *iterUUID )
                {
                    (*iterAction)->setMenuName( m_menuName ) ;
                    (*iterAction)->start() ;
                    actionIsFound = true;
                }
            }
            OSLM_ASSERT("Action "<<  *iterUUID << " Not Found", actionIsFound);
        }
    }

}

//-----------------------------------------------------------------------------

void DefaultMenu::stopping() throw( ::fwTools::Failed )
{
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    SLM_ASSERT( "No menu bar: MenuBar must exist", frame->GetMenuBar() );
    wxMenuBar *menuBar =  frame->GetMenuBar();
    SLM_ASSERT("Menu must exist", menuBar->FindMenu( ::fwWX::std2wx( m_menuName ) ) != wxNOT_FOUND );
    wxMenu *menuFile = menuBar->GetMenu( menuBar->FindMenu( ::fwWX::std2wx( m_menuName ) ) ) ;


    // Stopping actions
    std::vector< ::gui::action::IAction::sptr > allActions = ::fwServices::OSR::getServices< ::gui::action::IAction >() ;
    for(std::vector< std::string >::iterator iterUUID = m_actionsUID.begin() ; iterUUID != m_actionsUID.end() ; ++iterUUID )
    {
        if((*iterUUID) != ::gui::aspect::DefaultMenu::SEPARATOR_UID)
        {
            bool actionIsFound = false;
            for(    std::vector< ::gui::action::IAction::sptr >::iterator iterAction = allActions.begin();
                iterAction != allActions.end() && ! actionIsFound ;
                ++iterAction )
            {
                if( (*iterAction)->getUUID() == *iterUUID )
                {
                    (*iterAction)->stop() ;
                    actionIsFound = true;
                }
            }
            SLM_ASSERT("Action Not Found", actionIsFound);
        }
    }

    // removes separators
    for( std::set< wxMenuItem* >::iterator iter = m_separators.begin() ; iter != m_separators.end() ; ++iter )
    {
        menuFile->Remove( *iter );
    }
    m_separators.clear() ;


    ///@todo Menu not empty on MAC with specials Actions like help, Quit...
#ifndef __MACOSX__
    SLM_ASSERT( "Menu must be empty before his destruction.", menuFile->GetMenuItems().size() == 0 );
    menuBar->Remove( menuBar->FindMenu( ::fwWX::std2wx( m_menuName ) ) ) ;
#endif
}

//-----------------------------------------------------------------------------

void DefaultMenu::updating() throw(::fwTools::Failed)
{
    SLM_FATAL("ACH : an DefaultMenu service does not be updated." );
}

//-----------------------------------------------------------------------------

void DefaultMenu::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_FATAL("ACH : an DefaultMenu service does not received a message." );
}

//-----------------------------------------------------------------------------

}

}
