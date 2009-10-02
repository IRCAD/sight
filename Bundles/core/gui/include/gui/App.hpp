/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_APP_HPP_
#define GUI_APP_HPP_

#include <wx/app.h>
#include <wx/intl.h>
#include "gui/export.hpp"

namespace gui
{

/**
 * @brief	Defines the wx application.
 * @class	App.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 */

GUI_CLASS_API struct App : public wxApp
{
	///A wxWidgets application does not have a main procedure; the equivalent is the OnInit member defined for a class derived from wxApp.
	bool OnInit();

};


} // namespace gui


#endif /*GUI_APP_HPP_*/
