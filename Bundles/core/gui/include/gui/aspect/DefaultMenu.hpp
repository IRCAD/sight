/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_DEFAULT_MENU_HPP_
#define GUI_DEFAULT_MENU_HPP_

#include "gui/aspect/IMenu.hpp"
#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief	Defines the default menu for standard application
 * @class	DefaultMenu.
 * @author	IRCAD (Research and Development Team)

 * @date	2009.
 */
class GUI_CLASS_API DefaultMenu : public IMenu
{

public :

	/// Constructor. Do nothing.
	GUI_API DefaultMenu() throw();

	/// Destructor. Do nothing.
	GUI_API virtual ~DefaultMenu() throw();

};

}

}

#endif // GUI_DEFAULT_MENU_HPP_
