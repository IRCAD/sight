///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#ifndef _GUI_ACTION_QUITACTION_HPP_
//#define _GUI_ACTION_QUITACTION_HPP_
//
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
// * @brief   This action tries to close the window and reset root object.
// * @class   QuitAction.
// * @author  IRCAD (Research and Development Team).
//
// * @date    2009.
// */
//class GUI_CLASS_API QuitAction : public ::gui::action::IAction
//{
//
//public :
//
//     fwCoreServiceClassDefinitionsMacro ( (QuitAction)(::gui::action::IAction) ) ;
//
//    /**
//    * @brief Constructor. Do nothing.
//    */
//    GUI_API QuitAction() throw() ;
//
//    /**
//    * @brief Destructor. Do nothing.
//    */
//    GUI_API virtual ~QuitAction() throw() ;
//
//protected:
//
//    /**
//     * @brief This method tries to close the window and reset root object.
//     */
//    GUI_API virtual void updating() throw(::fwTools::Failed);
//
//    /*
//     * @brief This method gives information about the class.
//     */
//    GUI_API virtual void info(std::ostream &_sstream ) ;
//
//};
//
//
//} // namespace action
//} // namespace gui
//
//
//#endif /*_GUI_ACTION_QUITACTION_HPP_*/
