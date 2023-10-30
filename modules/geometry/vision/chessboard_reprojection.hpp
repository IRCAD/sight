/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "modules/geometry/vision/config.hpp"

#include <data/camera.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>

#include <service/controller.hpp>

#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief Reprojects the chessboard model onto the detected points using the estimated camera pose and calibration.
 *
 * Computes the reprojection error and sends it through a signal.
 *
 * @section Slots Slots
 *  - \b toggleDistortion(): toggles distortion for the reprojected points.
 *  - \b updateChessboardSize(): updates the chessboard model from the preferences.
 *
 * @section Signal Signal
 *  -\b errorComputed(double): sends the computed RMSE in pixels.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="sight::module::geometry::vision::chessboard_reprojection">
        <in key="transform" uid="..."/>
        <in key="camera" uid="..."/>
        <in key="detectedChessboard" uid="..." />
        <inout key="videoImage" uid="..." />
        <out key="chessboardModel" uid="..." />
        <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
        <config drawDetected="true" drawReprojection="true" distortReprojection="true" drawReprojectionError="true" />
     </service>
   @endcode
 *
 * @subsection Input Input
 * - \b transform [sight::data::matrix4] (mandatory): transform between the chessboard and the camera.
 * - \b camera [sight::data::camera] (mandatory): camera filming the chessboard.
 * - \b detectedChessboard [sight::data::point_list] (mandatory): detected chessboard corners.
 *
 * @subsection In-Out In-Out
 * - \b videoImage [sight::data::image] (optional): image of the chessboard, can be used to display the detected and/or
 * reprojected points.
 *
 * @subsection Output Output
 * - \b chessboardModel [sight::data::point_list] (optional): 3d chessboard model as described by the `board`
 * preferences.
 *
 * @subsection Configuration Configuration:
 * - \b board (mandatory): preference keys to retrieve the number of squares of the board in
 *                         width and height as well as square's size.
 * - \b drawDetected (optional, default=true): draw the detected points onto the image.
 * - \b drawReprojection (optional, default="true"): draw the reprojected points onto the image.
 * - \b drawReprojectionError (optional, default="true"): draw the reprojection error on the image.
 * - \b distortReprojection (optional, default="true"): whether the 'videoImage' is undistorted, in which case the
 *                                                      reprojected points should not be distorted.
 */
class MODULE_GEOMETRY_VISION_CLASS_API chessboard_reprojection final : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(chessboard_reprojection, service::base);

    ///Constructor
    MODULE_GEOMETRY_VISION_API chessboard_reprojection();

    ///Destructor
    MODULE_GEOMETRY_VISION_API ~chessboard_reprojection() final;

protected:

    /// Configures the service.
    MODULE_GEOMETRY_VISION_API void configuring() final;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void starting() final;

    /// Reprojects the detected using the camera's intrinsics and the tranform. Computes the reprojection's RMSE.
    /// Writes the detected and reprojected points on the video image if there is one.
    MODULE_GEOMETRY_VISION_API void updating() final;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void stopping() final;

    /// Connects camera, transform and detected points modification to the update slot.
    service::connections_t auto_connections() const final;

private:

    using error_computed_signal_t = core::com::signal<void (double)>;

    /// Fetches the chessboard dimension from the preferences and computes the model.
    void update_chessboard_size();

    /// Enables/disabled distorting the reprojected points.
    void toggle_distortion();

    /// Preference key to retrieve the chessboard width.
    std::string m_width_key;

    /// Preference key to retrieve the chessboard height.
    std::string m_height_key;

    /// Preference key to retrieve the size of the chessboard's squares.
    std::string m_square_size_key;

    /// Apply distortion to the reprojected points if true. Undistort the detected points otherwise.
    bool m_distort_reprojection {true};

    /// Draw reprojection points on the image if true.
    bool m_draw_reprojection {true};

    /// Draw detected points on the image if true.
    bool m_draw_detected {true};

    /// Draw the reprojection error on the image if true.
    bool m_draw_reprojection_error {true};

    /// True if this service outputs the chessboard model.
    bool m_has_output_chessboard {false};

    /// Chessboard model.
    std::vector<cv::Point3f> m_chessboard_model;

    /// Signal sent when the reprojection error is computed.
    error_computed_signal_t::sptr m_error_computed_sig;

    static constexpr std::string_view TRANSFORM_INPUT           = "transform";
    static constexpr std::string_view DETECTED_CHESSBOARD_INPUT = "detectedChessboard";
    static constexpr std::string_view CAMERA_INPUT              = "camera";
    static constexpr std::string_view CHESSBOARD_MODEL_OUTPUT   = "chessboardModel";

    data::ptr<data::matrix4, data::access::in> m_transform {this, TRANSFORM_INPUT};
    data::ptr<data::camera, data::access::in> m_camera {this, CAMERA_INPUT};
    data::ptr<data::point_list, data::access::in> m_detected_chessboard {this, DETECTED_CHESSBOARD_INPUT};
    data::ptr<data::image, data::access::inout> m_video_image {this, "videoImage"};
    data::ptr<data::point_list, data::access::out> m_chessboard_model_out {this, CHESSBOARD_MODEL_OUTPUT};
};

} //namespace sight::module::geometry::vision
