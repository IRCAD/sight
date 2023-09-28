/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include <data/CameraSet.hpp>
#include <data/FrameTL.hpp>

#include <service/filter.hpp>

#include <string_view>

namespace sight::module::filter::vision
{

/**
 * @brief This service applies a scale on a depth map buffer extracted from a timeline.
 *
 * The scale is given by the calibration (data::CameraSet). The scaled map is pushed in the output timeline with
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
   <service type="sight::module::filter::vision::STransformDepthTL2mm">
       <in key="cameraSet" uid="..." />
       <in key="originDepthTL" uid="..." />
       <inout key="scaledDepthTL" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b cameraSet [sight::data::CameraSet]: camera series containing the scale of the depth map.
 * - \b originDepthTL [sight::data::FrameTL]: timeline containing the original depth maps.
 * @subsection In-Out In-Out
 * - \b scaledDepthTL [sight::data::FrameTL]: timeline containing the scaled depth maps.
 */
class MODULE_FILTER_VISION_CLASS_API STransformDepthTL2mm : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(STransformDepthTL2mm, service::filter);

    /**
     * @brief STransformDepthTL2mm constructor.
     */
    MODULE_FILTER_VISION_API STransformDepthTL2mm();

    /**
     * @brief STransformDepthTL2mm destructor.
     */
    MODULE_FILTER_VISION_API ~STransformDepthTL2mm() override;

protected:

    /// Do nothing
    MODULE_FILTER_VISION_API void starting() override;

    /// Do nothing
    MODULE_FILTER_VISION_API void stopping() override;

    /// Do nothing
    MODULE_FILTER_VISION_API void configuring() override;

    /// Do nothing
    MODULE_FILTER_VISION_API void updating() override;

    /// Defines the auto-connection between the depth timeline and the 'compute' slot
    MODULE_FILTER_VISION_API service::connections_t getAutoConnections() const override;

private:

    /// Apply the scale on the depth map
    void compute(core::hires_clock::type timestamp);

    /// timestamp of the last process
    core::hires_clock::type m_lastTimestamp {0};

    static constexpr std::string_view s_CAMERA_SET_INPUT      = "cameraSet";
    static constexpr std::string_view s_ORIGIN_FRAME_TL_INPUT = "originDepthTL";
    static constexpr std::string_view s_SCALED_FRAME_TL_INOUT = "scaledDepthTL";

    sight::data::ptr<sight::data::CameraSet, sight::data::Access::in> m_camera_set {this, s_CAMERA_SET_INPUT};
    sight::data::ptr<sight::data::FrameTL, sight::data::Access::in> m_originFrameTL {this, s_ORIGIN_FRAME_TL_INPUT};
    sight::data::ptr<sight::data::FrameTL, sight::data::Access::inout> m_scaledDepthTL {this, s_SCALED_FRAME_TL_INOUT};
};

} // namespace sight::module::filter::vision
