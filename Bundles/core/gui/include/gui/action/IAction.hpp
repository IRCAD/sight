/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_IACTION_HPP_
#define _GUI_ACTION_IACTION_HPP_

#include <fwServices/IService.hpp>
#include <wx/menuitem.h>

#include "gui/action/Shortcut.hpp"
#include "gui/export.hpp"

namespace gui
{

/**
 * @namespace ::gui::action
 * @brief This namespace contains the classes which define an action
 */
namespace action
{

/**
 * @brief	Defines the service interface managing action placed in IAction menus.
 * @class	IAction.
 * @author	IRCAD (Research and Development Team).

 * @date	2009.
 *
 */
class GUI_CLASS_API IAction : public ::fwServices::IService
{

public :

	fwCoreNonInstanciableClassDefinitionsMacro ( (IAction)( ::fwServices::IService::Baseclass) ) ;

	/**
	* @brief Constructor. Do nothing (Just initialize parameters).
	*
	* By default, the m_shortcutDef is not defined.
	*/
	GUI_API IAction() throw() ;

	/*
	 * @brief Destructor. Do nothing.
	 */
	GUI_API virtual ~IAction() throw() ;

	/**
	 * @brief This method is used to change the menu name (IAction::m_menuName).
	 *
	 * @param[in] _menuName Name of the new menu.
	 */
	GUI_API void setMenuName(std::string _menuName) ;

	/**
	 * @brief This method is used to get the id of a wxMenuItem that represents this action.
	 *
	 * @return WxMenuItem id.
	 */
	GUI_API int getId() ;

	/**
	 * @brief This method is used to get the name of a wxMenuItem.
	 *
	 * @return WxMenuItem name.
	 */
	GUI_API std::string getNameInMenu() ;

	/**
	 * @brief This method is used to get the name of a wxMenu.
	 *
	 * @return WxMenu name.
	 */
	GUI_API std::string getMenuName() ;

	/**
	 * @brief This method is used to get the wxMenuItem.
	 *
	 * @return WxMenuItem corresponding to this action.
	 */
	GUI_API wxMenuItem* getMenuItem() ;

	/**
	 * @brief	Retrieves the constant shortcut for the action.
	 *
	 * @return	Constant shared pointer to the shortcut.
	 */
	GUI_API const ::boost::shared_ptr< ::gui::action::Shortcut > getShortcut() const;


	/**
	 * @brief	Retrieves the shortcut for the action.
	 *
	 * @return	Shared pointer to the shortcut.
	 */
	GUI_API ::boost::shared_ptr< ::gui::action::Shortcut > getShortcut();


	/**
	 * @brief	Installs a new shortcut to the action.
	 *
	 * @param[in]	_shortcut	Shared pointer to the new shortcut.
	 */
	GUI_API void setShortcut( ::gui::action::Shortcut::sptr _shortcut );

	/**
	* @brief	Check if the action is activated or not.
	*
	* @return	true if the action is activated.
	*/
	GUI_API bool isEnable();

	/**
	 * @brief	Activate or not this action. unable action, unable the wxMenuItem also.
	 *
	 * @param[in] _enable true if the action is activated.
	 */
	GUI_API void setEnable(bool _enable);

	/**
	 * @brief	Check or uncheck the wxMenuItem when it is possible.
	 *
	 * @param[in] _check true in order to check wxMenuItem.
	 */
	GUI_API void setCheck(bool _check);

protected :

	/** @name Service methods ( override from ::fwServices::IService )
	 * @{
	 */

	/**
	 * @brief This method is used to configure the class parameters.
	 * @todo Remove assert on depreciated tag
	 */
	GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

	/*
	 * @brief Add a new item in wxMenuBar, and register this action in ::gui::Manager.
	 */
	GUI_API virtual void starting() throw(::fwTools::Failed);

	/*
	 * @brief This method remove the item situated in the menu and unregister this action in ::gui::Manager.
	 */
	GUI_API virtual void stopping() throw(::fwTools::Failed);

	/**
	 * @brief This method is used to update services on notification. Do nothing.
	 * @attention An IAction service does not received a message.
	 * @todo ACH: May this method be removed and imposed to children classes ??
	 * @tode ACH : uncomment assertion
	 */
	GUI_API virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

	/**
	 * @brief This method is used to update services (it manages the check / uncheck).
	 *
	 * @todo ACH: It is really used and called by its children classes.
	 */
	GUI_API virtual void updating() throw(::fwTools::Failed);

	/**
	 * @brief This method gives information about the class. Do nothing.
	 */
	GUI_API virtual void info(std::ostream &_sstream ) ;

	///@}

	static const std::map<std::string, int> SPECIAL_ACTION_TO_WXID;

	/// To know if the action is checkable.
	bool m_isCheckable;

	/// To know if the action is a radio button.
	bool m_isRadio;

	/// To know if the action is check or uncheck when action is checkable.
	bool m_isCheck;

	/// To know if the action is disable or unable.
	bool m_enable;

	/// Contains the definition of the shortcut.  Example "F1".
	std::string m_shortcutDef;

	/// a shared pointer to the shortcut or null when none.
	::gui::action::Shortcut::sptr	m_shortcut;

	/// Id of the wxMenuItem.
	int m_actionIdInMenu ;

	/// Name of the wxMenuItem.
	std::string m_actionNameInMenu ;

	/// Name of the wxMenu.
	std::string m_menuName ;

};



} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_IACTION_HPP_*/
