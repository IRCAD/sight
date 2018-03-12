/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "trackingCalibration/config.hpp"

#include <fwServices/IRegisterer.hpp>

#include <handEyeCalibration/HandEyeApi.hpp>

namespace trackingCalibration
{
/**
 * @brief Service that computes the Hand-Eye Calibration matrix X from two vectors of corresponding tracking matrices
 *{Ai, Aj, ...} and {Bi, Bj, ...}
 *
 * @see https://github.com/jhu-lcsr/handeye_calib_camodocal
 *
 * @subsection Slots Slots
 * - \b setMovingCamera(bool): configure the service to compute a hand-eye calibration for a moving or fixed camera
 *
 * @code{.xml}
    <service uid="..." type="::trackingCalibration::SHandEyeCalibration" >
        <inout key="matX" uid="..." />
        <inout key="matZ" uid="..." />
        <in key="matrixVector1" uid="..." />
        <in key="matrixVector2" uid="..." />
        <config movingCamera="true|false" />
    </service>
   @endcode
 * @subsection InOut InOut
 * - \b matX [::fwData::TransformationMatrix3D]: Hand-Eye calibration matrix X
 * - \b matZ [::fwData::TransformationMatrix3D]: Hand-Eye calibration matrix Z
 *
 * @subsection Input Input
 * - \b matrixVector1 [::fwData::Vector]: vector of matrices corresponding to {Ai, Aj, ...} matrices
 * - \b matrixVector2 [::fwData::Vector]: vector of matrices corresponding to {Bi, Bj, ...} matrices
 *
 *  * @subsection Configuration Configuration
 * - \b movingCamera (optional, values:true|false, default: false): defines if the camera is a moving element of our
 * hand eye system (i.e. fixed chessboard)
 */
class TRACKINGCALIBRATION_CLASS_API SHandEyeCalibration : public ::fwServices::IRegisterer
{

public:
    fwCoreServiceClassDefinitionsMacro((SHandEyeCalibration)(::fwServices::IRegisterer));

    /**
     * @brief Constructor.
     */
    TRACKINGCALIBRATION_API SHandEyeCalibration() noexcept;

    /**
     * @brief Destructor.
     */
    TRACKINGCALIBRATION_API virtual ~SHandEyeCalibration() noexcept;

protected:

    /// Configures the service
    TRACKINGCALIBRATION_API void configuring() override;

    /// Does nothing
    TRACKINGCALIBRATION_API void starting() override;

    /// Does nothing
    TRACKINGCALIBRATION_API void updating() override;

    /// Does nothing
    TRACKINGCALIBRATION_API void stopping() override;

    /// Registers matrix slot
    TRACKINGCALIBRATION_API virtual void computeRegistration(::fwCore::HiResClock::HiResClockType timestamp) override;

private:

    /// SLOT: sets moving or fixed camera mode
    void setMovingCamera(bool movingCamera);

    /// Mutex used to lock access of update
    ::fwCore::mt::Mutex m_mutex;

    /// Whether or not the camera moves
    bool m_movingCamera;
};

}
