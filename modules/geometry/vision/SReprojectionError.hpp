/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include <data/Camera.hpp>
#include <data/Image.hpp>
#include <data/MarkerMap.hpp>
#include <data/Matrix4.hpp>

#include <service/IController.hpp>

#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   SReprojectionError Class used to compute the mean error of reprojection between 3d object points
 * and 2d image points.
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="sight::module::geometry::vision::SReprojectionError">
         <in group="matrix">
             <key uid="..." />
             <key uid="..." />
             <key uid="..." />
             <key uid="..." />
         </in>
         <in key="markerMap" uid="..." />
         <in key="camera" uid="..."/>
         <in key="extrinsic" uid="..." />
         <inout key="frame" uid="..." />
         <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerMap [sight::data::MarkerMap]: markers map list.
 * - \b camera [sight::data::Camera]: calibrated cameras.
 * - \b extrinsic [sight::data::Matrix4]: extrinsic matrix, only used if you have two cameras configured.
 * - \b matrix [sight::data::Matrix4]: list of matrices related to the markers. The marker's id must be
 * specified using the \b id tag to be found in the marker map.
 * @subsection InOut InOut
 * - \b frame [sight::data::Image]: video frame.
 * @subsection Output Output
 * - \b error [sight::data::Float] : computed error
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class MODULE_GEOMETRY_VISION_CLASS_API SReprojectionError : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SReprojectionError, service::IController);

    /// Double changed signal type
    typedef core::com::Signal<void (double)> ErrorComputedSignalType;

    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_COMPUTE_SLOT;

    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;

    ///Constructor
    MODULE_GEOMETRY_VISION_API SReprojectionError();

    ///Destructor
    MODULE_GEOMETRY_VISION_API ~SReprojectionError() override;

    /// Connect MatrixTL::s_OBJECT_PUSHED_SIG to s_COMPUTE_SLOT
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
     * @brief Updating method : This method is used to restart the service.
     */
    MODULE_GEOMETRY_VISION_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    MODULE_GEOMETRY_VISION_API void stopping() override;

private:

    void compute(core::HiResClock::HiResClockType timestamp);

    ///Slot called when a color value is changed
    void setColorParameter(std::array<std::uint8_t, 4> _val, std::string _key);
    ///Slot called when a boolean value is changed
    void setBoolParameter(bool _val, std::string _key);

    /// Last timestamp
    core::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    /// 3D object points
    std::vector<cv::Point3f> m_objectPoints;
    /// Camera Matrix (fx, fy, cx, cy)
    cv::Mat m_cameraMatrix;
    ///Distorsion coefficient
    cv::Mat m_distorsionCoef;
    /// Color of the reprojection circle
    cv::Scalar m_cvColor;
    /// if true: display circle centered at reprojection point.
    bool m_display;
    /// extrinsic matrix (can be identity)
    cv::Mat m_cvExtrinsic;

    /// List of tags associated with each input matrix
    std::vector<data::MarkerMap::KeyType> m_matricesTag;

    static constexpr std::string_view s_MATRIX_INPUT    = "matrix";
    static constexpr std::string_view s_MARKERMAP_INPUT = "markerMap";
    static constexpr std::string_view s_CAMERA_INPUT    = "camera";
    static constexpr std::string_view s_EXTRINSIC_INPUT = "extrinsic";
    static constexpr std::string_view s_FRAME_INOUT     = "frame";

    data::ptr_vector<data::Matrix4, data::Access::in> m_matrix {this, s_MATRIX_INPUT, true, 1};
    data::ptr<data::MarkerMap, data::Access::in> m_markerMap {this, s_MARKERMAP_INPUT};
    data::ptr<data::Camera, data::Access::in> m_camera {this, s_CAMERA_INPUT};
    data::ptr<data::Matrix4, data::Access::in> m_extrinsic {this, s_EXTRINSIC_INPUT};
    data::ptr<data::Image, data::Access::inout> m_frame {this, s_FRAME_INOUT};
};

} //namespace sight::module::geometry::vision
