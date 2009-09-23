/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_DEFAULT_TOOLBAR_HPP_
#define GUI_DEFAULT_TOOLBAR_HPP_

#include "gui/aspect/IToolBar.hpp"
#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief	Defines the default toolbar for standard application
 * @class	DefaultToolBar.
 * @author	IRCAD (Research and Development Team)
 * @author	Jean-Baptiste Fasquel.
 * @date	2009.
 */
class GUI_CLASS_API DefaultToolBar : public IToolBar
{

public :

	/// Constructor. Do nothing.
	GUI_API DefaultToolBar() throw();

	/// Destructor. Do nothing.
	GUI_API virtual ~DefaultToolBar() throw();

};


}

}

#endif // GUI_DEFAULT_TOOLBAR_HPP_
