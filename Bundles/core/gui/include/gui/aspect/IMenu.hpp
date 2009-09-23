/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_IMENU_HPP_
#define GUI_IMENU_HPP_

#include <boost/shared_ptr.hpp>
#include <set>

#include <wx/menuitem.h>

#include <fwServices/IService.hpp>

#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief	Defines the service interface managing GUI aspect, including menu actions and views
 * @class	IMenu.
 * @author	IRCAD (Research and Development Team)
 * @author	Jean-Baptiste Fasquel.
 * @date	2009.
 * @todo 	ACH : remove m_aspect from this class ( refactoring IAspect too )
 * @todo	ACH : imposed the definition of starting, updating, etc.... methods to children classes
 */
class GUI_CLASS_API IMenu : public ::fwServices::IService
{

public :

	fwCoreNonInstanciableClassDefinitionsMacro ( (IMenu)( ::fwServices::IService::Baseclass ) ) ;

	/// Default constructor, do nothing.
	GUI_API IMenu() throw() ;

	/// Default destructor, do nothing.
	GUI_API virtual ~IMenu() throw() ;

	/// Retrieve IMenu::m_menuName in configuration element
	GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

	GUI_API virtual void starting() throw( ::fwTools::Failed ) ;

	/**
	 * @brief Stop all actions and separator of this menu and remove menu to wxMenuBar
	 * @todo ACH : uncomment assert => all actions must be started
	 * @todo Menu not empty on MAC with specials Actions like help, Quit...
	 */
	GUI_API virtual void stopping() throw( ::fwTools::Failed ) ;

	/// Updating service, do nothing.
	GUI_API virtual void updating() throw(::fwTools::Failed);

	/// Updating service on notification, do nothing.
	GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

protected :
	const static std::string SEPARATOR_UUUID;

	std::vector< std::string >		m_actionsUUID ;
	std::set< wxMenuItem * > 		m_separators ;
	std::string 					m_menuName ;
};


}
}

#endif /*IMENU_HPP_*/
