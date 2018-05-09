/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
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
 * @brief Service that computes the
 *
 *
 * @section Signals Signals

 *
 * @section Slots Slots
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::trackingCalibration::SToolCalibration" >
        <out key="matrix" uid="..." />
        <in key="pointList" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b matrixVector1 [::fwData::Vector]: vector of matrices corresponding to {Ai, Aj, ...} matrices
 * - \b matrixVector2 [::fwData::Vector]: vector of matrices corresponding to {Bi, Bj, ...} matrices
 *
 * @subsection InOut InOut
 * - \b matX [::fwData::TransformationMatrix3D]: Hand-Eye calibration matrix X
 * - \b matZ [::fwData::TransformationMatrix3D]: Hand-Eye calibration matrix Z
 *
 */
class TRACKINGCALIBRATION_CLASS_API SToolCalibration : public ::fwServices::IRegisterer
{

public:
    fwCoreServiceClassDefinitionsMacro((SToolCalibration)(::fwServices::IRegisterer));

    /**
     * @brief Constructor.
     */
    TRACKINGCALIBRATION_API SToolCalibration() noexcept;

    /**
     * @brief Destructor.
     */
    TRACKINGCALIBRATION_API virtual ~SToolCalibration() noexcept;

    /// Matrices X and Y computed signal type
    typedef ::fwCom::Signal< void (double) > MatrixXComputedSignalType;
    typedef ::fwCom::Signal< void (double) > MatrixZComputedSignalType;

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

    bool m_hasOutputCenter {false};
};

}
