/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoCharucoCalibration/config.hpp"

#include <arServices/ICalibration.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

namespace videoCharucoCalibration
{

/**
 * @brief   SOpenCVIntrinsic service that computes intrinsic charuco calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateCharucoBoardSize(unsigned int, unsigned int, float) : Received when the Charuco board parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoCharucoCalibration::SOpenCVIntrinsic">
            <in key="calibrationInfo" uid="..." />
            <inout key="camera" uid="..." />
            <inout key="poseVector" uid="..." />
            <board width="CHARUCO_WIDTH" height="CHARUCO_HEIGHT" squareSize="CHARUCO_SQUARE_SIZE"
                   markerSize="CHARUCO_MARKER_SIZE" markerSizeInBits="CHARUCO_MARKER_SIZE_IN_BITS"/>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo [::arData::CalibrationInfo]: Data used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [::arData::Camera]: Output calibration.
 * - \b poseVector [::fwData::Vector] (optional): Camera calibration pose vector
 * @subsection Configuration Configuration:
 * - \b board : preferences keys to retrieve the number of squares of the board in width and height, the size of each
 * square in mm, the size of aruco markers in mm and the size of aruco markers in term of bits (4,5,6 or 7).
 */
class VIDEOCHARUCOCALIBRATION_CLASS_API SOpenCVIntrinsic : public ::arServices::ICalibration
{
public:

    /// Double changed signal type
    typedef ::fwCom::Signal< void (double) > ErrorComputedSignalType;

    fwCoreServiceClassDefinitionsMacro((SOpenCVIntrinsic)(::arServices::ICalibration))

    /// Constructor.
    VIDEOCHARUCOCALIBRATION_API SOpenCVIntrinsic() noexcept;

    /// Destructor.
    VIDEOCHARUCOCALIBRATION_API virtual ~SOpenCVIntrinsic() noexcept;

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
    ::cv::Ptr< ::cv::aruco::Dictionary > m_dictionary;

    /// Chessboard-aruco board
    ::cv::Ptr< ::cv::aruco::CharucoBoard > m_board;
};

} // namespace videoCharucoCalibration
