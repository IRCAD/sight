/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoCharucoCalibration/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/IController.hpp>

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
 * @brief This service updates CalibrationInfo objects with the points detected from charuco board.
 *
 * This service is used by calling 'detectPoints' slot. It checks on each timeline if points are visible in each
 * frame. Then it adds the detected points and the associated image in the CalibrationInfo.
 *
 * @section Signals Signals
 * - \b charucoBoardDetected(): Emitted when the charucoBoard is detected on the current image.
 * - \b charucoBoardNotDetected(): Emitted when the charucoBoard is not detected on the current image.
 *
 * @section Slots Slots
 * - \b checkPoints(::fwCore::HiResClock::HiResClockType): Try to detect the charucoBoard in the image(s) from the
 * timeline(s) at the given timestamp.
 * - \b detectPoints(): Request to store the current image in the calibration data, if the charucoBoard is detected.
 * - \b updateChessboardSize(): update the parameters of the charucoBoard from preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::videoCharucoCalibration::SCharucoBoardDetector" >
            <in group="timeline">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout group="calInfo">
                <key uid="..." />
                <key uid="..." />
            </inout>
            <inout group="detection">
                <key uid="..." />
                <key uid="..." />
            </inout>
            <board width="CHARUCO_WIDTH" height="CHARUCO_HEIGHT" squareSize="CHARUCO_SQUARE_SIZE"
                   markerSize="CHARUCO_MARKER_SIZE" markerSizeInBits="CHARUCO_MARKER_SIZE_IN_BITS"/>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b timeline [::arData::FrameTL]: timelines containing the images to detect the charucoBoard.
 * @subsection In-Out In-Out:
 * - \b calInfo [::arData::CalibrationInfo]: calibration object where to store the detected images.
 * - \b detection [::fwData::PointList] (optional): detected chessboard points in image coordinates.
 * @subsection Configuration Configuration:
 * - \b board : preferences keys to retrieve the number of squares of the board in width and height, the size of each
 * square in mm, the size of aruco Markers in mm and the size of aruco markers in term of bits (4,5,6 or 7).
 */
class VIDEOCHARUCOCALIBRATION_CLASS_API SCharucoBoardDetector : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SCharucoBoardDetector)(::fwServices::IController))

    /**
     * @name Signals API
     * @{
     */
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHARUCOBOARD_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> CharucoBoardDetectedSignalType;

    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHARUCOBOARD_NOT_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> CharucoBoardNotDetectedSignalType;
    /// @}

    /**
     * @name Slots API
     * @{
     */
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_CHECK_POINTS_SLOT;
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_DETECT_POINTS_SLOT;
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHARUCOBOARD_SIZE_SLOT;
    ///@}

    /// Constructor
    VIDEOCHARUCOCALIBRATION_API SCharucoBoardDetector() noexcept;

    /// Destructor
    VIDEOCHARUCOCALIBRATION_API virtual ~SCharucoBoardDetector() noexcept;

    /**
     * @brief Detect charuco board points
     * @param tl the timeline containing frames displaying the charucoBoard
     * @param timestamp time corresponding to the frame to process in the timeline
     * @return a pointlist where x, y are image coordinates of detected points, and z their ids.
     */
    ::fwData::PointList::sptr detectCharucoBoard(const ::arData::FrameTL::csptr tl,
                                                 const ::fwCore::HiResClock::HiResClockType timestamp,
                                                 ::arData::FrameTL::sptr tlDetection = nullptr);

protected:

    /// Configures the service.
    VIDEOCHARUCOCALIBRATION_API void configuring() override;

    /// Starts the service by initializing size of pointlist.
    VIDEOCHARUCOCALIBRATION_API void starting() override;

    /// Does nothing.
    VIDEOCHARUCOCALIBRATION_API void updating() override;

    /// Does nothing.
    VIDEOCHARUCOCALIBRATION_API void stopping() override;

private:

    /**
     * @brief SLOT : Check if charucoBoard is visible and send corresponding signal
     * @param timestamp timestamp used to gets image frame
     */
    VIDEOCHARUCOCALIBRATION_API void checkPoints(::fwCore::HiResClock::HiResClockType timestamp);

    /**
     * @brief SLOT: Check on each timeline if points are visible in each frame. Then add the detected points and the
     * associated image in the CalibrationInfo.
     */
    VIDEOCHARUCOCALIBRATION_API void detectPoints();

    /**
     * @brief SLOT: Update the charucoBoard size.
     */
    VIDEOCHARUCOCALIBRATION_API void updateCharucoBoardSize();

    /**
     * @brief Create an image from frame timeline
     */
    ::fwData::Image::sptr createImage(arData::FrameTL::csptr tl, ::fwCore::HiResClock::HiResClockType timestamp);

    /// Signal emitted when charucoBoard is detected
    CharucoBoardDetectedSignalType::sptr m_sigCharucoBoardDetected;

    /// Signal emitted when charucoBoard is not detected
    CharucoBoardNotDetectedSignalType::sptr m_sigCharucoBoardNotDetected;

    /// Preference key to retrieve width of the charucoBoard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the charucoBoard used for calibration
    std::string m_heightKey;

    /// Preference key to retrieve square size of the charucoBoard used for calibration
    std::string m_squareSizeKey;

    /// Preference key to retrieve marker size of the charucoBoard used for calibration
    std::string m_markerSizeKey;

    /// Preference key to retrieve marker size in bits.
    std::string m_markerSizeInBitsKey;

    /// Width of the charucoBoard used for calibration
    size_t m_width;

    /// Height of the charucoBoard used for calibration
    size_t m_height;

    /// Used to know if we detected the charucoBoard the last time we check
    bool m_isDetected;

    /// Size of the charucoBoard'square used for calibration
    float m_squareSize;

    /// Size of the aruco's markers used for calibration
    float m_markerSize;

    /// Last valid charucoBoard points for each timeline
    std::vector< ::fwData::PointList::sptr> m_cornerAndIdLists;

    /// Timestamp of the last managed image
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Size in pixels of marker
    int m_markerSizeInBits;

    /// Charuco dictionary
    cv::Ptr< ::cv::aruco::Dictionary > m_dictionary;

    /// Chessboard-aruco board
    ::cv::Ptr< ::cv::aruco::CharucoBoard > m_board;

};

} //namespace videoCharucoCalibration
