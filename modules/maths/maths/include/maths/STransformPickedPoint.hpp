/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "maths/config.hpp"

#include <fwDataTools/PickingInfo.hpp>

#include <fwServices/IService.hpp>

namespace maths
{

/**
 * @brief Applies a transformation matrix on a picked point.
 *
 * @section Signals Signals
 * - \b pickedPointTransformed(::fwDataTools::PickingInfo): Emitted when a picked point is transformed
 *
 * @section Slots Slots
 * - \b pickedPoint(::fwDataTools::PickingInfo): Transform picked point
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::maths::STransformPickedPoint">
            <in key="matrix" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D]: input matrix
 */
class MATHS_CLASS_API STransformPickedPoint final : public ::fwServices::IService
{

public:

    /// Generates default methods like New, dynamicCast, ...
    fwCoreServiceMacro(STransformPickedPoint, ::fwServices::IService)

    /// Initializes signals and slots.
    MATHS_API STransformPickedPoint() noexcept;

    /// Destroys the service.
    MATHS_API ~STransformPickedPoint() noexcept override;

protected:

    /// Does nothing.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Does nothing.
    MATHS_API void updating() override;

    /// Does nothing.
    MATHS_API void configuring() override;

private:

    typedef ::fwCom::Signal< void (::fwDataTools::PickingInfo) > PickedPointTransformedSignalType;

    /// SLOT: Transforms picked points.
    void pickedPoint(::fwDataTools::PickingInfo);

};
} // namespace maths
