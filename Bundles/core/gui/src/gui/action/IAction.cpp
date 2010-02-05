/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>

#include <wx/wx.h>

#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "gui/action/IAction.hpp"
#include "gui/Manager.hpp"

namespace gui
{
namespace action
{

const std::map<std::string, int> IAction::SPECIAL_ACTION_TO_WXID =
												::boost::assign::map_list_of("QUIT",wxID_EXIT)
																			("ABOUT",wxID_ABOUT)
																			("HELP",wxID_HELP)
																			("NEW",wxID_NEW);


IAction::IAction() throw() : m_shortcutDef(""),
					m_isCheckable(false),
					m_isRadio(false),
					m_isCheck(false),
					m_enable(true)
{
}

//-----------------------------------------------------------------------------

IAction::~IAction() throw()
{
}

//-----------------------------------------------------------------------------

void IAction::configuring() throw( ::fwTools::Failed )
{
	SLM_TRACE("configuring action") ;
	SLM_ASSERT("id tag deprecated", !m_configuration->hasAttribute("id"));
	SLM_ASSERT("menu tag deprecated", !m_configuration->hasAttribute("menu"));
	SLM_ASSERT("name tag missing", m_configuration->hasAttribute("name"));

	if( m_configuration->hasAttribute("specialAction") )
	{
		std::string specialActionName = m_configuration->getExistingAttributeValue("specialAction") ;
		OSLM_TRACE("Action identifier : " << specialActionName ) ;
		SLM_ASSERT("specialActionName unknown", SPECIAL_ACTION_TO_WXID.find(specialActionName) != SPECIAL_ACTION_TO_WXID.end() );
		m_actionIdInMenu = SPECIAL_ACTION_TO_WXID.find(specialActionName)->second;
	}
	else
	{
		m_actionIdInMenu  = wxNewId();
	}

	if( m_configuration->hasAttribute("name") )
	{
		m_actionNameInMenu = m_configuration->getExistingAttributeValue("name") ;
		OSLM_TRACE("Action name : " << m_actionNameInMenu ) ;
	}

	if( m_configuration->hasAttribute("shortcut") )
	{
		m_shortcutDef = m_configuration->getExistingAttributeValue("shortcut") ;
		OSLM_TRACE("shortcut : " << m_shortcutDef ) ;
	}
	if( m_configuration->hasAttribute("style") )
	{
		std::string style = m_configuration->getExistingAttributeValue("style") ;
		OSLM_TRACE("style : " << style ) ;
		m_isCheckable = (style == "check");
		m_isRadio = (style == "radio");

		if ((m_isCheckable || m_isRadio) && m_configuration->hasAttribute("state") )
		{
			std::string state = m_configuration->getExistingAttributeValue("state");
			m_isCheck = (state == "checked");
		}
	}
	if( m_configuration->hasAttribute("enable") )
	{
		std::string enable = m_configuration->getExistingAttributeValue("enable") ;
		OSLM_TRACE("enable : " << enable ) ;
		m_enable = (enable =="true");
	}
}

//-----------------------------------------------------------------------------

void IAction::info(std::ostream &_sstream )
{
	_sstream << "IAction" << std::endl;
}

//-----------------------------------------------------------------------------

void IAction::starting() throw(::fwTools::Failed)
{
	OSLM_TRACE("starting action " << this->getId() << " : info = " << *this) ;
	SLM_ASSERT("IAction must be associated with a menu", !m_menuName.empty());
	SLM_ASSERT("Action already registered", !::gui::Manager::isRegistered(m_actionIdInMenu));

	// get Frame
	wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
	SLM_ASSERT( "No wxFrame", frame ) ;

	// get MenuBar
	SLM_ASSERT( "No menu bar: MenuBar must be created by IAspect", frame->GetMenuBar() );
	wxMenuBar *menuBar =  frame->GetMenuBar();

	// get Menu
	SLM_ASSERT("Menu must be created", menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) != wxNOT_FOUND );
	wxMenu *menuFile = menuBar->GetMenu( menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) ) ;

	// create shortcut
	if( m_shortcutDef.empty() )
	{
		m_shortcut = ::boost::shared_ptr< ::gui::action::Shortcut >();
	}
	else
	{
		m_shortcut =  ::gui::action::Shortcut::New( m_shortcutDef );
		m_actionNameInMenu += "\t" + m_shortcut->toString();
	}

	// Adds menu item
	SLM_ASSERT( "wxMenuItem already exist", menuFile->FindItem( m_actionIdInMenu ) == NULL );
	if(m_isCheckable || m_isRadio)
	{
        wxItemKind kind = m_isRadio ? wxITEM_RADIO : wxITEM_CHECK;
		menuFile->Append( new wxMenuItem(menuFile, m_actionIdInMenu , wxConvertMB2WX( m_actionNameInMenu.c_str() ),_(""), kind ) ) ;
	}
	else
	{
		menuFile->Append( new wxMenuItem(menuFile, m_actionIdInMenu , wxConvertMB2WX( m_actionNameInMenu.c_str() )) ) ;
	}

