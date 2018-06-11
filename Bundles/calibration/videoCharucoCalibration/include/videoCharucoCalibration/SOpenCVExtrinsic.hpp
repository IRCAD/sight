/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
 * @brief   SOpenCVExtrinsic service that computes extrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateChessboardSize() : Received when the chessboard preferences parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoCalibration::SOpenCVExtrinsic">
            <in key="calibrationInfo1" uid="..." />
            <in key="calibrationInfo2" uid="..." />
            <inout key="cameraSeries" uid="..." />
            <camIndex>...</camIndex>
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo1 [::arData::CalibrationInfo]: Data of the first camera used to compute the calibration.
 * - \b calibrationInfo2 [::arData::CalibrationInfo]: Data of the second camera used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [::arData::CameraSeries]: Output calibration.
 * @subsection Configuration Configuration:
 * - \b camIndex (optional, default: 1): index of the camera in \b cameraSeries used to compute extrinsic matrix
 *      (from camera[0] to camera[index]).
 * - \b board : preference key to retrieve the number of square in 2 dimensions of the chessboard.
 */
class VIDEOCHARUCOCALIBRATION_CLASS_API SOpenCVExtrinsic : public ::arServices::ICalibration
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVExtrinsic)(::arServices::ICalibration));

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
     * @brief SLOT: update the chessboard size.
     */
    void updateCharucoBoardSize();

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

    /// Preference key to retrieve marker size in mm.
    std::string m_markerSizeKey;

    /// Width of the chessboard used for calibration
    size_t m_width;

    /// Height of the chessboard used for calibration
    size_t m_height;

    /// Size of the chessboard'square used for calibration
    float m_squareSize;

    /// Size of the aruco's markers used for calibration
    float m_markerSize;

    /// Index of the camera in cameraSeries used to compute extrinsic matrix (from camera[0] to camera[index]).
    size_t m_camIndex;
};
} // namespace videoCharucoCalibration
