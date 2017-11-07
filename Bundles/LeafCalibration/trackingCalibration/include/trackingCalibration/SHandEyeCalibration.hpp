/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TRACKINGCALIBRATION_SHANDEYECALIBRATION_HPP__
#define __TRACKINGCALIBRATION_SHANDEYECALIBRATION_HPP__

#include "trackingCalibration/config.hpp"

#include <arServices/IRegisterer.hpp>

#include <handEyeCalibration/HandEyeApi.hpp>

namespace trackingCalibration
{
/**
 * @brief Service that will compute the Hand-Eye Calibration from two list of corresponding tracking matrices
 *
 * @code{.xml}
    <service uid="..." type="::trackingCalibration::SHandEyeCalibration" >
        <inout key="matrix" uid="..." />
        <in key="matrixList1" uid="..." />
        <in key="matrixList2" uid="..." />
    </service>
   @endcode
 * @subsection InOut InOut:
 * - \b matrix uid of the matrix
 *
 * @subsection Inputs Inputs:
 * - \b matrixList1 uid of the matrix List 1
 * - \b matrixList2 uid of the matrix List 2
 */
class TRACKINGCALIBRATION_CLASS_API SHandEyeCalibration : public ::arServices::IRegisterer
{

public:
    fwCoreServiceClassDefinitionsMacro((SHandEyeCalibration)(arServices::IRegisterer));

    /**
     * @brief Constructor.
     */
    TRACKINGCALIBRATION_API SHandEyeCalibration() noexcept;

    /**
     * @brief Destructor.
     */
    TRACKINGCALIBRATION_API virtual ~SHandEyeCalibration() noexcept;

protected:

    TRACKINGCALIBRATION_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    TRACKINGCALIBRATION_API void starting() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    TRACKINGCALIBRATION_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    TRACKINGCALIBRATION_API void stopping() override;

    /// Register matrix slot
    TRACKINGCALIBRATION_API virtual void computeRegistration(::fwCore::HiResClock::HiResClockType timestamp) override;

private:

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Mutex used to lock access of update
    ::fwCore::mt::Mutex m_mutex;

    /// Connections
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Api to the Hand-Eye calibration
    UPTR(::handEyeCalibration::HandEyeApi) m_handEyeApi;

};

}

#endif // __TRACKINGCALIBRATION_SHANDEYECALIBRATION_HPP__
