/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#include <opencv2/core/mat.hpp>

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
         <input marker_map="..." camera="..." extrinsic="...">
             <matrix>
                 <item data="..." id="..." />
             </matrix>
         </input>
         <output frame="..." />
         <config pattern_width="80" />
     </service>
   @endcode
 * @subsection Input Input
 * - \b input.camera [sight::data::camera]: calibrated cameras.
 * - \b input.extrinsic [sight::data::matrix4]: extrinsic matrix, only used if you have two cameras configured.
 * - \b input.matrix.item.data [sight::data::matrix4]: list of matrices related to the markers. The marker's id must be
 * specified using the \b id tag to be found in the marker map.
 * - \b input.marker_map [sight::data::marker_map]: markers map list.
 * @subsection In-Out In-Out
 * - \b output.frame [sight::data::image]: video frame.
 * @subsection Output Output
 * - \b error [sight::data::real] : computed error
 * @subsection Configuration Configuration
 * - \b config.display : enables drawing of the reprojected points.
 * - \b config.color : color used to draw the reprojected points.
 * - \b config.pattern_width : width of the tag.
 */
class reprojection_error : public service::controller
{
public:

    struct signals
    {
        using error_computed_t = core::com::signal<void (double)>;
        static inline const signal_key_t ERROR_COMPUTED = "error_computed";
    };

    struct slots
    {
        static inline const slot_key_t COMPUTE       = "compute";
        static inline const slot_key_t SET_PARAMETER = "set_parameter";
    };

    SIGHT_DECLARE_SERVICE(reprojection_error, service::controller);

    reprojection_error();
    ~reprojection_error() override = default;

protected:

    /// Connect MatrixTL::signals::PUSHED to COMPUTE
    service::connections_t auto_connections() const override;

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

    data::ptr_vector<data::matrix4, data::access::in> m_matrix {this, "input.matrix.item.data"};
    data::ptr<data::marker_map, data::access::in> m_marker_map {this, "input.marker_map"};
    data::ptr<data::camera, data::access::in> m_camera {this, "input.camera"};
    data::ptr<data::matrix4, data::access::in> m_extrinsic {this, "input.extrinsic"};
    data::ptr<data::image, data::access::inout> m_frame {this, "output.frame"};

    data::ptr<data::boolean> m_display {this, "config.display", true};
    data::ptr<data::color> m_color {this, "config.color", {1.0, 1.0, 1.0, 1.0}};
    data::ptr<data::real> m_pattern_width {this, "config.pattern_width", 80.};
};

} //namespace sight::module::geometry::vision
