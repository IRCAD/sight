/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/wxprec.h>

#include "gui/App.hpp"
#include "gui/Manager.hpp"

namespace gui
{

IMPLEMENT_APP_NO_MAIN(gui::App) ;

bool App::OnInit()
{
	// Initialize application manager
	Manager::initialize() ;

    return TRUE;
}

}
