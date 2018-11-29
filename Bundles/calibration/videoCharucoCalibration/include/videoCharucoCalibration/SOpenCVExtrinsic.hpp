/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "videoCharucoCalibration/config.hpp"

#include <arServices/ICalibration.hpp>

#include <opencv2/core.hpp>

namespace cv
{
namespace aruco
{
class Dictionary;
class CharucoBoard;

} //namespace cv
} //namespace aruco

namespace videoCharucoCalibration
{
/**
 * @brief   SOpenCVExtrinsic service that computes charuco extrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateCharucoBoardSize() : Received when the Charuco board preferences parameters changes.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoCharucoCalibration::SOpenCVExtrinsic">
            <in key="calibrationInfo1" uid="..." />
            <in key="calibrationInfo2" uid="..." />
            <inout key="cameraSeries" uid="..." />
            <camIndex>...</camIndex>
            <board width="CHARUCO_WIDTH" height="CHARUCO_HEIGHT" squareSize="CHARUCO_SQUARE_SIZE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo1 [::arData::CalibrationInfo]: Data of the first camera used to compute the calibration.
 * - \b calibrationInfo2 [::arData::CalibrationInfo]: Data of the second camera used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [::arData::CameraSeries]: Output calibration.
 * @subsection Configuration Configuration:
 * - \b camIndex (optional, default: 1): index of the camera in cameraSeries used to compute extrinsic matrix
 *      (from camera[0] to camera[index]).
 * - \b board : preferences keys to retrieve the number of squares of the board in width and height, the size of each
 * square in mm (others charuco preferences are not used here).
 */
class VIDEOCHARUCOCALIBRATION_CLASS_API SOpenCVExtrinsic : public ::arServices::ICalibration
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVExtrinsic)(::arServices::ICalibration))

    /// Double changed signal type
    typedef ::fwCom::Signal< void (double) > ErrorComputedSignalType;

    /// Constructor.
    VIDEOCHARUCOCALIBRATION_API SOpenCVExtrinsic() noexcept;

    /// Destructor.
    VIDEOCHARUCOCALIBRATION_API virtual ~SOpenCVExtrinsic() noexcept;

protected:

    /// Configures the service.
    VIDEOCHARUCOCALIBRATION_API void configuring() override;

    /// Does nothing.
    VIDEOCHARUCOCALIBRATION_API void starting() override;

    /// Calls stopping and starting.
    VIDEOCHARUCOCALIBRATION_API void swapping() override;

    /// Computes intrinsic calibration
    VIDEOCHARUCOCALIBRATION_API void updating() override;

    /// Removes connections
    VIDEOCHARUCOCALIBRATION_API void stopping() override;

private:

    /**
     * @brief SLOT: update the charuco board size.
     */
    void updateCharucoBoardSize();

    /**
     * @brief checkDegeneratedConfiguration is a private function only used in updating(),
     * this is used to check if a particular set of detected points is a degenerate configuration.
     * @param _undistortedPoints: point detected in undistorted image.
     * @param _boardCoords: coordinates on CharucoBoard.
     * @param _boardSize: board size.
     * @return true if a degenerateConfiguration is detected, false otherwise.
     */
    bool checkDegeneratedConfiguration(const std::vector< ::cv::Point2f >& _undistortedPoints,
                                       const std::vector< ::cv::Point2f >& _boardCoords,
                                       const cv::Size& _boardSize);

    /// FwId of the first calibrationInfo
    std::string m_calibrationInfo1ID;

    /// FwId of the second calibrationInfo
    std::string m_calibrationInfo2ID;

    /// Preference key to retrieve width of the chessboard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_heightKey;

    /// Preference key to retrieve size of the chessboard'square used for calibration
    std::string m_squareSizeKey;

    /// Width of the chessboard used for calibration
    size_t m_width;

    /// Height of the chessboard used for calibration
    size_t m_height;

    /// Size of the chessboard's square used for calibration
    float m_squareSize;

    /// Index of the camera in cameraSeries used to compute extrinsic matrix (from camera[0] to camera[index]).
    size_t m_camIndex;
};
} // namespace videoCharucoCalibration
