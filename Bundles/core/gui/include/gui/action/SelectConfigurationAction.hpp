///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#ifndef _GUI_ACTION_SELECTCONFIGURATIONACTION_HPP_
//#define _GUI_ACTION_SELECTCONFIGURATIONACTION_HPP_
//
//#include <fwServices/IService.hpp>
//
//#include "gui/export.hpp"
//#include "gui/action/IAction.hpp"
//
//namespace gui
//{
//namespace action
//{
//
///**
// * @brief   This action un-initializes root Object,
// * sets the new selected object in root,
// * sets  the new selected configuration
// * and re-initialize root Object.
// *
// * @class   SelectConfigurationAction.
// * @author  IRCAD (Research and Development Team).
//
// * @date    2009.
// */
//class GUI_CLASS_API SelectConfigurationAction : public ::gui::action::IAction
//{
//
//public :
//
//    fwCoreServiceClassDefinitionsMacro ( (SelectConfigurationAction)(::gui::action::IAction) ) ;
//
//    /**
//    * @brief Constructor. Do nothing.
//    */
//    GUI_API SelectConfigurationAction() throw() ;
//
//    /**
//    * @brief Destructor. Do nothing.
//    */
//    GUI_API virtual ~SelectConfigurationAction() throw() ;
//
//protected:
//
//    /**
//     * @brief This method :
//     * - uninitialize root Object
//     * - show a root Object selector
//     * - set root Object
//     * - show a configuration Object selector
//     * - set root Object configuration
//     * - initialize root Object
//     */
//    GUI_API virtual void updating() throw(::fwTools::Failed);
//
//    /**
//     * @brief This method gives information about the class. Do nothing.
//     */
//    GUI_API virtual void info(std::ostream &_sstream ) ;
//
//
//};
//
//
//} // namespace action
//} // namespace gui
//
//
//#endif /*_GUI_ACTION_SELECTCONFIGURATIONACTION_HPP_*/
