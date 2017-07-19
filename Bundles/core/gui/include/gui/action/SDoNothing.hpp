/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SDONOTHING_HPP__
#define __GUI_ACTION_SDONOTHING_HPP__

#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

namespace gui
{
namespace action
{

/**
 * @brief   This action does nothing.
 */
class GUI_CLASS_API SDoNothing : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SDoNothing)(::fwGui::IActionSrv) );

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SDoNothing() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SDoNothing() noexcept;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the action.
     */
    GUI_API virtual void configuring();

    GUI_API virtual void starting();

    GUI_API virtual void stopping();

    /**
     * @brief Process the action. Do nothing.
     */
    GUI_API virtual void updating();

    /**
     * @brief This method gives information about the class.
     */
    GUI_API virtual void info(std::ostream &_sstream );

    ///@}

};


} // namespace action
} // namespace gui


#endif /*__GUI_ACTION_SDONOTHING_HPP__*/
