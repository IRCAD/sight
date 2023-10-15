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

#include <data/camera_set.hpp>
#include <data/image.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::vision
{

/**
 * @brief This service applies a scale on a depth map image.
 *
 * The scale is given by the calibration (data::camera_set).
 *
 * @section Signals Signals
 * - \b computed() : emitted when the compute method is done
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::filter::vision::transform_depth_map2mm">
       <in key="cameraSet" uid="..." />
       <in key="originDepth" uid="..." />
       <inout key="scaledDepth" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b cameraSet [sight::data::camera_set]: camera series containing the scale of the depth map.
 * - \b originDepth [sight::data::image]: original depth map.
 * @subsection In-Out In-Out
 * - \b scaledDepth [sight::data::image]: scaled depth map.
 */
class MODULE_FILTER_VISION_CLASS_API transform_depth_map2mm : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(transform_depth_map2mm, service::filter);

    /**
     * @brief transform_depth_map2mm constructor.
     */
    MODULE_FILTER_VISION_API transform_depth_map2mm();

    /**
     * @brief transform_depth_map2mm destructor.
     */
    MODULE_FILTER_VISION_API ~transform_depth_map2mm() override;

protected:

    /// Do nothing
    MODULE_FILTER_VISION_API void starting() override;

    /// Do nothing
    MODULE_FILTER_VISION_API void stopping() override;

    /// Do nothing
    MODULE_FILTER_VISION_API void configuring() override;

    /// Apply the scale on the depth map
    MODULE_FILTER_VISION_API void updating() override;

private:

    static constexpr std::string_view s_CAMERA_SET_INPUT   = "cameraSet";
    static constexpr std::string_view s_ORIGIN_FRAME_INPUT = "originDepth";
    static constexpr std::string_view s_SCALED_FRAME_INOUT = "scaledDepth";

    sight::data::ptr<sight::data::camera_set, sight::data::Access::in> m_camera_set {this, s_CAMERA_SET_INPUT};
    sight::data::ptr<sight::data::image, sight::data::Access::in> m_originDepth {this, s_ORIGIN_FRAME_INPUT};
    sight::data::ptr<sight::data::image, sight::data::Access::inout> m_scaledDepth {this, s_SCALED_FRAME_INOUT};
};

} // namespace sight::module::filter::vision
