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

#include <data/CalibrationInfo.hpp>
#include <data/FrameTL.hpp>
#include <data/Image.hpp>
#include <data/PointList.hpp>

#include <service/IController.hpp>

#include <opencv2/core.hpp>

namespace cv
{

namespace aruco
{

class Dictionary;
class CharucoBoard;

} //namespace cv

} //namespace aruco

namespace sight::module::geometry::vision::charuco
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
 * - \b checkPoints(core::HiResClock::HiResClockType): Try to detect the charucoBoard in the image(s) from the
 * timeline(s) at the given timestamp.
 * - \b detectPoints(): Request to store the current image in the calibration data, if the charucoBoard is detected.
 * - \b updateChessboardSize(): update the parameters of the charucoBoard from preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::geometry::vision::charuco::SCharucoBoardDetector" >
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
 * - \b timeline [sight::data::FrameTL]: timelines containing the images to detect the charucoBoard.
 * @subsection In-Out In-Out:
 * - \b calInfo [sight::data::CalibrationInfo]: calibration object where to store the detected images.
 * - \b detection [sight::data::PointList] (optional): detected chessboard points in image coordinates.
 * @subsection Configuration Configuration:
 * - \b board : preferences keys to retrieve the number of squares of the board in width and height, the size of each
 * square in mm, the size of aruco Markers in mm and the size of aruco markers in term of bits (4,5,6 or 7).
 */
class MODULE_GEOMETRY_VISION_CLASS_API SCharucoBoardDetector : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SCharucoBoardDetector, sight::service::IController);

    /**
     * @name Signals API
     * @{
     */
    MODULE_GEOMETRY_VISION_API static const core::com::Signals::SignalKeyType s_CHARUCOBOARD_DETECTED_SIG;
    typedef core::com::Signal<void ()> CharucoBoardDetectedSignalType;

    MODULE_GEOMETRY_VISION_API static const core::com::Signals::SignalKeyType s_CHARUCOBOARD_NOT_DETECTED_SIG;
    typedef core::com::Signal<void ()> CharucoBoardNotDetectedSignalType;
    /// @}

    /**
     * @name Slots API
     * @{
     */
    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_CHECK_POINTS_SLOT;
    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_DETECT_POINTS_SLOT;
    MODULE_GEOMETRY_VISION_API static const core::com::Slots::SlotKeyType s_UPDATE_CHARUCOBOARD_SIZE_SLOT;
    ///@}

    /// Constructor
    MODULE_GEOMETRY_VISION_API SCharucoBoardDetector() noexcept;

    /// Destructor
    MODULE_GEOMETRY_VISION_API virtual ~SCharucoBoardDetector() noexcept;

    /**
     * @brief Detect charuco board points
     * @param tl the timeline containing frames displaying the charucoBoard
     * @param timestamp time corresponding to the frame to process in the timeline
     * @return a pointlist where x, y are image coordinates of detected points, and z their ids.
     */
    data::PointList::sptr detectCharucoBoard(
        const data::FrameTL::csptr tl,
        const core::HiResClock::HiResClockType timestamp,
        data::FrameTL::sptr tlDetection = nullptr
    );

protected:

    /// Configures the service.
    MODULE_GEOMETRY_VISION_API void configuring() override;

    /// Starts the service by initializing size of pointlist.
    MODULE_GEOMETRY_VISION_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void updating() override;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API void stopping() override;

private:

    /**
     * @brief SLOT : Check if charucoBoard is visible and send corresponding signal
     * @param timestamp timestamp used to gets image frame
     */
    MODULE_GEOMETRY_VISION_API void checkPoints(core::HiResClock::HiResClockType timestamp);

    /**
     * @brief SLOT: Check on each timeline if points are visible in each frame. Then add the detected points and the
     * associated image in the CalibrationInfo.
     */
    MODULE_GEOMETRY_VISION_API void detectPoints();

    /**
     * @brief SLOT: Update the charucoBoard size.
     */
    MODULE_GEOMETRY_VISION_API void updateCharucoBoardSize();

    /**
     * @brief Create an image from frame timeline
     */
    data::Image::sptr createImage(const data::FrameTL& tl, core::HiResClock::HiResClockType timestamp);

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
    std::vector<data::PointList::sptr> m_cornerAndIdLists;

    /// Timestamp of the last managed image
    core::HiResClock::HiResClockType m_lastTimestamp;

    /// Size in pixels of marker
    int m_markerSizeInBits;

    /// Charuco dictionary
    cv::Ptr< ::cv::aruco::Dictionary> m_dictionary;

    /// Chessboard-aruco board
    ::cv::Ptr< ::cv::aruco::CharucoBoard> m_board;

    static constexpr std::string_view s_TIMELINE_INPUT    = "timeline";
    static constexpr std::string_view s_CALIBRATION_INOUT = "calInfo";
    static constexpr std::string_view s_DETECTION_INOUT   = "detection";

    data::ptr_vector<data::FrameTL, data::Access::in> m_timeline {this, s_TIMELINE_INPUT};
    data::ptr_vector<data::CalibrationInfo, data::Access::inout> m_calInfo {this, s_CALIBRATION_INOUT};
    data::ptr_vector<data::PointList, data::Access::inout> m_detection {this, s_DETECTION_INOUT};
};

} //namespace sight::module::geometry::vision::charuco
