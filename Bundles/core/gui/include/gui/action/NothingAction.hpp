/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_NOTHINGACTION_HPP_
#define _GUI_ACTION_NOTHINGACTION_HPP_


#include <fwServices/IService.hpp>

#include "gui/export.hpp"
#include "gui/action/IAction.hpp"

namespace gui
{
namespace action
{

/**
 * @brief	This action does nothing.
 * @class	NothingAction.
 * @author	IRCAD (Research and Development Team).

 * @date	2009.
 */
class GUI_CLASS_API NothingAction : public ::gui::action::IAction
{

public :
	
	/**
	* @brief Constructor. Do nothing.
	*/
	GUI_API NothingAction() throw() ;

	/**
	* @brief Destructor. Do nothing.
	*/
	GUI_API virtual ~NothingAction() throw() ;

protected:

	/**
	 * @brief This method is used to update services on notification. Do nothing.
	 */
	GUI_API virtual void updating() throw(::fwTools::Failed);

	/**
	 * @brief This method gives information about the class. Do nothing.
	 */
	GUI_API virtual void info(std::ostream &_sstream ) ;
	
};


} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_NOTHINGACTION_HPP_*/
