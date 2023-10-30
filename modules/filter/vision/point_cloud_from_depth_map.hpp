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
#include <data/matrix4.hpp>
#include <data/mesh.hpp>

#include <service/filter.hpp>

#include <glm/mat4x4.hpp>

namespace sight::module::filter::vision
{

/**
 * @brief Service to compute a point cloud from a depth map obtained from an RGBD camera. Optionally, combine it
 * with an RGB map to get a colored point cloud.
 *
 * @section Signals Signals
 * - \b computed(): Emitted when a point cloud has been computed.
 *
 * @section Slots Slots
 * - \b compute(double): Compute a point cloud from the input image(s).
 * - \b setDepthRange(int depth, std::string key): Set min/max depth (with key "minDepth" or "maxDepth").
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="computePointCloud" type="sight::module::filter::vision::point_cloud_from_depth_map">
            <in  key="depthMap" uid="..." />
            <in  key="rgbMap" uid="..." />
            <in  key="calibration" uid="..." />
            <inout key="pointCloud" uid="..." />
        </service>
 * @endcode
 * @subsection Input Input
 * - \b calibration (mandatory) [data::camera_set]: calibration rgbd sensor.
 * - \b depthMap (mandatory) [data::image]: Grayscale image containing a depth map.
 * - \b rgbMap   (optional)  [data::image]: RGB image corresponding to the depth map. If present, the RGB
 *   map must have the same dimensions as the depth map.
 *
 * @subsection In-Out In-Out
 * - \b pointCloud [sight::data::mesh]: Computed point cloud.
 *
 * @subsection Configuration Configuration
 */
class MODULE_FILTER_VISION_CLASS_API point_cloud_from_depth_map : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(point_cloud_from_depth_map, service::filter);

    /**
     * @brief point_cloud_from_depth_map constructor.
     */
    MODULE_FILTER_VISION_API point_cloud_from_depth_map() noexcept;

    /**
     * @brief point_cloud_from_depth_map destructor.
     */
    MODULE_FILTER_VISION_API ~point_cloud_from_depth_map() noexcept override;

protected:

    /**
     * @brief Starts the service
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_VISION_API void starting() override;

    /**
     * @brief Stops the service
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_VISION_API void stopping() override;

    /**
     * @brief Configures the service
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_VISION_API void configuring() override;

    /**
     * @brief Does nothing
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_VISION_API void updating() override;

private:

    /**
     * @name Slots API
     * @{
     */
    MODULE_FILTER_VISION_API static const core::com::slots::key_t SET_DEPTH_RANGE;
    /** @} */

    /// SLOT: update the depth range
    void set_depth_range(int _val, std::string _key);

    /**
     * @brief Computes a point cloud from a depth map.
     */
    void depth_map_to_point_cloud(
        const data::camera::csptr& _depth_camera,
        const data::image::csptr& _depth_map,
        const data::mesh::sptr& _point_cloud
    ) const;

    /**
     * @brief Computes a point cloud with colors from a depth map and a color
     * map.
     */
    void depth_map_to_point_cloud_rgb(
        const data::camera::csptr& _depth_camera,
        const data::camera::csptr& _color_camera,
        const data::image::csptr& _depth_map,
        const data::image::csptr& _color_map,
        const data::matrix4::csptr& _extrinsic,
        const data::mesh::sptr& _point_cloud
    ) const;

    /// Min value of depth used to build pointcloud.
    std::uint16_t m_min_depth = 0;
    /// Max value of depth used to build pointcloud.
    std::uint16_t m_max_depth = UINT16_MAX;

    sight::data::ptr<sight::data::camera_set, sight::data::access::in> m_calibration {this, "calibration"};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_depth_map {this, "depthMap"};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_rgb_map {this, "rgbMap"};
    sight::data::ptr<sight::data::mesh, sight::data::access::inout> m_point_cloud {this, "pointCloud"};
};

} // namespace sight::module::filter::vision
