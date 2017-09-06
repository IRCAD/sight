/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SEXTRINSIC_HPP__
#define __VIDEOCALIBRATION_SEXTRINSIC_HPP__

#include "videoCalibration/ICalibration.hpp"
#include "videoCalibration/config.hpp"

namespace videoCalibration
{
/**
 * @brief   SExtrinsic service that computes extrinsic calibration.
 * @class   SExtrinsic
 */
class VIDEOCALIBRATION_CLASS_API SExtrinsic : public ::videoCalibration::ICalibration
{
public:
    fwCoreServiceClassDefinitionsMacro((SExtrinsic)(::videoCalibration::ICalibration));

    /// Constructor.
    VIDEOCALIBRATION_API SExtrinsic() noexcept;

    /// Destructor.
    VIDEOCALIBRATION_API virtual ~SExtrinsic() noexcept;

protected:

    /**
     * @brief Configures the service.
     * @code{.xml}
       <service impl="::videoCalibration::SExtrinsic" >
            <calibrationInfo1ID> ... </calibrationInfo1ID>
            <calibrationInfo2ID> ... </calibrationInfo2ID>
            <camIndex>...</camIndex>
            <board width="17" height="13" />
       </service>
       @endcode
     * - \b calibrationInfo1ID: FwId of the first calibrationInfo.
     * - \b calibrationInfo2ID: FwId of the second calibrationInfo.
     * - \b camIndex (optional, default: 1): index of the camera in cameraSeries used to compute extrinsic matrix
     *      (from camera[0] to camera[index]).
     * - \b board : defines the number of square in 2 dimensions of the chessboard.
     */
    VIDEOCALIBRATION_API void configuring();

    /// Does nothing.
    VIDEOCALIBRATION_API void starting();

    /// Calls stopping and starting.
    VIDEOCALIBRATION_API void swapping();

    /// Computes intrinsic calibration
    VIDEOCALIBRATION_API void updating();

    /// Removes connections
    VIDEOCALIBRATION_API void stopping();

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
};
} // namespace videoCalibration

#endif // __VIDEOCALIBRATION_SEXTRINSIC_HPP__
