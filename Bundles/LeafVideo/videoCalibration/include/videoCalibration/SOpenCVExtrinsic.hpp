/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SOPENCVEXTRINSIC_HPP__
#define __VIDEOCALIBRATION_SOPENCVEXTRINSIC_HPP__

#include "videoCalibration/ICalibration.hpp"
#include "videoCalibration/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

namespace videoCalibration
{
/**
 * @brief   SOpenCVExtrinsic service that computes extrinsic calibration with openCV.
 * @class   SOpenCVExtrinsic
 */
class VIDEOCALIBRATION_CLASS_API SOpenCVExtrinsic : public ::videoCalibration::ICalibration
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVExtrinsic)(::videoCalibration::ICalibration));

    typedef ::fwCom::Slot <void (int, int)> UpdateChessboardSizeSlotType;

    /// Constructor.
    VIDEOCALIBRATION_API SOpenCVExtrinsic() throw ();

    /// Destructor.
    VIDEOCALIBRATION_API virtual ~SOpenCVExtrinsic() throw ();

protected:

    /**
     * @brief Configures the service.
     * @verbatim
       <service impl="::videoCalibration::SExtrinsic" >
            <calibrationInfo1ID> ... </calibrationInfo1ID>
            <calibrationInfo2ID> ... </calibrationInfo2ID>
            <camIndex>...</camIndex>
            <board width="17" height="13" />
       </service>
       @endverbatim
     * - \b calibrationInfo1ID: FwId of the first calibrationInfo.
     * - \b calibrationInfo2ID: FwId of the second calibrationInfo.
     * - \b camIndex (optional, default: 1): index of the camera in cameraSeries used to compute extrinsic matrix
     *      (from camera[0] to camera[index]).
     * - \b board : defines the number of square in 2 dimensions of the chessboard.
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

    /**
     * @brief SLOT: update the chessboard size.
     * @param width chessboard's width expresses by the number of square.
     * @param height chessboard's height expresses by the number of square.
     */
    void updateChessboardSize(const int width, const int height);

private:

    /// FwId of the first calibrationInfo
    std::string m_calibrationInfo1ID;

    /// FwId of the second calibrationInfo
    std::string m_calibrationInfo2ID;

    /// Width of the chessboard used for calibration
    unsigned int m_width;

    /// Height of the chessboard used for calibration
    unsigned int m_height;

    /// Index of the camera in cameraSeries used to compute extrinsic matrix (from camera[0] to camera[index]).
    size_t m_camIndex;

    /// Slot that calls update chessboard size method
    UpdateChessboardSizeSlotType::sptr m_slotUpdateChessboardSize;
};
} // namespace videoCalibration

#endif // __VIDEOCALIBRATION_SOPENCVEXTRINSIC_HPP__
