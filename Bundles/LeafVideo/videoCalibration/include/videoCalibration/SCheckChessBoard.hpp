/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SCHECKCHESSBOARD_HPP__
#define __VIDEOCALIBRATION_SCHECKCHESSBOARD_HPP__


#include "videoCalibration/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Image.hpp>
#include <extData/FrameTL.hpp>

#include <fwServices/IController.hpp>

#include <vector>
#include <string>

namespace videoCalibration
{
/**
 * @class SCheckChessBoard
 * @brief This services listen update on frame timeline and check if the chessboard is visible.
 *
 * If chessboard is detected, is send 'chessboardDetected', else is send 'chessboardNotDetected'.
 */
class VIDEOCALIBRATION_CLASS_API SCheckChessBoard : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SCheckChessBoard)(::fwServices::IController));

    VIDEOCALIBRATION_API SCheckChessBoard() throw ();

    VIDEOCALIBRATION_API ~SCheckChessBoard() throw ();

    /**@name Signals API
     * @{
     */
    VIDEOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHESSBOARD_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> ChessboardDetectedSignalType;

    VIDEOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHESSBOARD_NOT_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> ChessboardNotDetectedSignalType;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef fwCom::Slot <void (int, int, float)> UpdateChessboardSizeSlotType;

    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_DETECT_POINTS_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> DetectPointsSlotType;

    /**
     * @brief SLOT: update the chessboard size.
     * @param width chessboard's width expresses by the number of square.
     * @param height chessboard's height expresses by the number of square.
     */
    void updateChessboardSize(const int width, const int height, const float squareSize);
    ///@}

protected:


    /**
     * @brief Configures the service
     *
     * @verbatim
        <service uid="..." impl="::videoCalibration::SCheckChessBoard" autoConnect="no">
            <calibration timeline="timeline1" />
            <calibration timeline="timeline2" />
            <board width="17" height="13" />
        </service>
       @endverbatim
     * - \b timeline : key of frame timeline in the composite.
     * - \b board : number of square in board width and height
     */
    VIDEOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() throw (fwTools::Failed);

    /// Calls stopping and starting.
    VIDEOCALIBRATION_API void swapping() throw (fwTools::Failed);

    /// Computes intrinsic calibration
    VIDEOCALIBRATION_API void updating() throw (fwTools::Failed);

    /// Removes connections
    VIDEOCALIBRATION_API void stopping() throw (fwTools::Failed);

    /// SLOT : check if chessboard is visible and send corresponding signal
    void detectPoints(::fwCore::HiResClock::HiResClockType timestamp);

    /// Creates an image from frame timeline
    ::fwData::Image::sptr createImage(::extData::FrameTL::sptr tl, ::fwCore::HiResClock::HiResClockType timestamp);

private:

    std::vector< std::string > m_timeLineKeys; ///< Keys of frame timeline

    /// Signal emitted when chessboard is detected
    ChessboardDetectedSignalType::sptr m_sigChessboardDetected;

    /// Signal emitted when chessboard is not detected
    ChessboardNotDetectedSignalType::sptr m_sigChessboardNotDetected;

    // Slot that calls update chessboard size method
    UpdateChessboardSizeSlotType::sptr m_slotUpdateChessboardSize;

    /// Slot to call detect
    DetectPointsSlotType::sptr m_slotDetectPoints;

    /// Timestamp of the last managed image
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Width of the chessboard used for calibration
    size_t m_width;

    /// Height of the chessboard used for calibration
    size_t m_height;
};

} //namespace videoCalibration



#endif //__VIDEOCALIBRATION_SCHECKCHESSBOARD_HPP__
