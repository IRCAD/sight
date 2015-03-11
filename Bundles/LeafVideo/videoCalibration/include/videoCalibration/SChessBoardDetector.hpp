/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__
#define __VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__


#include <vector>
#include <string>

#include <fwData/Image.hpp>
#include <extData/FrameTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>

#include "videoCalibration/config.hpp"

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
     * @name Slots API
     * @{
     */
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_DETECTPTS_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> DetectPtsSlotType;

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
     * @brief SLOT: Checks on each timeline if points are visible in each frame. Then it add the detected points and the
     * associated image in the CalibrationInfo.
     * @param timestamp timestamp used to gets image frame
     */
    void detectPoints(::fwCore::HiResClock::HiResClockType timestamp);

    /// Returns a fwData::Image from FrameTL at the given timestamp
    ::fwData::Image::sptr createImage(::extData::FrameTL::sptr tl, ::fwCore::HiResClock::HiResClockType timestamp);

private:

    std::vector< std::string > m_timeLineKeys; ///< Keys of timeline in the composite
    std::vector< std::string > m_calInfoKeys; ///< Keys of CalibrationInfo in the composite

    /// Slot that calls detectPoints method
    DetectPtsSlotType::sptr m_slotDetectPts;


    /// Width of the chessboard used for calibration
    size_t m_width;

    /// Height of the chessboard used for calibration
    size_t m_height;
};

} //namespace videoCalibration



#endif //__VIDEOCALIBRATION_SCHESSBOARDDETECTOR_HPP__
