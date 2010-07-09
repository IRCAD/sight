/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_CLOSEACTION_HPP_
#define _GUI_ACTION_CLOSEACTION_HPP_


#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{

/**
 * @brief   This action tries to close the window and reset root object.
 * @class   CloseAction.
 * @author  IRCAD (Research and Development Team).

 * @date    2009-2010.
 */
class GUI_CLASS_API CloseAction : public ::fwGui::IActionSrv
{

public :

     fwCoreServiceClassDefinitionsMacro ( (CloseAction)(::fwGui::IActionSrv) ) ;

    /**
    * @brief Constructor. Do nothing.
    */
    GUI_API CloseAction() throw() ;

    /**
    * @brief Destructor. Do nothing.
    */
    GUI_API virtual ~CloseAction() throw() ;

protected:

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or stopped
     */
    GUI_API void configuring() throw( ::fwTools::Failed );

    GUI_API virtual void starting() throw(::fwTools::Failed);

    GUI_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief This method tries to close the window and reset root object.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /*
     * @brief This method gives information about the class.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

};


} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_CLOSEACTION_HPP_*/
