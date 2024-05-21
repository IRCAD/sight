/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <data/calibration_info.hpp>
#include <data/camera.hpp>
#include <data/vector.hpp>

#include <geometry/vision/calibrator.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   open_cv_intrinsic service that computes intrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b update_chessboard_size(unsigned int, unsigned int, float) : Received when the chessboard parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::geometry::vision::open_cv_intrinsic">
            <in key="calibrationInfo" uid="..." />
            <inout key="camera" uid="..." />
            <inout key="poseVector" uid="..." />
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo [sight::data::calibration_info]: Data used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [sight::data::camera]: Output calibration.
 * - \b poseVector [sight::data::vector] (optional): Camera calibration pose vector
 * @subsection Configuration Configuration:
 * - \b board : preference key to defines the number of square in 2 dimensions of the chessboard.
 */
class open_cv_intrinsic : public sight::geometry::vision::calibrator
{
public:

    /// Double changed signal type
    using error_computed_signal_t = core::com::signal<void (double)>;

    SIGHT_DECLARE_SERVICE(open_cv_intrinsic, sight::geometry::vision::calibrator);

    /// Constructor.
    open_cv_intrinsic() noexcept;

    /// Destructor.
    ~open_cv_intrinsic() noexcept override;

protected:

    /// Configures the service.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Computes intrinsic calibration
    void updating() override;

    /// Removes connections
    void stopping() override;

    /**
     * @brief SLOT: update the chessboard size.
     */
    void update_chessboard_size();

private:

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

    data::ptr<data::calibration_info, data::access::in> m_calibration_info {this, "calibrationInfo"};
    data::ptr<data::camera, data::access::inout> m_camera {this, "camera"};
    data::ptr<data::vector, data::access::inout> m_pose_vector {this, "poseVector"};
};

} // namespace sight::module::geometry::vision
