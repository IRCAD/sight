/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/timer.h>

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Object.hpp>

#include "gui/action/TestAction.hpp"

namespace gui
{
namespace action
{

REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::TestAction , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

class Next : public wxEvtHandler
{
public:
	/// Constructor
	Next( ::fwServices::IService::sptr client) throw() : m_client(client)
	{
	}
	
private:
	/// Declares the wxWidgets event table
	DECLARE_EVENT_TABLE()

	/// Handles the click on a menu element.
	void onKeyPressed(wxCommandEvent & event)
	{
		m_client->update();
	}
	::fwServices::IService::sptr m_client ;
};

BEGIN_EVENT_TABLE(Next, wxEvtHandler)
	EVT_TEXT_ENTER(-1, Next::onKeyPressed)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------

/**
 * @brief use wx based timing to trigger service update
 * @author IRCAD (Research and Development Team).
 */
class Timer : public wxEvtHandler
{
public :
	/**
	 * @brief	constructor
	 */
	Timer() throw()
	{
		m_timer = new wxTimer( this );
	}
	
	/// Dtor
	~Timer() throw()
	{
		m_timer->Stop();
		delete m_timer ;
	}
	
	void setRate( int _rate )
	{
		m_rate = _rate ;
	}
	// Start clocking
	void start()
	{
		assert( m_rate ) ;
		m_current = m_testSequence.begin();
		m_timer->Start( m_rate ) ;
	}
	void setTestSequence(std::map< std::string , std::string > test)
	{
		m_testSequence = test ;
		m_current = m_testSequence.begin();
	}
	// Stop clocking
	void stop()
	{
		m_timer->Stop() ;
		m_current = m_testSequence.begin();
	}
	void OnTimer(wxTimerEvent& event)
	{
		if( m_testSequence.empty() )
		{
			return ;
		}
		if( m_current == m_testSequence.end() )
		{
			m_current = m_testSequence.begin();
		}
		std::string configId 	= m_current->first ;
		std::string objectType 	= m_current->second ;
		OSLM_INFO("Run : " << configId << " for type " << objectType ) ;
		::fwServices::OSR::uninitializeRootObject();
		::fwServices::OSR::setRootObjectClassName(objectType) ;
		::fwServices::OSR::setRootObjectConfigurationName(configId) ;
		::fwServices::OSR::initializeRootObject();
		++m_current ;

	}

private:

	/*
	 * @brief	declares the wxWidgets event table
	 */
	DECLARE_EVENT_TABLE()
	
	wxTimer	* 				m_timer;
	int						m_rate ;
	
	std::map< std::string , std::string > m_testSequence ;
	std::map< std::string , std::string >::iterator m_current;
};


BEGIN_EVENT_TABLE(Timer, wxEvtHandler)
	EVT_TIMER(-1, Timer::OnTimer)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------

TestAction::TestAction() throw() :
m_timer( ::boost::shared_ptr< Timer >( new Timer ) ) , m_isStarted( false ), m_isAutomatic( false )
{
}

//-----------------------------------------------------------------------------

TestAction::~TestAction() throw() 
{
}

//-----------------------------------------------------------------------------

void TestAction::configuring() throw( ::fwTools::Failed )
{
	this->::gui::action::IAction::configuring();
	::fwRuntime::ConfigurationElementContainer::Iterator iter ;
   	for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
   	{
   		if( (*iter)->getName() == "test" )
   		{
   			assert( (*iter)->hasAttribute("id") && (*iter)->hasAttribute("type") ) ;
   			m_testSequence[(*iter)->getExistingAttributeValue("id")] = (*iter)->getExistingAttributeValue("type") ;
   		}
   		if( (*iter)->getName() == "automatic" )
   		{
   			m_isAutomatic = true ;
   			std::string id = (*iter)->getValue() ;
   			m_rate = boost::lexical_cast< int >( id ) ;	
   		}
   	}
   	m_current = m_testSequence.begin() ;
}

//-----------------------------------------------------------------------------

void TestAction::starting() throw( ::fwTools::Failed )
{
	this->::gui::action::IAction::starting();
	if( !m_isAutomatic )
	{
		wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
		m_next = ::boost::shared_ptr< Next >( new Next( ::boost::dynamic_pointer_cast< ::fwServices::IService >( shared_from_this() ) ));
		frame->PushEventHandler( m_next.get() );
	}
}

//-----------------------------------------------------------------------------

void TestAction::stopping() throw( ::fwTools::Failed )
{
	this->::gui::action::IAction::stopping();
	if( !m_isAutomatic )
	{
		m_next->Disconnect();
	}
}

//-----------------------------------------------------------------------------

void TestAction::info(std::ostream &_sstream )
{
	_sstream << "Test Action" << std::endl ;
}

//-----------------------------------------------------------------------------

void TestAction::updating() throw( ::fwTools::Failed )
{
	if( m_testSequence.empty() )
	{
		return ;
	}
	// case automatic
	if( m_isAutomatic )
	{
		if( !m_isStarted )
		{
			m_timer->setTestSequence(m_testSequence) ;
			m_timer->setRate(m_rate);	
			m_timer->start();
			m_isStarted = true ;
		}
		else
		{
			m_timer->stop();
			m_isStarted = false ;
		}
	}
	else
	{
		if( m_current == m_testSequence.end() )
		{
			m_current = m_testSequence.begin();
		}
		std::string configId 	= m_current->first ;
		std::string objectType 	= m_current->second ;
		OSLM_INFO("Run : " << configId << " for type " << objectType ) ;
		::fwServices::OSR::uninitializeRootObject();
		::fwServices::OSR::setRootObjectClassName(objectType) ;
		::fwServices::OSR::setRootObjectConfigurationName(configId) ;
		::fwServices::OSR::initializeRootObject();
		++m_current ;
	}
}
}
}
