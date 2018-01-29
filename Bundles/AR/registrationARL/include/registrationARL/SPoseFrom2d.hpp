/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "registrationARL/config.hpp"

#include <fwCore/HiResClock.hpp>
#include <fwCore/mt/types.hpp>

#include <fwServices/IRegisterer.hpp>
#include <fwServices/macros.hpp>

#include <arlcore/Point.h>
#include <arlcore/Reconst3D.h>

namespace registrationARL
{

/**
 * @brief   SPoseFrom2d Class used to compute the 3d pose of a object using 2d points.
 *
 * @section Slots Slots
 * - \b computeRegistration(::fwCore::HiResClock::HiResClockType timestamp) : computes the registration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::registrationARL::SPoseFrom2d">
         <in group="markerTL" autoConnect="yes">
             <key uid="markerTL1" />
             <key uid="markerTL2" />
         </in>
         <in group="camera">
             <key uid="cam1" />
             <key uid="cam2" />
         </in>
         <in key="extrinsic" uid="matrix1" />
         <inout key="matrixTL" uid="matrixTL1" />
         <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerTL [::arData::MarkerTL]: timeline for markers.
 * - \b camera [::arData::Camera]: calibrated cameras.
 * - \b extrinsic [::fwData::TransformationMatrix3D]: extrinsic matrix, only used if you have two cameras configured.
 * @subsection In-Out In-Out
 * - \b matrixTL [::arData::MatrixTL]: timeline of 3D transformation matrices.
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class REGISTRATIONARL_CLASS_API SPoseFrom2d : public ::fwServices::IRegisterer
{
public:
    fwCoreServiceClassDefinitionsMacro((SPoseFrom2d)(::fwServices::IRegisterer));

    typedef std::vector< ::arlCore::Point::csptr > ARLPointListType;
    typedef std::vector<std::string> VectKeyType;

    /**
     * @brief Constructor.
     */
    REGISTRATIONARL_API SPoseFrom2d() noexcept;

    /**
     * @brief Destructor.
     */
    REGISTRATIONARL_API virtual ~SPoseFrom2d() noexcept;

    /// Connect MarkerTL::s_OBJECT_PUSHED_SIG to s_REGISTER_SLOT
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    REGISTRATIONARL_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    REGISTRATIONARL_API void starting() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    REGISTRATIONARL_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    REGISTRATIONARL_API void stopping() override;

    /// Register matrix slot
    void computeRegistration(::fwCore::HiResClock::HiResClockType timestamp) override;

private:

    /// Initialized ARL cameras
    void initialize();

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    /// Matrix timeline keys
    std::string m_matrixKey;

    /// True if the service is initialized (timelines and ARLCameras)
    bool m_isInitialized;

    /// ARL planeSystem
    ::arlCore::PlaneSystem* m_planeSystem;

    /// Connections
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Points of the 3D model of the marker
    ARLPointListType m_3dModel;

    /// ARL cameras
    std::vector< const ::arlCore::Camera* > m_arlCameras;
};

} // namespace registrationARL
