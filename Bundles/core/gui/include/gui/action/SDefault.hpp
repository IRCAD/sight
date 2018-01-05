/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SDEFAULT_HPP__
#define __GUI_ACTION_SDEFAULT_HPP__

#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

namespace gui
{
namespace action
{

/**
 * @brief  Defines the default action to have a simple action button.
 */
class GUI_CLASS_API SDefault : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SDefault)(::fwGui::IActionSrv) );

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SDefault() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SDefault() noexcept;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    GUI_API virtual void configuring() override;

    GUI_API virtual void starting() override;

    GUI_API virtual void stopping() override;

    GUI_API virtual void updating() override;

    GUI_API virtual void info(std::ostream& _sstream ) override;

    ///@}

};

} // namespace action
} // namespace gui

#endif /*__GUI_ACTION_SDEFAULT_HPP__*/
