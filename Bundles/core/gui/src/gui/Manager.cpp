/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cassert>
#include <vector>

#include <wx/wx.h>
#include <wx/accel.h>
#include <wx/config.h>
#include <wx/evtloop.h>

#include <fwCore/base.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include <fwWX/convert.hpp>

#include "gui/Manager.hpp"
#include "gui/aspect/IAspect.hpp"
#include "gui/action/Shortcut.hpp"
#include "gui/utility/WindowLocation.hpp"

namespace gui
{

namespace
{
    const wxString  CONFIG_PATH         ( _("/Frame")   );  ///< the path in the user configuration for frtame settings
    const wxString  LOCATION_CONFIG_KEY ( _("Location") );  ///< the location configuration key
}

/**
 * @brief Forward menu event received to the manager in order to update/execute the appropriate service
 * @author IRCAD (Research and Development Team).
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
        ::gui::Manager::processAction( event.GetId() ) ;
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
     * @brief   constructor
     */
    MainFrame() throw(): wxFrame( 0, -1,   ::fwWX::std2wx( "gui::Frame" ) )
    {
        this->readConfig();
    }

    /**
     * @brief   destructor
     */
    ~MainFrame() throw() {}

    bool Destroy()
    {
        ::gui::Manager::uninitialize() ;
        this->writeConfig();
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
            const wxString location( config->Read( LOCATION_CONFIG_KEY, wxEmptyString ) );
            const ::gui::utility::WindowLocation windowLocation( location );
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

//-----------------------------------------------------------------------------

::gui::Manager::sptr Manager::m_instance;

//-----------------------------------------------------------------------------

const ::gui::Manager::sptr Manager::getDefault() throw()
{
    if(m_instance.get() == 0)
    {
        m_instance = ::gui::Manager::NewSptr();
    }
    return m_instance;
}

//-----------------------------------------------------------------------------

Manager::Manager() throw() : m_wxmanager( 0 )
{}

//-----------------------------------------------------------------------------

Manager::~Manager() throw()
{
    m_instance.reset() ;
}

//-----------------------------------------------------------------------------

void Manager::uninitialize()
{
    SLM_TRACE_FUNC();

    // Stops all services of the application root object before uninstalling wxContainer
    ::fwServices::OSR::uninitializeRootObject();

    // Uninstalling wxContainer
    if( ::gui::Manager::getDefault()->m_wxmanager )
    {
        ::gui::Manager::getDefault()->m_wxmanager->UnInit() ;
        delete ::gui::Manager::getDefault()->m_wxmanager;
        ::gui::Manager::getDefault()->m_wxmanager = 0 ;
    }
}

//-----------------------------------------------------------------------------

void Manager::initialize()
{
    SLM_TRACE_FUNC();

     wxEventLoopBase* eventLoop = wxEventLoop::GetActive();
     if (!eventLoop)
     {
         wxEventLoop::SetActive(new wxEventLoop() );
     }

    // wxWidget initialization
    wxInitAllImageHandlers();
    // Build top window
    wxFrame *frame = new MainFrame();
    wxTheApp->SetTopWindow( frame ) ;

    frame->CreateStatusBar();
    frame->PushEventHandler( new MenuHandler );
    frame->Refresh();
    frame->Layout();

    if( getDefault()->m_wxmanager == 0 )
    {
        getDefault()->m_wxmanager = new wxAuiManager( wxTheApp->GetTopWindow() ) ;
    }

    // Initialize root object : root object, views, ...
    ::fwServices::OSR::initializeRootObject();
    getDefault()->m_wxmanager->Update();
}

//-----------------------------------------------------------------------------

wxAuiManager * Manager::getTopAuiManager()
{
    SLM_ASSERT("Sorry, no wxManager", getDefault()->m_wxmanager );
    return getDefault()->m_wxmanager ;
}

//-----------------------------------------------------------------------------

void Manager::registerAction( ::gui::action::IAction::sptr _action)
{
    SLM_ASSERT("Sorry, Action expired", _action);
    int id = _action->getId();

    OSLM_ASSERT("Sorry, Action with id " <<id<< " is already registered!",
            getDefault()->m_serviceCallBacks.find(id) == getDefault()->m_serviceCallBacks.end() );

    // Associated action and id
    getDefault()->m_serviceCallBacks[id]= _action ;

    ::gui::action::Shortcut::csptr shortcut = _action->getShortcut();
    // It is possible that the current action has no shortcut.
    if( shortcut != 0 )
    {
        getDefault()->m_vAccelEntries.push_back(wxAcceleratorEntry( shortcut->getModifiers(), shortcut->getKeyCode(), id ) );
    }
    ::gui::Manager::registerShortcuts();
}

//-----------------------------------------------------------------------------

void Manager::unregisterAction( ::gui::action::IAction::sptr _action)
{
    SLM_ASSERT("Sorry, Action expired", _action);

    int id = _action->getId();
    OSLM_ASSERT("Sorry, Action with id " <<id<< " is already unregistered!",
            getDefault()->m_serviceCallBacks.find(id) != getDefault()->m_serviceCallBacks.end());

    getDefault()->m_serviceCallBacks.erase( id ) ;
}

//-----------------------------------------------------------------------------

void Manager::processAction(int id )
{
    SLM_ASSERT("Sorry, Action not found: "<< id ,
            getDefault()->m_serviceCallBacks.find(id) != getDefault()->m_serviceCallBacks.end());

    SLM_ASSERT("Sorry, Action expired",
            !getDefault()->m_serviceCallBacks[id].expired() ) ;

    SLM_ASSERT("Sorry, Action is disabled",
            getDefault()->m_serviceCallBacks[id].lock()->isEnable());

    getDefault()->m_serviceCallBacks[id].lock()->update() ;
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

