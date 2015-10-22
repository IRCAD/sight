/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__
#define __VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__

#include "videoCalibration/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>
#include <extData/FrameTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>

#include <vector>
#include <string>

namespace videoCalibration
{

/**
 * @class SChessBoardDetector
 * @brief This service updates CalibrationInfo objects with the points detected from chessboard.
 *
 * This service is used by calling 'detectPoints' slot. It checks on each timeline if points are visible in each
 * frame. Then it add the detected points and the associated image in the CalibrationInfo.
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
    VIDEOCALIBRATION_API SChessBoardDetector() throw ();

    /// Destructor
    VIDEOCALIBRATION_API ~SChessBoardDetector() throw ();

protected:

    /**
     * @brief Configures the service
     *
     * @verbatim
        <service uid="..." impl="::videoCalibration::SChessBoardDetector" autoConnect="no">
            <calibration timeline="timeline1" calInfo="calInfo1" />
            <calibration timeline="timeline2" calInfo="calInfo2" />
            <board width="17" height="13" />
        </service>
       @endverbatim
     * - \b timeline : key of frame timeline in the composite.
     * - \b calInfo : key of the CalibrationInfo in the composite
     * - \b board : number of square in board width and height
     */
    VIDEOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void swapping() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void updating() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void stopping() throw (fwTools::Failed);

    /**
     * @name Slots API
     * @{
     */

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
     * @param width chessboard's width expresses by the number of square.
     * @param height chessboard's height expresses by the number of square.
     */
    VIDEOCALIBRATION_API void updateChessboardSize(const int width, const int height);
    ///@}

private:

    /**
     * @brief Detect chessboard points
     * @param image the frame displaying the chessboard
     * @param xDim the number of chessboard squares horizontally
     * @param yDim the number of chessboard squares vertically
     * @return The list of chessboard points or NULL if no points are detected
     */
    static SPTR(::fwData::PointList) detectChessboard(::extData::FrameTL::sptr tl,
                                                      ::fwCore::HiResClock::HiResClockType timestamp,
                                                      size_t xDim, size_t yDim);

    /**
     * @brief Creates an image from frame timeline
     */
    ::fwData::Image::sptr createImage(::extData::FrameTL::sptr tl, ::fwCore::HiResClock::HiResClockType timestamp);

    /// Keys of timeline in the composite
    std::vector< std::string > m_timeLineKeys;

    /// Keys of CalibrationInfo in the composite
    std::vector< std::string > m_calInfoKeys;

    /// Signal emitted when chessboard is detected
    ChessboardDetectedSignalType::sptr m_sigChessboardDetected;

    /// Signal emitted when chessboard is not detected
    ChessboardNotDetectedSignalType::sptr m_sigChessboardNotDetected;

    /// Width of the chessboard used for calibration
    size_t m_width;

    /// Height of the chessboard used for calibration
    size_t m_height;

    /// Used to know if we detected the chessboard the last time we check
    bool m_isDetected;

    /// Frame time line objects
    std::vector< ::extData::FrameTL::sptr> m_frameTLs;

    /// Last valid chessboard points for each timeline
    std::vector< ::fwData::PointList::sptr> m_pointsLists;

    /// Timestamp of the last managed image
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
};

} //namespace videoCalibration



#endif //__VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__
