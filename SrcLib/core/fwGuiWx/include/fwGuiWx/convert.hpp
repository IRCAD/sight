/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _FWGUIWX_CONVERT_HPP_
#define _FWGUIWX_CONVERT_HPP_

#include <string>

#ifdef __WXMSW__
#   include <wx/msw/winundef.h>
#endif
#include <wx/string.h>

#include "fwGuiWx/config.hpp"

#ifdef __WXMAC__
#include <wx/event.h>
wxDECLARE_EVENT(wxEventFwOpen, wxCommandEvent);
#define wxIDEventFwOpen 27001
#endif

namespace fwGuiWx
{
    FWGUIWX_API wxString std2wx( const std::string & value );
    FWGUIWX_API std::string wx2std( const wxString & value );
}
#endif
