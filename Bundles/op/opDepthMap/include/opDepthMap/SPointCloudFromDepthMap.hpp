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

#include <arData/Camera.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IOperator.hpp>

#include <glm/mat4x4.hpp>

namespace opDepthMap
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
        <service uid="computePointCloud" type="::opDepthMap::SPointCloudFromDepthMap">
            <in  key="depthMap" uid="..." />
            <in  key="rgbMap" uid="..." />
            <in  key="calibration" uid="..." />
            <inout key="pointCloud" uid="..." />
        </service>
 * @endcode
 * @subsection Input Input
 * - \b calibration (mandatory) [::arData::CameraSeries]: calibration rgbd sensor.
 * - \b depthMap (mandatory) [::fwData::Image]: Grayscale image containing a depth map.
 * - \b rgbMap   (optional)  [::fwData::Image]: RGB image corresponding to the depth map. If present, the RGB
 *   map must have the same dimensions as the depth map.
 *
 * @subsection In-Out In-Out
 * - \b pointCloud [::fwData::Mesh]: Computed point cloud.
 *
 * @subsection Configuration Configuration
 */
class OPDEPTHMAP_CLASS_API SPointCloudFromDepthMap : public ::fwServices::IOperator
{
public:
    fwCoreServiceClassDefinitionsMacro((SPointCloudFromDepthMap)(::fwServices::IOperator))

    /**
     * @brief SPointCloudFromDepthMap constructor.
     */
    OPDEPTHMAP_API SPointCloudFromDepthMap() noexcept;

    /**
     * @brief SPointCloudFromDepthMap destructor.
     */
    OPDEPTHMAP_API virtual ~SPointCloudFromDepthMap() noexcept;

protected:
    /**
     * @brief Starts the service
     *
     * @throw ::fwTools::Failed
     */
    OPDEPTHMAP_API virtual void starting() override;

    /**
     * @brief Stops the service
     *
     * @throw ::fwTools::Failed
     */
    OPDEPTHMAP_API virtual void stopping() override;

    /**
     * @brief Configures the service
     *
     * @throw ::fwTools::Failed
     */
    OPDEPTHMAP_API virtual void configuring() override;

    /**
     * @brief Does nothing
     *
     * @throw ::fwTools::Failed
     */
    OPDEPTHMAP_API virtual void updating() override;

private:

    /**
     * @name Slots API
     * @{
     */
    OPDEPTHMAP_API static const ::fwCom::Slots::SlotKeyType s_SET_DEPTH_RANGE;
    /** @} */

    /// SLOT: update the depth range
    void setDepthRange(int depth, std::string key);

    /**
     * @brief Computes a point cloud from a depth map.
     */
    void depthMapToPointCloud(const ::arData::Camera::csptr& depthCamera,
                              const ::fwData::Image::csptr& depthMap,
                              const ::fwData::Mesh::sptr& pointCloud);

    /**
     * @brief Computes a point cloud with colors from a depth map and a color
     * map.
     */
    void depthMapToPointCloudRGB(const ::arData::Camera::csptr& depthCamera,
                                 const ::arData::Camera::csptr& colorCamera,
                                 const ::fwData::Image::csptr& depthMap,
                                 const ::fwData::Image::csptr& colorMap,
                                 const ::fwData::TransformationMatrix3D::csptr& extrinsic,
                                 const ::fwData::Mesh::sptr& pointCloud);

    /// Min value of depth used to build pointcloud.
    std::uint16_t m_minDepth = 0;
    /// Max value of depth used to build pointcloud.
    std::uint16_t m_maxDepth = UINT16_MAX;

};

} // namespace opDepthMap
