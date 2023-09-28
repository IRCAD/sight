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

#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/Vector.hpp>

#include <geometry/vision/ICalibration.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   SOpenCVIntrinsic service that computes intrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateChessboardSize(unsigned int, unsigned int, float) : Received when the chessboard parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::geometry::vision::SOpenCVIntrinsic">
            <in key="calibrationInfo" uid="..." />
            <inout key="camera" uid="..." />
            <inout key="poseVector" uid="..." />
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo [sight::data::CalibrationInfo]: Data used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [sight::data::Camera]: Output calibration.
 * - \b poseVector [sight::data::Vector] (optional): Camera calibration pose vector
 * @subsection Configuration Configuration:
 * - \b board : preference key to defines the number of square in 2 dimensions of the chessboard.
 */
class MODULE_GEOMETRY_VISION_CLASS_API SOpenCVIntrinsic : public sight::geometry::vision::ICalibration
{
public:

    /// Double changed signal type
    typedef core::com::signal<void (double)> ErrorComputedSignalType;

    SIGHT_DECLARE_SERVICE(SOpenCVIntrinsic, sight::geometry::vision::ICalibration);

    /// Constructor.
    MODULE_GEOMETRY_VISION_API SOpenCVIntrinsic() noexcept;

    /// Destructor.
    MODULE_GEOMETRY_VISION_API ~SOpenCVIntrinsic() noexcept override;

protected:

    /// Configures the service.
    MODULE_GEOMETRY_VISION_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void starting() override;

    /// Computes intrinsic calibration
    MODULE_GEOMETRY_VISION_API void updating() override;

    /// Removes connections
    MODULE_GEOMETRY_VISION_API void stopping() override;

    /**
     * @brief SLOT: update the chessboard size.
     */
    void updateChessboardSize();

private:

    /// Preference key to retrieve width of the chessboard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_heightKey;

    /// Preference key to retrieve size of the chessboard'square used for calibration
    std::string m_squareSizeKey;

    /// Width of the chessboard used for calibration
    unsigned int m_width {11};

    /// Height of the chessboard used for calibration
    unsigned int m_height {8};

    /// Size of the chessboard'square used for calibration
    float m_squareSize {20.0};

    data::ptr<data::CalibrationInfo, data::Access::in> m_calibrationInfo {this, "calibrationInfo"};
    data::ptr<data::Camera, data::Access::inout> m_camera {this, "camera"};
    data::ptr<data::Vector, data::Access::inout> m_poseVector {this, "poseVector"};
};

} // namespace sight::module::geometry::vision
