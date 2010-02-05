/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>

#include <fwServices/helper.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "gui/action/IAction.hpp"
#include "gui/aspect/IMenu.hpp"
#include "gui/aspect/IAspect.hpp"

namespace gui
{

namespace aspect
{

const std::string  IMenu::SEPARATOR_UUUID = "SEPARATOR_UUUID";

//-----------------------------------------------------------------------------

IMenu::IMenu() throw()
{}

//-----------------------------------------------------------------------------

IMenu::~IMenu() throw()
{}

//-----------------------------------------------------------------------------

void IMenu::configuring() throw( ::fwTools::Failed )
{
	SLM_ASSERT("name tag missing", m_configuration->hasAttribute("name")) ;
	m_menuName = m_configuration->getExistingAttributeValue("name");

	::fwRuntime::ConfigurationElementContainer::Iterator iter ;
	for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
	{
		if( (*iter)->getName() == "action" )
		{
			SLM_ASSERT("uid attribute missing", (*iter)->hasAttribute("uid"));
			SLM_ASSERT("uid attribute == SEPARATOR_UUUID", (*iter)->getExistingAttributeValue("uid")!=::gui::aspect::IMenu::SEPARATOR_UUUID);
			m_actionsUUID.push_back( (*iter)->getExistingAttributeValue("uid") ) ;
		}
		if( (*iter)->getName() == "separator" )
		{
			m_actionsUUID.push_back( ::gui::aspect::IMenu::SEPARATOR_UUUID ) ;
		}
	}

}

//-----------------------------------------------------------------------------

void IMenu::starting() throw( ::fwTools::Failed )
{
	wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
	SLM_ASSERT( "No wxFrame", frame ) ;

	// get MenuBar
	SLM_ASSERT( "No menu bar: MenuBar must be created by IAspect", frame->GetMenuBar() );
	wxMenuBar *menuBar =  frame->GetMenuBar();

	// create Menu
	SLM_ASSERT("Menu must not be yet created", menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) == wxNOT_FOUND );
	OSLM_TRACE("No menu named " << m_menuName << " :  creating it") ;
	menuBar->Append( new wxMenu , wxConvertMB2WX( m_menuName.c_str() ));

	// get Menu
	SLM_ASSERT("Menu must exist now", menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) != wxNOT_FOUND );
	wxMenu *menuFile = menuBar->GetMenu( menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) ) ;


	// starting appropriate actions
	std::vector< ::gui::action::IAction::sptr > allActions = ::fwServices::OSR::getServices< ::gui::action::IAction >() ;
	for(std::vector< std::string >::iterator iterUUID = m_actionsUUID.begin() ; iterUUID != m_actionsUUID.end() ; ++iterUUID )
	{
		if((*iterUUID) == ::gui::aspect::IMenu::SEPARATOR_UUUID)
		{
			m_separators.insert( menuFile->AppendSeparator() ) ;
		}
		else
		{
			bool actionIsFound = false;
			for( 	std::vector< ::gui::action::IAction::sptr >::iterator iterAction = allActions.begin();
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

void IMenu::stopping() throw( ::fwTools::Failed )
{
	wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
	SLM_ASSERT( "No menu bar: MenuBar must exist", frame->GetMenuBar() );
	wxMenuBar *menuBar =  frame->GetMenuBar();
	SLM_ASSERT("Menu must exist", menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) != wxNOT_FOUND );
	wxMenu *menuFile = menuBar->GetMenu( menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) ) ;


	// Stopping actions
	std::vector< ::gui::action::IAction::sptr > allActions = ::fwServices::OSR::getServices< ::gui::action::IAction >() ;
	for(std::vector< std::string >::iterator iterUUID = m_actionsUUID.begin() ; iterUUID != m_actionsUUID.end() ; ++iterUUID )
	{
		if((*iterUUID) != ::gui::aspect::IMenu::SEPARATOR_UUUID)
		{
			bool actionIsFound = false;
			for( 	std::vector< ::gui::action::IAction::sptr >::iterator iterAction = allActions.begin();
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
	menuBar->Remove( menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) ) ;
#endif
}

//-----------------------------------------------------------------------------

void IMenu::updating() throw(::fwTools::Failed)
{
	SLM_FATAL("ACH : an IMenu service does not be updated." );
}

//-----------------------------------------------------------------------------

void IMenu::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
	SLM_FATAL("ACH : an IMenu service does not received a message." );
}

//-----------------------------------------------------------------------------

}

}

