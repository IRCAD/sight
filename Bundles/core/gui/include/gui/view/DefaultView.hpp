/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_VIEW_DEFAULT_VIEW_HPP_
#define _GUI_VIEW_DEFAULT_VIEW_HPP_

#include <fwTools/Failed.hpp>

#include "gui/export.hpp"
#include "gui/view/IView.hpp"

namespace gui
{
namespace view
{

/**
 * @brief	Defines the default panel for standard application.
 * @class	DefaultView.
 * @author	IRCAD (Research and Development Team)

 * @date	2009.
 */
class GUI_CLASS_API DefaultView : public ::gui::view::IView
{

public :

	/// Constructor. Do nothing.
	GUI_API DefaultView() throw() ;
	
	/// Destructor. Do nothing.
	GUI_API virtual ~DefaultView() throw() ;

};


} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_DEFAULT_VIEW_HPP_*/
