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

#include <data/Camera.hpp>
#include <data/Image.hpp>
#include <data/Mesh.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <glm/mat4x4.hpp>

#include <service/IOperator.hpp>

namespace sight::modules::filter::vision
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
        <service uid="computePointCloud" type="::sight::modules::filter::vision::SPointCloudFromDepthMap">
            <in  key="depthMap" uid="..." />
            <in  key="rgbMap" uid="..." />
            <in  key="calibration" uid="..." />
            <inout key="pointCloud" uid="..." />
        </service>
 * @endcode
 * @subsection Input Input
 * - \b calibration (mandatory) [data::CameraSeries]: calibration rgbd sensor.
 * - \b depthMap (mandatory) [data::Image]: Grayscale image containing a depth map.
 * - \b rgbMap   (optional)  [data::Image]: RGB image corresponding to the depth map. If present, the RGB
 *   map must have the same dimensions as the depth map.
 *
 * @subsection In-Out In-Out
 * - \b pointCloud [sight::data::Mesh]: Computed point cloud.
 *
 * @subsection Configuration Configuration
 */
class MODULE_FILTER_VISION_CLASS_API SPointCloudFromDepthMap : public service::IOperator
{
public:
    fwCoreServiceMacro(SPointCloudFromDepthMap, service::IOperator)

    /**
     * @brief SPointCloudFromDepthMap constructor.
     */
    MODULE_FILTER_VISION_API SPointCloudFromDepthMap() noexcept;

    /**
     * @brief SPointCloudFromDepthMap destructor.
     */
    MODULE_FILTER_VISION_API virtual ~SPointCloudFromDepthMap() noexcept;

protected:
    /**
     * @brief Starts the service
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_VISION_API virtual void starting() override;

    /**
     * @brief Stops the service
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_VISION_API virtual void stopping() override;

    /**
     * @brief Configures the service
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_VISION_API virtual void configuring() override;

    /**
     * @brief Does nothing
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_VISION_API virtual void updating() override;

private:

    /**
     * @name Slots API
     * @{
     */
    MODULE_FILTER_VISION_API static const core::com::Slots::SlotKeyType s_SET_DEPTH_RANGE;
    /** @} */

    /// SLOT: update the depth range
    void setDepthRange(int depth, std::string key);

    /**
     * @brief Computes a point cloud from a depth map.
     */
    void depthMapToPointCloud(const data::Camera::csptr& depthCamera,
                              const data::Image::csptr& depthMap,
                              const data::Mesh::sptr& pointCloud);

    /**
     * @brief Computes a point cloud with colors from a depth map and a color
     * map.
     */
    void depthMapToPointCloudRGB(const data::Camera::csptr& depthCamera,
                                 const data::Camera::csptr& colorCamera,
                                 const data::Image::csptr& depthMap,
                                 const data::Image::csptr& colorMap,
                                 const data::TransformationMatrix3D::csptr& extrinsic,
                                 const data::Mesh::sptr& pointCloud);

    /// Min value of depth used to build pointcloud.
    std::uint16_t m_minDepth = 0;
    /// Max value of depth used to build pointcloud.
    std::uint16_t m_maxDepth = UINT16_MAX;

};

} // namespace sight::modules::filter::vision
