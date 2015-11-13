/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_NOTHINGACTIONSRV_HPP__
#define __GUI_ACTION_NOTHINGACTIONSRV_HPP__


#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/config.hpp"

namespace gui
{
namespace action
{

/**
 * @brief   This action does nothing.
 * @class   NothingActionSrv
 */
class GUI_CLASS_API NothingActionSrv : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NothingActionSrv)(::fwGui::IActionSrv) );

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API NothingActionSrv() throw();

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~NothingActionSrv() throw();

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the action.
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed );

    GUI_API virtual void starting() throw(::fwTools::Failed);

    GUI_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Process the action. Do nothing.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method gives information about the class.
     */
    GUI_API virtual void info(std::ostream &_sstream );

    ///@}

};


} // namespace action
} // namespace gui


#endif /*__GUI_ACTION_NOTHINGACTIONSRV_HPP__*/
