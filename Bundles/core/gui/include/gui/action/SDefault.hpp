/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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

    fwCoreServiceMacro(SDefault, ::fwGui::IActionSrv);

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
