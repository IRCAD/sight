/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <data/calibration_info.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>

#include <geometry/vision/icalibration.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   open_cv_extrinsic service that computes extrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b update_chessboard_size() : Received when the chessboard preferences parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::geometry::vision::open_cv_extrinsic">
            <in key="calibrationInfo1" uid="..." />
            <in key="calibrationInfo2" uid="..." />
            <inout key="cameraSet" uid="..." />
            <out key="matrix" uid="..." />
            <camIndex>...</camIndex>
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo1 [sight::data::calibration_info]: Data of the first camera used to compute the calibration.
 * - \b calibrationInfo2 [sight::data::calibration_info]: Data of the second camera used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [sight::data::camera_set]: Output calibration.
 *  @subsection Output Output:
 * - \b matrix [sight::data::matrix4]: Extrinsic matrix (for export purpose) (optional).
 * @subsection Configuration Configuration:
 * - \b camIndex (optional, default: 1): index of the camera in \b cameraSet used to compute extrinsic matrix
 *      (from camera[0] to camera[index]).
 * - \b board : preference key to retrieve the number of square in 2 dimensions of the chessboard.
 */
class MODULE_GEOMETRY_VISION_CLASS_API open_cv_extrinsic : public sight::geometry::vision::i_calibration
{
public:

    SIGHT_DECLARE_SERVICE(open_cv_extrinsic, sight::geometry::vision::i_calibration);

    /// Double changed signal type
    using error_computed_signal_t = core::com::signal<void (double)>;

    /// Constructor.
    MODULE_GEOMETRY_VISION_API open_cv_extrinsic() noexcept;

    /// Destructor.
    MODULE_GEOMETRY_VISION_API ~open_cv_extrinsic() noexcept override;

protected:

    /// Configures the service.
    MODULE_GEOMETRY_VISION_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void starting() override;

    /// Computes intrinsic calibration
    MODULE_GEOMETRY_VISION_API void updating() override;

    /// Removes connections
    MODULE_GEOMETRY_VISION_API void stopping() override;

private:

    /**
     * @brief SLOT: update the chessboard size.
     */
    void update_chessboard_size();

    /// FwId of the first calibrationInfo
    std::string m_calibration_info1_id;

    /// FwId of the second calibrationInfo
    std::string m_calibration_info2_id;

    /// Preference key to retrieve width of the chessboard used for calibration
    std::string m_width_key;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_height_key;

    /// Preference key to retrieve size of the chessboard'square used for calibration
    std::string m_square_size_key;

    /// Width of the chessboard used for calibration
    unsigned int m_width {11};

    /// Height of the chessboard used for calibration
    unsigned int m_height {8};

    /// Size of the chessboard'square used for calibration
    float m_square_size {20.0};

    /// Index of the camera in cameraSet used to compute extrinsic matrix (from camera[0] to camera[index]).
    std::size_t m_cam_index {1};

    data::ptr<data::calibration_info, data::access::in> m_calibration_info1 {this, "calibrationInfo1"};
    data::ptr<data::calibration_info, data::access::in> m_calibration_info2 {this, "calibrationInfo2"};
    data::ptr<data::camera_set, data::access::inout> m_camera_set {this, "cameraSet"};
    data::ptr<data::matrix4, data::access::out> m_matrix {this, "matrix"};
};

} // namespace sight::module::geometry::vision
