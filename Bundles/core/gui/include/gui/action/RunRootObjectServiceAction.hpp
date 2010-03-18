/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_RUNROOTOBJECTSERVICEACTION_HPP_
#define _GUI_ACTION_RUNROOTOBJECTSERVICEACTION_HPP_

#include <fwServices/IService.hpp>

#include "gui/export.hpp"
#include "gui/action/IAction.hpp"

namespace gui
{
namespace action
{

/**
 * @brief   This action starts an ::gui::editor::IEditor on the default root object.
 * @class   RunRootObjectServiceAction.
 * @author  IRCAD (Research and Development Team).

 * @date    2009.
 */
class GUI_CLASS_API RunRootObjectServiceAction : public ::gui::action::IAction
{

public :
    
    fwCoreServiceClassDefinitionsMacro ( (RunRootObjectServiceAction)(::gui::action::IAction::Baseclass) ) ;

    /**
    * @brief Constructor. Do nothing.
    */
    GUI_API RunRootObjectServiceAction() throw() ;

    /**
    * @brief Destructor. Do nothing.
    */
    GUI_API virtual ~RunRootObjectServiceAction() throw() ;

protected:

    /**
     * @brief This method is used to start, update and stop service ::gui::editor::IEditor on root object.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;
    
    
};


} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_RUNROOTOBJECTSERVICEACTION_HPP_*/
