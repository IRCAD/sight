/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "uiMeasurement/config.hpp"

#include <fwCore/macros.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action adds distances on an image field.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="::uiMeasurement::action::SAddDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: Image containing the distance field.
 */
class UIMEASUREMENT_CLASS_API SAddDistance final : public ::fwGui::IActionSrv
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SAddDistance,  ::fwGui::IActionSrv)

    /// Creates the service.
    UIMEASUREMENT_API SAddDistance() noexcept;

    /// Destroys the service.
    UIMEASUREMENT_API ~SAddDistance() noexcept override;

protected:

    /// Configures the action.
    UIMEASUREMENT_API void configuring() override;

    /// Starts the action.
    UIMEASUREMENT_API void starting() override;

    /// Adds a new distance to the image field.
    UIMEASUREMENT_API void updating() override;

    /// Stops the action.
    UIMEASUREMENT_API void stopping() override;

};

} // namespace action.

} // namespace uiMeasurement.
