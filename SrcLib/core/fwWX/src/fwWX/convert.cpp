/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include "fwWX/convert.hpp"

namespace fwWX
{
wxString std2wx( const std::string & value )
{
	return wxString( value.c_str() , wxConvUTF8 );
}

std::string wx2std( const wxString & value )
{
	return (const char*)value.mb_str(wxConvUTF8);
}
}
