/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/config.h>
#include <wx/evtloop.h>

#include <fwWX/WindowLocation.hpp>

#include "fwGuiWx/wxMainFrame.hpp"


namespace fwGuiWx
{

//-----------------------------------------------------------------------------

const wxString  wxMainFrame::CONFIG_PATH         ( _("/Frame")   );
const wxString  wxMainFrame::LOCATION_CONFIG_KEY ( _("Location") );

//-----------------------------------------------------------------------------

wxMainFrame::wxMainFrame(wxWindow *parent, wxWindowID  id, const wxString &title, const wxPoint &pos,
        const wxSize &size, long style, const wxString &name) :
        wxFrame(parent, id, title, pos, size, style, name)
{
    SLM_TRACE_FUNC();
    this->readConfig();
}

//-----------------------------------------------------------------------------

wxMainFrame::~wxMainFrame()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

bool wxMainFrame::Destroy()
{
    this->writeConfig();
    return wxFrame::Destroy();
}

//-----------------------------------------------------------------------------

void wxMainFrame::readConfig()
{
    // Initializes the configuration.
    wxConfigBase* config = wxConfig::Get();
    config->SetPath( wxMainFrame::CONFIG_PATH );

    // Reads the location.
    try
    {
        const wxString location( config->Read( wxMainFrame::LOCATION_CONFIG_KEY, wxEmptyString ) );
        const ::fwWX::WindowLocation windowLocation( location );
        windowLocation.applyOn( this );
    }
    catch( ... )
    {
        Maximize();
    }
}

//-----------------------------------------------------------------------------

void wxMainFrame::writeConfig()
{
    // Initializes the configuration.
    wxConfigBase* config = wxConfig::Get();
    config->SetPath( wxMainFrame::CONFIG_PATH );

    // Writes the location.
    config->Write( wxMainFrame::LOCATION_CONFIG_KEY, ::fwWX::WindowLocation(this).toString() );
    config->Flush();
}

}//namespace fwGuiWx

