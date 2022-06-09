/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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
#include "modules/geometry/vision/config.hpp"

#include <core/HiResClock.hpp>
#include <core/mt/types.hpp>

#include <data/Camera.hpp>
#include <data/MarkerMap.hpp>
#include <data/Matrix4.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

#include <service/IRegisterer.hpp>

#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   SPoseFrom2d Class used to compute the 3d pose of a object using 2d points.
 *
 * @section Slots Slots
 * - \b computeRegistration(core::HiResClock::HiResClockType timestamp) : computes the registration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="sight::module::geometry::vision::SPoseFrom2d">
         <in group="markerMap" autoConnect="true">
             <key uid="..." />
             <key uid="..." />
         </in>
         <in group="camera">
             <key uid="..." />
             <key uid="..." />
         </in>
         <in key="extrinsic" uid="..." />
         <in group="matrix">
             <key uid="..." id="101"/>
             <key uid="..." id="102"/>
             <key uid="..." id="103"/>
             <key uid="..." id="104"/>
         </in>
         <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerMap [sight::data::MarkerMap]: markers map list.
 * - \b camera [sight::data::Camera]: calibrated cameras.
 * - \b extrinsic [sight::data::Matrix4]: extrinsic matrix, only used if you have two cameras configured.
 * @subsection In-Out In-Out
 * - \b matrix [sight::data::Matrix4]: list of matrices related to the markers. The marker's id must be
 * specified using the \b id tag to be found in the marker map.
 * - \b pointList [sight::data::PointList] (optional): list of points corresponding to the model.
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class MODULE_GEOMETRY_VISION_CLASS_API SPoseFrom2d : public service::IRegisterer
{
public:

    SIGHT_DECLARE_SERVICE(SPoseFrom2d, service::IRegisterer);

    typedef std::vector<std::string> VectKeyType;

    MODULE_GEOMETRY_VISION_API SPoseFrom2d() noexcept;
    MODULE_GEOMETRY_VISION_API ~SPoseFrom2d() noexcept override = default;

    /// Connect MarkerTL::s_OBJECT_PUSHED_SIG to s_REGISTER_SLOT
    service::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    MODULE_GEOMETRY_VISION_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    MODULE_GEOMETRY_VISION_API void starting() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    MODULE_GEOMETRY_VISION_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    MODULE_GEOMETRY_VISION_API void stopping() override;

    /// Register matrix slot
    void computeRegistration(core::HiResClock::HiResClockType timestamp) override;

private:

    /**
     * @brief The Marker struct:
     * to handle marker object, with 4 corners 2D, and optionally a centroid
     */
    struct Marker
    {
        std::vector<cv::Point2f> corners2D;
        cv::Point3f centroid;
    };

    /**
     * @brief The Camera struct : to handle intrinsic parameters and distorsion coefficient.
     */
    struct Camera
    {
        cv::Size imageSize;
        cv::Mat intrinsicMat;
        cv::Mat distCoef;
    };
    /**
     * @brief The Extrinsic struct : to handle several format of extrinsic matrix
     */
    struct Extrinsic
    {
        cv::Mat rotation;
        cv::Mat translation;
        cv::Mat Matrix4x4;
    };

    /// Initialize cameras
    void initialize();

    /**
     * @brief : Compute the camera position from a maker detected on two separated view
     * @param : Marker points in each view
     *
     **/
    const cv::Matx44f cameraPoseFromStereo(const Marker& _markerCam1, const Marker& _markerCam2) const;

    /**
     * @brief :Compute the camera position from a marker detected in one view
     **/
    const cv::Matx44f cameraPoseFromMono(const Marker& _markerCam1) const;

    /// Last timestamp
    core::HiResClock::HiResClockType m_lastTimestamp {0};

    /// Marker pattern width.
    double m_patternWidth {80};

    /// 3d model
    std::vector<cv::Point3f> m_3dModel;

    /// std::vector of cameras
    std::vector<Camera> m_cameras;

    /// Extrinsic matrix
    Extrinsic m_extrinsicMat;

    /// List of tags associated with each inout matrix
    std::vector<data::MarkerMap::KeyType> m_matricesTag;

    static constexpr std::string_view s_MARKERMAP_INPUT = "markerMap";
    static constexpr std::string_view s_CAMERA_INPUT    = "camera";
    static constexpr std::string_view s_EXTRINSIC_INPUT = "extrinsic";
    static constexpr std::string_view s_MATRIX_INOUT    = "matrix";
    static constexpr std::string_view s_POINTLIST_INOUT = "pointList";

    data::ptr_vector<data::MarkerMap, data::Access::in> m_markerMap {this, s_MARKERMAP_INPUT, true};
    data::ptr_vector<data::Camera, data::Access::in> m_camera {this, s_CAMERA_INPUT, true};
    data::ptr<data::Matrix4, data::Access::in> m_extrinsic {this, s_EXTRINSIC_INPUT};
    data::ptr_vector<data::Matrix4, data::Access::inout> m_matrix {this, s_MATRIX_INOUT};
    data::ptr<data::PointList, data::Access::inout> m_pointList {this, s_POINTLIST_INOUT, false, true};
};

} // namespace sight::module::geometry::vision
