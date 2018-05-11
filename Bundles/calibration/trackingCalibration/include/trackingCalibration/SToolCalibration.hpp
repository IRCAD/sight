/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "trackingCalibration/config.hpp"

#include <fwServices/IRegisterer.hpp>

namespace trackingCalibration
{
/**
 * @brief Service that computes a pivot calibration
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::trackingCalibration::SToolCalibration" >
        <in key="matricesVector" uid="..." />
        <out key="matrixCalibration" uid="..." />
        <out key="matrixCenter" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b matricesVector [::fwData::Vector]: vector of matrices corresponding to points on a sphere
 *
 * @subsection Output Output
 * - \b matrixCalibration [::fwData::TransformationMatrix3D]: tool calibration
 * - \b matrixCenter [::fwData::TransformationMatrix3D] (optional): matrix representing the point around which the tool
 * is moved
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
