/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/colour.h>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Object.hpp>

#include "gui/sample/DummyTest.hpp"
#include "gui/action/IAction.hpp"
#include "gui/Manager.hpp"

namespace gui
{
namespace sample
{

/**
 * @brief	Dummy action
 * @author	IRCAD (Research and Development Team).
 */
class DummyAction : public ::gui::action::IAction
{

public :

	DummyAction(DummyTest *test , int _id ) throw() : m_client( test )
	{
		m_actionIdInMenu = _id ;
	}
	virtual ~DummyAction() throw() {};

	virtual void updating() throw(::fwTools::Failed)
	{
		m_client->changeColor() ;
	}
	virtual void info(std::ostream &_sstream )
	{
		_sstream << "Action for changing color" << std::endl;
	}

private :

	DummyTest	*m_client ;
};


REGISTER_SERVICE( ::gui::sample::ITestService , ::gui::sample::DummyTest , ::fwTools::Object ) ;

DummyTest::DummyTest() throw()
: m_wxContainerId( 0 ), m_color("RED")
{}

DummyTest::~DummyTest() throw()
{}

void DummyTest::configuring() throw( ::fwTools::Failed )
{
	OSLM_TRACE("configuring DummyTest") ;
	::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin() ;
	for( ; iter != this->m_configuration->end() ; ++iter )
	{
		if( (*iter)->getName() == "usedView" )
		{
			assert( (*iter)->hasAttribute("id")) ;
   			std::string id = (*iter)->getExistingAttributeValue("id") ;
   			m_wxContainerId = boost::lexical_cast< int >(id) ;
			OSLM_TRACE("Used view : " << id ) ;
		}
		if( (*iter)->getName() == "usedAction" )
		{
			assert( (*iter)->hasAttribute("id")) ;
   			std::string id = (*iter)->getExistingAttributeValue("id") ;
   			m_actionId = boost::lexical_cast< int >(id) ;
   			OSLM_TRACE("Used action : " << id ) ;
		}
		if( (*iter)->getName() == "color" )
		{
			std::string id = (*iter)->getValue() ;
			m_color = id ;
		}
	}
}
void DummyTest::info(std::ostream &_sstream )
{
	_sstream << "DummyTest" ;
}
void DummyTest::starting() throw(::fwTools::Failed)
{
	m_localAction = ::boost::shared_ptr< DummyAction >( new DummyAction( this , m_actionId ) ) ;
	m_localAction->start() ;
	assert( m_wxContainerId != 0 ) ;
	assert( wxWindow::FindWindowById( m_wxContainerId ) ) ;
	if( wxWindow::FindWindowById( m_wxContainerId ) )
	{
		if( m_color == "RED" )
		{
			wxWindow::FindWindowById( m_wxContainerId )->SetBackgroundColour( wxColour( 255 ,0 ,0 ) ) ;
		}
		else
		{
			wxWindow::FindWindowById( m_wxContainerId )->SetBackgroundColour( wxColour( 0 , 255 ,0 ) ) ;
		}
		wxWindow::FindWindowById( m_wxContainerId )->Refresh() ;
	}

}

void DummyTest::changeColor()
{
	assert( m_wxContainerId != 0 ) ;
	assert( wxWindow::FindWindowById( m_wxContainerId ) ) ;
	if( wxWindow::FindWindowById( m_wxContainerId ) )
	{
		if( m_color == "RED" )
		{
			if( wxWindow::FindWindowById( m_wxContainerId )->GetBackgroundColour() == wxColour( 255 ,0 ,0 ) )
			{
				wxWindow::FindWindowById( m_wxContainerId )->SetBackgroundColour( wxColour( 0 ,255 ,0 ) ) ;
			}
			else
			{
				wxWindow::FindWindowById( m_wxContainerId )->SetBackgroundColour( wxColour( 255 ,0 ,0 ) ) ;
			}
		}
		else
		{
			if( wxWindow::FindWindowById( m_wxContainerId )->GetBackgroundColour() == wxColour( 0 ,255 ,0 ) )
			{
				wxWindow::FindWindowById( m_wxContainerId )->SetBackgroundColour( wxColour( 0 , 0 , 255 ) ) ;
			}
			else
			{
				wxWindow::FindWindowById( m_wxContainerId )->SetBackgroundColour( wxColour( 0 , 255,0 ) ) ;
			}

		}
		wxWindow::FindWindowById( m_wxContainerId )->Refresh() ;
	}
}
void DummyTest::stopping() throw(::fwTools::Failed)
{}
void DummyTest::updating() throw( ::fwTools::Failed )
{}

}
}

