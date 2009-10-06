/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ASPECT_DEFAULTASPECT_HPP_
#define _GUI_ASPECT_DEFAULTASPECT_HPP_

#include <vector>
#include <wx/aui/aui.h>
#include <wx/menuitem.h>
#include <fwServices/IService.hpp>

#include "gui/export.hpp"
#include "gui/aspect/IAspect.hpp"
#include "gui/action/IAction.hpp"
#include "gui/view/IView.hpp"

namespace gui
{
namespace aspect
{


/**
 * @brief	Defines the default aspect for standard application
 * @class	DefaultAspect.
 * @author	IRCAD (Research and Development Team)

 * @date	2009.
 */
class GUI_CLASS_API DefaultAspect : public ::gui::aspect::IAspect
{

public :
	
	/// Constructor. Do nothing.
	GUI_API DefaultAspect() throw() ;

	/// Destructor. Do nothing.
	GUI_API virtual ~DefaultAspect() throw() ;

protected:

	/**
	 * @brief	This method initializes class member parameters from configuration elements.
	 *
	 */
	GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

	/**
	 * @brief This method first registers the aspect gui manager.
	 * It secondly starts the menus contained in the IAspect::m_menus
	 * and finally all IView services contained in the IAspect::m_views
	 */
	GUI_API virtual void starting() throw(::fwTools::Failed);

	/**
	 * @brief This method first unregisters the aspect gui manager.
	 * It secondly stops the menus contained in the IAspect::m_menus
	 * and finally all IView services contained in the IAspect::m_views
	 */
	GUI_API virtual void stopping() throw(::fwTools::Failed);

	/// This method gives information about the class. Do nothing.
	GUI_API virtual void info(std::ostream &_sstream ) ;

    /**
     * @brief   Normally, perform some computations according to object (this service is attached to) attribute values and its internal state.
     *          Do nothing here.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed );

    /**
     * @brief   Normally, perform some computations according to object (this service is attached to) attribute values and its internal state.
     *          Do nothing here.
     */
    GUI_API virtual void updating() throw( ::fwTools::Failed );
};


} // namespace aspect
} // namespace gui


#endif /* _GUI_ASPECT_DEFAULTASPECT_HPP_ */
