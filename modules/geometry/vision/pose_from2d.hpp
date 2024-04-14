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

#include <core/clock.hpp>
#include <core/mt/types.hpp>

#include <data/camera.hpp>
#include <data/marker_map.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

#include <service/registerer.hpp>

#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   pose_from2d Class used to compute the 3d pose of a object using 2d points.
 *
 * @section Slots Slots
 * - \b compute_registration(core::clock::type timestamp) : computes the registration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="sight::module::geometry::vision::pose_from2d">
         <in group="markerMap" auto_connect="true">
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
 * - \b markerMap [sight::data::marker_map]: markers map list.
 * - \b camera [sight::data::camera]: calibrated cameras.
 * - \b extrinsic [sight::data::matrix4]: extrinsic matrix, only used if you have two cameras configured.
 * @subsection In-Out In-Out
 * - \b matrix [sight::data::matrix4]: list of matrices related to the markers. The marker's id must be
 * specified using the \b id tag to be found in the marker map.
 * - \b pointList [sight::data::point_list] (optional): list of points corresponding to the model.
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class pose_from2d : public service::registerer
{
public:

    SIGHT_DECLARE_SERVICE(pose_from2d, service::registerer);

    using vect_key_t = std::vector<std::string>;

    pose_from2d() noexcept;
    ~pose_from2d() noexcept override = default;

    /// Connect MarkerTL::signals::PUSHED to REGISTER_SLOT
    service::connections_t auto_connections() const override;

protected:

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    void starting() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    void stopping() override;

    /// Register matrix slot
    void compute_registration(core::clock::type _timestamp) override;

private:

    /**
     * @brief The Marker struct:
     * to handle marker object, with 4 corners 2D, and optionally a centroid
     */
    struct marker
    {
        std::vector<cv::Point2f> corners_2d;
        cv::Point3f centroid;
    };

    /**
     * @brief The Camera struct : to handle intrinsic parameters and distorsion coefficient.
     */
    struct camera
    {
        cv::Size image_size;
        cv::Mat intrinsic_mat;
        cv::Mat dist_coef;
    };
    /**
     * @brief The Extrinsic struct : to handle several format of extrinsic matrix
     */
    struct extrinsic
    {
        cv::Mat rotation;
        cv::Mat translation;
        cv::Mat matrix4x4;
    };

    /// Initialize cameras
    void initialize();

    /**
     * @brief : Compute the camera position from a maker detected on two separated view
     * @param : Marker points in each view
     *
     **/
    cv::Matx44f camera_pose_from_stereo(const marker& _marker_cam1, const marker& _marker_cam2) const;

    /**
     * @brief :Compute the camera position from a marker detected in one view
     **/
    cv::Matx44f camera_pose_from_mono(const marker& _marker_cam1) const;

    /// Last timestamp
    core::clock::type m_last_timestamp {0};

    /// Marker pattern width.
    double m_pattern_width {80};

    /// 3d model
    std::vector<cv::Point3f> m_3d_model;

    /// std::vector of cameras
    std::vector<camera> m_cameras;

    /// Extrinsic matrix
    extrinsic m_extrinsic_mat;

    /// List of tags associated with each inout matrix
    std::vector<data::marker_map::key_t> m_matrices_tag;

    static constexpr std::string_view MARKERMAP_INPUT = "markerMap";
    static constexpr std::string_view CAMERA_INPUT    = "camera";
    static constexpr std::string_view EXTRINSIC_INPUT = "extrinsic";
    static constexpr std::string_view MATRIX_INOUT    = "matrix";
    static constexpr std::string_view POINTLIST_INOUT = "pointList";

    data::ptr_vector<data::marker_map, data::access::in> m_marker_map {this, MARKERMAP_INPUT, true};
    data::ptr_vector<data::camera, data::access::in> m_camera {this, CAMERA_INPUT, true};
    data::ptr<data::matrix4, data::access::in> m_extrinsic {this, EXTRINSIC_INPUT};
    data::ptr_vector<data::matrix4, data::access::inout> m_matrix {this, MATRIX_INOUT};
    data::ptr<data::point_list, data::access::inout> m_point_list {this, POINTLIST_INOUT, false, true};
};

} // namespace sight::module::geometry::vision
