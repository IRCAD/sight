/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SOPENCVEXTRINSIC_HPP__
#define __VIDEOCALIBRATION_SOPENCVEXTRINSIC_HPP__

#include "videoCalibration/ICalibration.hpp"
#include "videoCalibration/config.hpp"

namespace videoCalibration
{
/**
 * @brief   SOpenCVExtrinsic service that computes extrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateChessboardSize(unsigned int, unsigned int, float) : Received when the chessboard parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoCalibration::SOpenCVExtrinsic">
            <in key="calibrationInfo1" uid="..." />
            <in key="calibrationInfo2" uid="..." />
            <inout key="cameraSeries" uid="..." />
            <camIndex>...</camIndex>
            <board width="17" height="13" />
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
 * - \b board : defines the number of square in 2 dimensions of the chessboard.
 */
class VIDEOCALIBRATION_CLASS_API SOpenCVExtrinsic : public ::videoCalibration::ICalibration
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVExtrinsic)(::videoCalibration::ICalibration));

    typedef ::fwCom::Slot <void (unsigned int, unsigned int, float)> UpdateChessboardSizeSlotType;

    /// Constructor.
    VIDEOCALIBRATION_API SOpenCVExtrinsic() throw ();

    /// Destructor.
    VIDEOCALIBRATION_API virtual ~SOpenCVExtrinsic() throw ();

protected:

    /// Configures the service.
    VIDEOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() throw (fwTools::Failed);

    /// Calls stopping and starting.
    VIDEOCALIBRATION_API void swapping() throw (fwTools::Failed);

    /// Computes intrinsic calibration
    VIDEOCALIBRATION_API void updating() throw (fwTools::Failed);

    /// Removes connections
    VIDEOCALIBRATION_API void stopping() throw (fwTools::Failed);

private:

    /**
     * @brief SLOT: update the chessboard size.
     * @param width chessboard's width expresses by the number of square.
     * @param height chessboard's height expresses by the number of square.
     */
    void updateChessboardSize(unsigned int width, unsigned int height, float squareSize);

    /// FwId of the first calibrationInfo
    std::string m_calibrationInfo1ID;

    /// FwId of the second calibrationInfo
    std::string m_calibrationInfo2ID;

    /// Width of the chessboard used for calibration
    unsigned int m_width;

    /// Height of the chessboard used for calibration
    unsigned int m_height;

    /// Size of the chessboard'square used for calibration
    float m_squareSize;

    /// Index of the camera in cameraSeries used to compute extrinsic matrix (from camera[0] to camera[index]).
    size_t m_camIndex;
};
} // namespace videoCalibration

#endif // __VIDEOCALIBRATION_SOPENCVEXTRINSIC_HPP__
