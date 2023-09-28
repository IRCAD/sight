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
#include <data/Image.hpp>
#include <data/Matrix4.hpp>
#include <data/Mesh.hpp>

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
        <service uid="computePointCloud" type="sight::module::filter::vision::SPointCloudFromDepthMap">
            <in  key="depthMap" uid="..." />
            <in  key="rgbMap" uid="..." />
            <in  key="calibration" uid="..." />
            <inout key="pointCloud" uid="..." />
        </service>
 * @endcode
 * @subsection Input Input
 * - \b calibration (mandatory) [data::CameraSet]: calibration rgbd sensor.
 * - \b depthMap (mandatory) [data::Image]: Grayscale image containing a depth map.
 * - \b rgbMap   (optional)  [data::Image]: RGB image corresponding to the depth map. If present, the RGB
 *   map must have the same dimensions as the depth map.
 *
 * @subsection In-Out In-Out
 * - \b pointCloud [sight::data::Mesh]: Computed point cloud.
 *
 * @subsection Configuration Configuration
 */
class MODULE_FILTER_VISION_CLASS_API SPointCloudFromDepthMap : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(SPointCloudFromDepthMap, service::filter);

    /**
     * @brief SPointCloudFromDepthMap constructor.
     */
    MODULE_FILTER_VISION_API SPointCloudFromDepthMap() noexcept;

    /**
     * @brief SPointCloudFromDepthMap destructor.
     */
    MODULE_FILTER_VISION_API ~SPointCloudFromDepthMap() noexcept override;

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
    void setDepthRange(int _val, std::string key);

    /**
     * @brief Computes a point cloud from a depth map.
     */
    void depthMapToPointCloud(
        const data::Camera::csptr& depthCamera,
        const data::Image::csptr& depthMap,
        const data::Mesh::sptr& pointCloud
    ) const;

    /**
     * @brief Computes a point cloud with colors from a depth map and a color
     * map.
     */
    void depthMapToPointCloudRGB(
        const data::Camera::csptr& depthCamera,
        const data::Camera::csptr& colorCamera,
        const data::Image::csptr& depthMap,
        const data::Image::csptr& colorMap,
        const data::Matrix4::csptr& extrinsic,
        const data::Mesh::sptr& pointCloud
    ) const;

    /// Min value of depth used to build pointcloud.
    std::uint16_t m_minDepth = 0;
    /// Max value of depth used to build pointcloud.
    std::uint16_t m_maxDepth = UINT16_MAX;

    sight::data::ptr<sight::data::CameraSet, sight::data::Access::in> m_calibration {this, "calibration"};
    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_depthMap {this, "depthMap"};
    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_rgbMap {this, "rgbMap"};
    sight::data::ptr<sight::data::Mesh, sight::data::Access::inout> m_pointCloud {this, "pointCloud"};
};

} // namespace sight::module::filter::vision
