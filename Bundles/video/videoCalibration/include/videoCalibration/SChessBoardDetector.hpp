/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__
#define __VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__

#include "videoCalibration/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/IController.hpp>

#include <string>
#include <vector>

namespace videoCalibration
{

/**
 * @brief This service updates CalibrationInfo objects with the points detected from chessboard.
 *
 * This service is used by calling 'detectPoints' slot. It checks on each timeline if points are visible in each
 * frame. Then it adds the detected points and the associated image in the CalibrationInfo.
 *
 * @section Signals Signals
 * - \b chessboardDetected(): Emitted when the chessboard is detected on the current image.
 * - \b chessboardNotDetected(): Emitted when the chessboard is not detected on the current image.
 *
 * @section Slots Slots
 * - \b checkPoints(::fwCore::HiResClock::HiResClockType): Try to detect the chessboard in the image(s) from the
 * timeline(s) at the given timestamp.
 * - \b detectPoints(): Request to store the current image in the calibration data, if the chessboard is detected.
 * - \b updateChessboardSize(): update the parameters of the chessboard from preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." impl="::videoCalibration::SChessBoardDetector" >
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
           <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b timeline [::arData::FrameTL]: timelines containing the images to detect the chessboard.
 * @subsection In-Out In-Out:
 * - \b calInfo [::arData::CalibrationInfo]: calibration object where to store the detected images.
 * - \b detection [::arData::FrameTL] (optional): timelines filled with images on which detected chessboard are drawn
 * @subsection Configuration Configuration:
 * - \b board : preference key to retrieve the number of squares of the board in width and height.
 */
class VIDEOCALIBRATION_CLASS_API SChessBoardDetector : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SChessBoardDetector)(::fwServices::IController));

    /**
     * @name Signals API
     * @{
     */
    VIDEOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHESSBOARD_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> ChessboardDetectedSignalType;

    VIDEOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHESSBOARD_NOT_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> ChessboardNotDetectedSignalType;
    /// @}

    /**
     * @name Slots API
     * @{
     */
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_CHECK_POINTS_SLOT;
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_DETECT_POINTS_SLOT;
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT;
    ///@}

    /// Constructor
    VIDEOCALIBRATION_API SChessBoardDetector() noexcept;

    /// Destructor
    VIDEOCALIBRATION_API ~SChessBoardDetector() noexcept;

protected:

    /// Configure the service.
    VIDEOCALIBRATION_API void configuring() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void updating() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void stopping() override;

private:

    /**
     * @brief SLOT : check if chessboard is visible and send corresponding signal
     * @param timestamp timestamp used to gets image frame
     */
    VIDEOCALIBRATION_API void checkPoints(::fwCore::HiResClock::HiResClockType timestamp);

    /**
     * @brief SLOT: Checks on each timeline if points are visible in each frame. Then it add the detected points and the
     * associated image in the CalibrationInfo.
     */
    VIDEOCALIBRATION_API void detectPoints();

    /**
     * @brief SLOT: update the chessboard size.
     */
    VIDEOCALIBRATION_API void updateChessboardSize();

    /**
     * @brief Detect chessboard points
     * @param tl the timeline containing frames displaying the chessboard
     * @param timestamp time corresponding to the frame to process in the timeline
     * @param xDim the number of chessboard squares horizontally
     * @param yDim the number of chessboard squares vertically
     * @param tlDetection the optional result timeline filled with processed frames
     * @return The list of chessboard points or NULL if no points are detected
     */
    static SPTR(::fwData::PointList) detectChessboard(::arData::FrameTL::csptr tl,
                                                      ::fwCore::HiResClock::HiResClockType timestamp,
                                                      size_t xDim, size_t yDim,
                                                      ::arData::FrameTL::sptr tlDetection = 0);

    /**
     * @brief Creates an image from frame timeline
     */
    ::fwData::Image::sptr createImage(arData::FrameTL::csptr tl, ::fwCore::HiResClock::HiResClockType timestamp);

    /// Signal emitted when chessboard is detected
    ChessboardDetectedSignalType::sptr m_sigChessboardDetected;

    /// Signal emitted when chessboard is not detected
    ChessboardNotDetectedSignalType::sptr m_sigChessboardNotDetected;

    /// Preference key to retrieve width of the chessboard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_heightKey;

    /// Width of the chessboard used for calibration
    size_t m_width;

    /// Height of the chessboard used for calibration
    size_t m_height;

    /// Used to know if we detected the chessboard the last time we check
    bool m_isDetected;

    /// Last valid chessboard points for each timeline
    std::vector< ::fwData::PointList::sptr> m_pointsLists;

    /// Timestamp of the last managed image
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
};

} //namespace videoCalibration

#endif //__VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__
