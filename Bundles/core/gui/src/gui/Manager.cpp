/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <cassert>
#include <vector>

#include <wx/wx.h>
#include <wx/accel.h>
#include <wx/statusbr.h>
#include <wx/config.h>

#include <fwCore/base.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "gui/Manager.hpp"
#include "gui/aspect/IAspect.hpp"
#include "gui/action/Shortcut.hpp"
#include "gui/utility/WindowLocation.hpp"

namespace gui
{

namespace
{

	const wxString	CONFIG_PATH			( _("/Frame")	);	///< the path in the user configuration for frtame settings
	const wxString	LOCATION_CONFIG_KEY	( _("Location")	);	///< the location configuration key

}

/**
 * @brief Forward menu event received to the manager in order to update/execute the appropriate service
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
class MenuHandler : public wxEvtHandler
{
public:
	/// Constructor
	MenuHandler() throw() {}

private:
	/// Declares the wxWidgets event table
	DECLARE_EVENT_TABLE()

	/// Handles the click on a menu element.
	void onSelect(wxCommandEvent & event)
	{
		OSLM_TRACE("Menu pressed : " << event.GetId() );
		Manager::processAction( event.GetId() ) ;
	}
};

BEGIN_EVENT_TABLE(MenuHandler, wxEvtHandler)
	EVT_MENU(-1, MenuHandler::onSelect)
END_EVENT_TABLE()

/**
 * @brief Application top window
 */
class MainFrame : public wxFrame
{
public:

	/**
	 * @brief	constructor
	 */
	MainFrame() throw():	wxFrame( 0, -1,  wxConvertMB2WX( "gui::Frame" ) )
	{
		readConfig();
	}

	/**
	 * @brief	destructor
	 */
	~MainFrame() throw() {}

	bool Destroy()
	{
		Manager::uninitialize() ;
		writeConfig();
		return wxFrame::Destroy();
	}

private:
	void readConfig()
	{
		// Initializes the configuration.
		wxConfigBase* config = wxConfig::Get();

		config->SetPath( CONFIG_PATH );

		// Reads the location.
		try
		{
			const wxString location( config->Read( LOCATION_CONFIG_KEY, wxEmptyString )	);
			const utility::WindowLocation windowLocation( location );
			windowLocation.applyOn( this );
		}
		catch( ... )
		{
			Maximize();
		}
	}


