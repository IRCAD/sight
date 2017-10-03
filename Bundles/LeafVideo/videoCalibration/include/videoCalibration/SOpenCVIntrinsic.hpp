/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SOPENCVINTRINSIC_HPP__
#define __VIDEOCALIBRATION_SOPENCVINTRINSIC_HPP__

#include "videoCalibration/config.hpp"
#include "videoCalibration/ICalibration.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

namespace videoCalibration
{

/**
 * @brief   SOpenCVIntrinsic service that computes intrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateChessboardSize(unsigned int, unsigned int, float) : Received when the chessboard parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoCalibration::SOpenCVIntrinsic">
            <in key="calibrationInfo" uid="..." />
            <inout key="camera" uid="..." />
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo [::arData::CalibrationInfo]: Data used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [::arData::Camera]: Output calibration.
 * @subsection Configuration Configuration:
 * - \b board : preference key to defines the number of square in 2 dimensions of the chessboard.
 */
class VIDEOCALIBRATION_CLASS_API SOpenCVIntrinsic : public ::videoCalibration::ICalibration
{
public:
    fwCoreServiceClassDefinitionsMacro((SOpenCVIntrinsic)(::videoCalibration::ICalibration));

    /// Constructor.
    VIDEOCALIBRATION_API SOpenCVIntrinsic() noexcept;

    /// Destructor.
    VIDEOCALIBRATION_API virtual ~SOpenCVIntrinsic() noexcept;

protected:

    /// Configures the service.
    VIDEOCALIBRATION_API void configuring() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() override;

    /// Calls stopping and starting.
    VIDEOCALIBRATION_API void swapping() override;

    /// Computes intrinsic calibration
    VIDEOCALIBRATION_API void updating() override;

    /// Removes connections
    VIDEOCALIBRATION_API void stopping() override;

    /**
     * @brief SLOT: update the chessboard size.
     */
    void updateChessboardSize();

private:

    /// Preference key to retrieve width of the chessboard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_heightKey;

    /// Preference key to retrieve size of the chessboard'square used for calibration
    std::string m_squareSizeKey;

    /// Width of the chessboard used for calibration
    unsigned int m_width;

    /// Height of the chessboard used for calibration
    unsigned int m_height;

    /// Size of the chessboard'square used for calibration
    float m_squareSize;
};

} // namespace videoCalibration

#endif // __VIDEOCALIBRATION_SOPENCVINTRINSIC_HPP__
