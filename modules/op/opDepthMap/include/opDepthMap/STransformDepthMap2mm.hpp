/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "opDepthMap/config.hpp"

#include <fwServices/IOperator.hpp>

namespace opDepthMap
{

/**
 * @brief This service applies a scale on a depth map image.
 *
 * The scale is given by the calibration (::arData::CameraSeries).
 *
 * @section Signals Signals
 * - \b computed() : emitted when the compute method is done
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::opDepthMap::STransformDepthMap2mm">
       <in key="cameraSeries" uid="..." />
       <in key="originDepth" uid="..." />
       <inout key="scaledDepth" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b cameraSeries [::arData::CameraSeries]: camera series containing the scale of the depth map.
 * - \b originDepth [::fwData::Image]: original depth map.
 * @subsection In-Out In-Out
 * - \b scaledDepth [::fwData::Image]: scaled depth map.
 */
class OPDEPTHMAP_CLASS_API STransformDepthMap2mm : public ::fwServices::IOperator
{
public:
    fwCoreServiceMacro(STransformDepthMap2mm, ::fwServices::IOperator);

    /**
     * @brief STransformDepthMap2mm constructor.
     */
    OPDEPTHMAP_API STransformDepthMap2mm();

    /**
     * @brief STransformDepthMap2mm destructor.
     */
    OPDEPTHMAP_API virtual ~STransformDepthMap2mm();

protected:

    /// Do nothing
    OPDEPTHMAP_API virtual void starting() override;

    /// Do nothing
    OPDEPTHMAP_API virtual void stopping() override;

    /// Do nothing
    OPDEPTHMAP_API virtual void configuring() override;

    /// Apply the scale on the depth map
    OPDEPTHMAP_API virtual void updating() override;
};

} // namespace opDepthMap
