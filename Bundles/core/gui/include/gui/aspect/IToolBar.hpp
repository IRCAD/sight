/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_ASPECT_ITOOLBAR_HPP_
#define GUI_ASPECT_ITOOLBAR_HPP_

#include <boost/shared_ptr.hpp>
#include <set>
#include <wx/toolbar.h>

#include <fwServices/IService.hpp>

#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief	Defines the default toolbar for standard application
 * @class	IToolBar.
 * @author	IRCAD (Research and Development Team)
 * @date	2009.
 */
class GUI_CLASS_API IToolBar : public ::fwServices::IService
{

public :

	fwCoreNonInstanciableClassDefinitionsMacro ( (IToolBar)( ::fwServices::IService::Baseclass) ) ;

	/**
	 * @brief Constructor. Do nothing (Just initialize parameters).
	 */
	GUI_API IToolBar() throw() ;

	/**
	 * @brief Destructor. Do nothing.
	 */
	GUI_API virtual ~IToolBar() throw() ;

protected :

	/** @name Service methods ( override from ::fwServices::IService )
	 * @{
     */

	   /**
	 * @brief This method is used to configure the class parameters. Do nothing.
	 */
	GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

	/**
	 * @brief This method create a new ToolBar in frame, and add tools with IToolBar::m_actionIds list.
	 */
	GUI_API virtual void starting() throw( ::fwTools::Failed ) ;

	/**
	 * @brief This method remove the tools situated in the ToolBar and delete ToolBar.
	 */
	GUI_API virtual void stopping() throw( ::fwTools::Failed ) ;

	/**
	 * @brief This method is used to update services on notification. Do nothing.
	 * @attention An IToolBar service does not received a message.
	 */
	GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) ;

	/**
	 * @brief This method is used to update services. Do nothing.
	 */
	GUI_API virtual void updating() throw(::fwTools::Failed) ;
	///@}

private:

	wxToolBar*	m_toolBar;
};

}
}

#endif /*GUI_ASPECT_ITOOLBAR_HPP_*/
