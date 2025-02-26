/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <data/boolean.hpp>
#include <data/camera.hpp>
#include <data/color.hpp>
#include <data/image.hpp>
#include <data/marker_map.hpp>
#include <data/matrix4.hpp>
#include <data/real.hpp>

#include <service/controller.hpp>

#include <ui/__/parameter.hpp>

#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   reprojection_error Class used to compute the mean error of reprojection between 3d object points
 * and 2d image points.
 *
 * @section Slots Slots
 * - \b set_parameter(ui::parameter_t, std::string): set a parameter from the UI.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="sight::module::geometry::vision::reprojection_error">
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
 * - \b markerMap [sight::data::marker_map]: markers map list.
 * - \b camera [sight::data::camera]: calibrated cameras.
 * - \b extrinsic [sight::data::matrix4]: extrinsic matrix, only used if you have two cameras configured.
 * - \b matrix [sight::data::matrix4]: list of matrices related to the markers. The marker's id must be
 * specified using the \b id tag to be found in the marker map.
 * @subsection InOut InOut
 * - \b frame [sight::data::image]: video frame.
 * @subsection Output Output
 * - \b error [sight::data::real] : computed error
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class reprojection_error : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(reprojection_error, service::controller);

    /// Double changed signal type
    using error_computed_t = core::com::signal<void (double)>;

    static const core::com::slots::key_t COMPUTE_SLOT;
    static const core::com::slots::key_t SET_PARAMETER_SLOT;

    reprojection_error();
    ~reprojection_error() override = default;

    /// Connect MatrixTL::signals::PUSHED to COMPUTE_SLOT
    service::connections_t auto_connections() const override;

protected:

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    void configuring(const config_t& _config) override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    void starting() override;

    /**
     * @brief Updating method : This method is used to restart the service.
     */
    void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    void stopping() override;

private:

    void compute(core::clock::type _timestamp);

    /// Last timestamp
    core::clock::type m_last_timestamp {0};

    /// 3D object points
    std::vector<cv::Point3f> m_object_points;
    /// Camera Matrix (fx, fy, cx, cy)
    cv::Mat m_camera_matrix;
    ///Distorsion coefficient
    cv::Mat m_distorsion_coef;
    /// extrinsic matrix (can be identity)
    cv::Mat m_cv_extrinsic;

    /// List of tags associated with each input matrix
    std::vector<data::marker_map::key_t> m_matrices_tag;

    static constexpr std::string_view MATRIX_INPUT    = "matrix";
    static constexpr std::string_view MARKERMAP_INPUT = "markerMap";
    static constexpr std::string_view CAMERA_INPUT    = "camera";
    static constexpr std::string_view EXTRINSIC_INPUT = "extrinsic";
    static constexpr std::string_view FRAME_INOUT     = "frame";

    data::ptr_vector<data::matrix4, data::access::in> m_matrix {this, MATRIX_INPUT};
    data::ptr<data::marker_map, data::access::in> m_marker_map {this, MARKERMAP_INPUT};
    data::ptr<data::camera, data::access::in> m_camera {this, CAMERA_INPUT};
    data::ptr<data::matrix4, data::access::in> m_extrinsic {this, EXTRINSIC_INPUT};
    data::ptr<data::image, data::access::inout> m_frame {this, FRAME_INOUT};

    data::property<data::boolean> m_display {this, "display", true};
    data::property<data::color> m_color {this, "color", {1.0, 1.0, 1.0, 1.0}};
    data::property<data::real> m_pattern_width {this, "pattern_width", 80.};
};

} //namespace sight::module::geometry::vision