	void writeConfig()
	{
		// Initializes the configuration.
		wxConfigBase* config = wxConfig::Get();

		config->SetPath( CONFIG_PATH );

		// Writes the location.
		config->Write( LOCATION_CONFIG_KEY, utility::WindowLocation(this).toString() );
		config->Flush();
	}
};


::boost::shared_ptr< Manager > Manager::m_instance;

const ::boost::shared_ptr< Manager > Manager::getDefault() throw()
{
	if(m_instance.get() == 0)
	{
		m_instance = ::boost::shared_ptr< Manager >(new Manager());
	}
	return m_instance;
}

//-----------------------------------------------------------------------------

Manager::Manager() throw()
: m_wxmanager( 0 ), m_globalAspectInformation(false , "")
{
}

//-----------------------------------------------------------------------------

Manager::~Manager() throw()
{
	m_instance.reset() ;
}

//-----------------------------------------------------------------------------

void Manager::uninitialize()
{
	SLM_INFO("Uninitialize GUI aspect") ;
	// Stops all services of the application root object before uninstalling wxContainer
	::fwServices::OSR::uninitializeRootObject();
	// Release global aspect elements
	if( getDefault()->m_globalAspectInformation.first )
	{
	  getDefault()->m_unlessObject.reset();
	}
	// Uninstalling wxContainer
	if( getDefault()->m_wxmanager )
	{
		getDefault()->m_wxmanager->UnInit() ;
		delete getDefault()->m_wxmanager;
		getDefault()->m_wxmanager = 0 ;
	}
}

//-----------------------------------------------------------------------------

void Manager::initialize()
{
	SLM_INFO("Initialize GUI aspect") ;
	// wxWidget initialization
	wxInitAllImageHandlers();
	// Build top window
	wxFrame *frame = new MainFrame();
    wxTheApp->SetTopWindow( frame ) ;

//    frame->Show(TRUE);
    frame->CreateStatusBar();
    frame->PushEventHandler( new MenuHandler );
    frame->Refresh();
    frame->Layout();

	if( getDefault()->m_wxmanager == 0 )
	{
		getDefault()->m_wxmanager = new wxAuiManager( wxTheApp->GetTopWindow() ) ;
	}
    // Initialize global aspect : independent from root object considered
    if( getDefault()->m_globalAspectInformation.first )
    {
    	// Initialize general aspect
    	SLM_INFO("Initializing general GUI aspect") ;
    	::boost::shared_ptr< ::fwRuntime::Extension > ext = ::fwRuntime::findExtension(getDefault()->m_globalAspectInformation.second) ;
       	assert( ext );
       	assert( ext->getPoint() == "::gui::aspect::DefaultAspect");
       	ext->getBundle()->start(); // start dll to retrieve proxy
       	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > cfg = *(ext->begin()) ;
       	getDefault()->m_unlessObject = ::fwTools::Object::NewSptr() ;
       	getDefault()->m_globalAspect = ::fwServices::add< ::gui::aspect::IAspect >( getDefault()->m_unlessObject , "::gui::aspect::DefaultAspect" ) ;

       	::fwServices::get< ::gui::aspect::IAspect >( getDefault()->m_unlessObject )->setConfiguration( cfg ) ;
       	::fwServices::get< ::gui::aspect::IAspect >( getDefault()->m_unlessObject )->configure() ;
       	::fwServices::get< ::gui::aspect::IAspect >( getDefault()->m_unlessObject )->start();
    }
    else
    {
    	SLM_INFO("No general GUI aspect required") ;
    }

    // Initialize root object : root object, views, ...
    ::fwServices::OSR::initializeRootObject();
    getDefault()->m_wxmanager->Update();
}

//-----------------------------------------------------------------------------

wxAuiManager * Manager::getTopAuiManager()
{
	assert( getDefault()->m_wxmanager );
	return getDefault()->m_wxmanager ;
}

//-----------------------------------------------------------------------------

void Manager::setGlobalAspectInformations( std::pair< bool , std::string > globalAspectInfo )
{
	getDefault()->m_globalAspectInformation = globalAspectInfo ;
}

//-----------------------------------------------------------------------------

void Manager::registerAspect( ::gui::aspect::IAspect::wptr _aspect )
{
	wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
	frame->SetTitle(wxConvertMB2WX( _aspect.lock()->getName().c_str() )) ;
	if(_aspect.lock()->getIcon()!= 0)
	{
		frame->SetIcon( *_aspect.lock()->getIcon().get() );
	}
    frame->SetMinSize(_aspect.lock()->getMinSize());
    frame->Show(TRUE);
}

//-----------------------------------------------------------------------------

void Manager::unregisterAspect( ::gui::aspect::IAspect::wptr _aspect )
{
	if( getDefault()->m_globalAspectInformation.first && getDefault()->m_globalAspect.lock() != _aspect.lock() )
	{
		wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
		frame->SetTitle(wxConvertMB2WX( ::fwServices::get< ::gui::aspect::IAspect >( getDefault()->m_unlessObject )->getName().c_str() )) ;
	}
	if( getDefault()->m_wxmanager )
	{
		getDefault()->m_wxmanager->Update();
		wxTheApp->GetTopWindow()->Refresh();
	}
}

//-----------------------------------------------------------------------------

void Manager::registerAction( ::gui::action::IAction::wptr _action)
{
	assert(_action.use_count());
	int id = _action.lock()->getId();
	// If gui menu entry to action not yet registered : install it
	if( getDefault()->m_serviceCallBacks.find(id) != getDefault()->m_serviceCallBacks.end() )
	{
		//getDefault()->m_serviceCallBacks[id].lock()->stop();
		OSLM_TRACE("Replacing menu " << id );
	}
	// Associated action and id
	getDefault()->m_serviceCallBacks[id]= _action ;

	::boost::shared_ptr< ::gui::action::Shortcut >	shortcut = _action.lock()->getShortcut();
	// It is possible that the current action has no shortcut.
	if( shortcut != 0 )
	{
		getDefault()->m_vAccelEntries.push_back(wxAcceleratorEntry( shortcut->getModifiers(), shortcut->getKeyCode(), _action.lock()->getId() ) );
	}
	registerShortcuts();

	// action can observe its object by default updating on object is a NOP function
//	::fwServices::registerCommunicationChannel(_action.lock()->getObject(),  _action.lock() )->start();


}

//-----------------------------------------------------------------------------

void Manager::unregisterAction( ::gui::action::IAction::wptr _action)
{
	assert(_action.use_count());
	int id = _action.lock()->getId();
	if( getDefault()->m_serviceCallBacks.find(id) != getDefault()->m_serviceCallBacks.end() )
	{
//		::fwServices::unregisterCommunicationChannel(  _action.lock()->getObject(), _action.lock() );
		getDefault()->m_serviceCallBacks.erase( id ) ;
	}


}

//-----------------------------------------------------------------------------

::gui::action::IAction::wptr Manager::getAction( int id )
{
	assert( getDefault()->m_serviceCallBacks.find(id) != getDefault()->m_serviceCallBacks.end() );
	return getDefault()->m_serviceCallBacks[id] ;
}

//-----------------------------------------------------------------------------

bool Manager::isRegistered( int id )
{
	return !(getDefault()->m_serviceCallBacks.find(id) == getDefault()->m_serviceCallBacks.end()) ;
}

//-----------------------------------------------------------------------------

void Manager::processAction(int id )
{
	if(getDefault()->m_serviceCallBacks.find(id) == getDefault()->m_serviceCallBacks.end())
	{
		OSLM_WARN("ServiceCallBack not found: "<< id);
	}
	else
	{
		//assert( getDefault()->m_serviceCallBacks.find(id) 	!= getDefault()->m_serviceCallBacks.end() );
		assert( !getDefault()->m_serviceCallBacks[id].expired() ) ;

		assert(getDefault()->m_serviceCallBacks[id].lock()->isEnable());

		getDefault()->m_serviceCallBacks[id].lock()->update() ;
	}
}

//-----------------------------------------------------------------------------

void Manager::registerShortcuts()
{
	// Installs the new accelerator table into the managed window.
	wxAcceleratorEntry* accelEntries = new wxAcceleratorEntry[ getDefault()->m_vAccelEntries.size() ];
	for(size_t i = 0; i < getDefault()->m_vAccelEntries.size() ; i++ )
	{
		accelEntries[i] = getDefault()->m_vAccelEntries[i];
	}
	wxTheApp->GetTopWindow()->SetAcceleratorTable( wxAcceleratorTable((int) getDefault()->m_vAccelEntries.size(), accelEntries) );
	delete accelEntries;
}

} // namespace gui

