/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_NOTHINGACTION_HPP_
#define _GUI_ACTION_NOTHINGACTION_HPP_


#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{

/**
 * @brief   This action does nothing.
 * @class   NothingActionSrv.
 * @author  IRCAD (Research and Development Team).

 * @date    2009.
 */
class GUI_CLASS_API NothingActionSrv : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (NothingActionSrv)(::fwGui::IActionSrv) ) ;

    /**
    * @brief Constructor. Do nothing.
    */
    GUI_API NothingActionSrv() throw() ;

    /**
    * @brief Destructor. Do nothing.
    */
    GUI_API virtual ~NothingActionSrv() throw() ;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the action.
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

    GUI_API virtual void starting() throw(::fwTools::Failed);

    GUI_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /**
     * @brief Process the action. Do nothing.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method gives information about the class.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

    ///@}

};


} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_NOTHINGACTION_HPP_*/
