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
 * @brief This service applies a scale on a depth map buffer extracted from a timeline.
 *
 * The scale is given by the calibration (data::CameraSeries). The scaled map is pushed in the output timeline with
 * the same timestamp as the input map.
 *
 * @section Signals Signals
 * - \b computed() : emitted when the compute method is done
 *
 * @section Slots Slots
 * - \b compute(double) : apply the scale on the depth map
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::filter::vision::STransformDepthTL2mm">
       <in key="cameraSeries" uid="..." />
       <in key="originDepthTL" uid="..." />
       <inout key="scaledDepthTL" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b cameraSeries [sight::data::CameraSeries]: camera series containing the scale of the depth map.
 * - \b originDepthTL [sight::data::FrameTL]: timeline containing the original depth maps.
 * @subsection In-Out In-Out
 * - \b scaledDepthTL [sight::data::FrameTL]: timeline containing the scaled depth maps.
 */
class MODULE_FILTER_VISION_CLASS_API STransformDepthTL2mm : public services::IOperator
{
public:
    fwCoreServiceMacro(STransformDepthTL2mm, services::IOperator)

    /**
     * @brief STransformDepthTL2mm constructor.
     */
    MODULE_FILTER_VISION_API STransformDepthTL2mm();

    /**
     * @brief STransformDepthTL2mm destructor.
     */
    MODULE_FILTER_VISION_API virtual ~STransformDepthTL2mm();

protected:

    /// Do nothing
    MODULE_FILTER_VISION_API virtual void starting() override;

    /// Do nothing
    MODULE_FILTER_VISION_API virtual void stopping() override;

    /// Do nothing
    MODULE_FILTER_VISION_API virtual void configuring() override;

    /// Do nothing
    MODULE_FILTER_VISION_API virtual void updating() override;

    /// Defines the auto-connection between the depth timeline and the 'compute' slot
    MODULE_FILTER_VISION_API services::IService::KeyConnectionsMap getAutoConnections() const override;

private:
    /// Apply the scale on the depth map
    void compute(core::HiResClock::HiResClockType timestamp);

    /// timestamp of the last process
    core::HiResClock::HiResClockType m_lastTimestamp;
};

} // namespace sight::modules::filter::vision