	::gui::Manager::registerAction( ::boost::dynamic_pointer_cast< ::gui::action::IAction>( shared_from_this() )) ;

	setEnable(m_enable);
	setCheck(m_isCheck);
}

//-----------------------------------------------------------------------------

void IAction::stopping() throw(::fwTools::Failed)
{
	// get Frame
	wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
	SLM_ASSERT( "No wxFrame", frame ) ;

	// get MenuBar
	SLM_ASSERT( "No menu bar: MenuBar must be created by IAspect", frame->GetMenuBar() );
	wxMenuBar *menuBar =  frame->GetMenuBar();

	// get Menu
	SLM_ASSERT("Menu must exist", menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) != wxNOT_FOUND );
	wxMenu *menuFile = menuBar->GetMenu( menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) ) ;

	SLM_ASSERT( "wxMenuItem must exist", menuFile->FindItem( m_actionIdInMenu ) != NULL );


#ifdef __MACOSX__
		if (m_actionIdInMenu < 4999 || m_actionIdInMenu >= 6000)  // From 4999 to 6000, wxWidgets ID reserved, don't touch ! [wxID_LOWEST wxID_HIGHEST]
		{
			menuFile->Remove( m_actionIdInMenu );
		}
		else
		{
			OSLM_WARN("::gui::IAction try to remove wxWidgets identifier : " << m_actionIdInMenu);
		}
#else
		menuFile->Remove( m_actionIdInMenu );
#endif

	::gui::Manager::unregisterAction( ::boost::dynamic_pointer_cast< ::gui::action::IAction >( shared_from_this() )) ;
}

bool IAction::isEnable()
{
	return m_enable;
}

//-----------------------------------------------------------------------------

void IAction::setCheck(bool _check)
{
	m_isCheck = _check;
	wxMenuItem* item = this->getMenuItem();
	if(item && item->IsCheckable() && (m_isCheckable || (_check && m_isRadio)))
	{
		item->Check(m_isCheck);
		wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
		wxToolBar* toolBar =  frame->GetToolBar();
		if(toolBar != NULL)
		{
			if( toolBar->FindById( m_actionIdInMenu ) != NULL )
			{
				toolBar->ToggleTool(m_actionIdInMenu, m_isCheck);
			}
		}
	}
}

//-----------------------------------------------------------------------------

void IAction::setEnable(bool _enable)
{
	m_enable = _enable;
	wxMenuItem* item = getMenuItem();
	if(item)
	{
		item->Enable( m_enable);
		wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
		wxToolBar* toolBar =  frame->GetToolBar();
		if(toolBar != NULL)
		{
			if( toolBar->FindById( m_actionIdInMenu ) != NULL )
			{
				toolBar->EnableTool(m_actionIdInMenu, m_enable);
			}
		}
	}
}

//-----------------------------------------------------------------------------

void IAction::updating() throw(::fwTools::Failed)
{
	SLM_TRACE("IAction::updating");
    if (!m_isRadio)
    {
        setCheck(!m_isCheck);
    }
}

//-----------------------------------------------------------------------------

void IAction::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
	SLM_FATAL("an IAction service does not received a message." );
}

//-----------------------------------------------------------------------------

void IAction::setMenuName(std::string _menuName)
{
	m_menuName = _menuName ;
}

//-----------------------------------------------------------------------------

int IAction::getId()
{
	return m_actionIdInMenu ;
}

//-----------------------------------------------------------------------------

std::string IAction::getNameInMenu()
{
	return m_actionNameInMenu ;
}

//-----------------------------------------------------------------------------

std::string IAction::getMenuName()
{
	return m_menuName ;
}

//-----------------------------------------------------------------------------

wxMenuItem* IAction::getMenuItem()
{
	// get Frame
	wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
	SLM_ASSERT( "No wxFrame", frame ) ;

	// get MenuBar
	SLM_ASSERT( "No menu bar: MenuBar must be created by IAspect", frame->GetMenuBar() );
	wxMenuBar *menuBar =  frame->GetMenuBar();

	// get Menu
	SLM_ASSERT("Menu must exist", menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) != wxNOT_FOUND );
	wxMenu *menuFile = menuBar->GetMenu( menuBar->FindMenu( wxConvertMB2WX( m_menuName.c_str() ) ) ) ;

	return menuFile->FindItem( m_actionIdInMenu );
}

//-----------------------------------------------------------------------------

const ::gui::action::Shortcut::sptr IAction::getShortcut() const
{
	return m_shortcut;
}

//-----------------------------------------------------------------------------

::gui::action::Shortcut::sptr IAction::getShortcut()
{
	return m_shortcut;
}

//-----------------------------------------------------------------------------

void IAction::setShortcut( ::gui::action::Shortcut::sptr _shortcut )
{
	m_shortcut = _shortcut;
	//std::for_each( m_listeners.begin(), m_listeners.end(), NotifyPropertyChanged(shared_from_this(), SHORTCUT) );
}


}
}
