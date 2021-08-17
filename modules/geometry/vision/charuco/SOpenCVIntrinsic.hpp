/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <service/ICalibration.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

namespace sight::module::geometry::vision::charuco
{

/**
 * @brief   SOpenCVIntrinsic service that computes intrinsic charuco calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateCharucoBoardSize(unsigned int, unsigned int, float) : Received when the Charuco board parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::geometry::vision::charuco::SOpenCVIntrinsic">
            <in key="calibrationInfo" uid="..." />
            <inout key="camera" uid="..." />
            <inout key="poseVector" uid="..." />
            <board width="CHARUCO_WIDTH" height="CHARUCO_HEIGHT" squareSize="CHARUCO_SQUARE_SIZE"
                   markerSize="CHARUCO_MARKER_SIZE" markerSizeInBits="CHARUCO_MARKER_SIZE_IN_BITS"/>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo [sight::data::CalibrationInfo]: Data used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [sight::data::Camera]: Output calibration.
 * - \b poseVector [sight::data::Vector] (optional): Camera calibration pose vector
 * @subsection Configuration Configuration:
 * - \b board : preferences keys to retrieve the number of squares of the board in width and height, the size of each
 * square in mm, the size of aruco markers in mm and the size of aruco markers in term of bits (4,5,6 or 7).
 */
class MODULE_GEOMETRY_VISION_CLASS_API SOpenCVIntrinsic : public service::ICalibration
{
public:

    /// Double changed signal type
    typedef core::com::Signal<void (double)> ErrorComputedSignalType;

    SIGHT_DECLARE_SERVICE(SOpenCVIntrinsic, sight::service::ICalibration);

    /// Constructor.
    MODULE_GEOMETRY_VISION_API SOpenCVIntrinsic() noexcept;

    /// Destructor.
    MODULE_GEOMETRY_VISION_API virtual ~SOpenCVIntrinsic() noexcept;

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
    void updateCharucoBoardSize();

private:

    /// Preference key to retrieve width of the chessboard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_heightKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_squareSizeKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_markerSizeKey;

    /// Preference key to retrieve marker size in bits.
    std::string m_markerSizeInBitsKey;

    /// Width of the chessboard used for calibration
    size_t m_width;

    /// Height of the chessboard used for calibration
    size_t m_height;

    /// Size of the chessboard'square used for calibration
    float m_squareSize;

    /// Size of the aruco's markers used for calibration
    float m_markerSize;

    /// Size in pixels of marker
    int m_markerSizeInBits;

    /// Charuco dictionary
    ::cv::Ptr< ::cv::aruco::Dictionary> m_dictionary;

    /// Chessboard-aruco board
    ::cv::Ptr< ::cv::aruco::CharucoBoard> m_board;
};

} // namespace sight::module::geometry::vision::charuco
