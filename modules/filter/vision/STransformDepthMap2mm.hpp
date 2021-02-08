/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "modules/filter/vision/config.hpp"

#include <services/IOperator.hpp>

namespace sight::modules::filter::vision
{

/**
 * @brief This service applies a scale on a depth map image.
 *
 * The scale is given by the calibration (data::CameraSeries).
 *
 * @section Signals Signals
 * - \b computed() : emitted when the compute method is done
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::modules::filter::vision::STransformDepthMap2mm">
       <in key="cameraSeries" uid="..." />
       <in key="originDepth" uid="..." />
       <inout key="scaledDepth" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b cameraSeries [data::CameraSeries]: camera series containing the scale of the depth map.
 * - \b originDepth [data::Image]: original depth map.
 * @subsection In-Out In-Out
 * - \b scaledDepth [data::Image]: scaled depth map.
 */
class MODULE_FILTER_VISION_CLASS_API STransformDepthMap2mm : public services::IOperator
{
public:
    fwCoreServiceMacro(STransformDepthMap2mm, services::IOperator)

    /**
     * @brief STransformDepthMap2mm constructor.
     */
    MODULE_FILTER_VISION_API STransformDepthMap2mm();

    /**
     * @brief STransformDepthMap2mm destructor.
     */
    MODULE_FILTER_VISION_API virtual ~STransformDepthMap2mm();

protected:

    /// Do nothing
    MODULE_FILTER_VISION_API virtual void starting() override;

    /// Do nothing
    MODULE_FILTER_VISION_API virtual void stopping() override;

    /// Do nothing
    MODULE_FILTER_VISION_API virtual void configuring() override;

    /// Apply the scale on the depth map
    MODULE_FILTER_VISION_API virtual void updating() override;
};

} // namespace sight::modules::filter::vision
