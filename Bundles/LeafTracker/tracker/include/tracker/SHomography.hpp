/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TRACKER_SHOMOGRAPHY_HPP__
#define __TRACKER_SHOMOGRAPHY_HPP__

#include "tracker/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/HiResClock.hpp>
#include <fwCore/mt/types.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/IController.hpp>
#include <fwServices/macros.hpp>

#include <arlcore/Point.h>
#include <arlcore/Reconst3D.h>


namespace tracker
{

/**
 * @brief   SHomography Class used to compute the rigid transformation.
 *
 * @section Slots Slots
 * - \b register(::fwCore::HiResClock::HiResClockType timestamp) : computes the homography.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::tracker::SHomography">
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
 * - \b matrixTL [::arData::MatrixTL]: timaline of 3D transformation matrices.
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class TRACKER_CLASS_API SHomography : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SHomography)(fwServices::IController));

    static const ::fwCom::Slots::SlotKeyType s_REGISTER_SLOT;

    typedef std::vector< ::arlCore::Point::csptr > ARLPointListType;
    typedef std::vector<std::string> VectKeyType;

    /**
     * @brief Constructor.
     */
    TRACKER_API SHomography() throw ();

    /**
     * @brief Destructor.
     */
    TRACKER_API virtual ~SHomography() throw ();

    /// Connect MarkerTL::s_OBJECT_PUSHED_SIG to s_REGISTER_SLOT
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    TRACKER_API void configuring() throw (fwTools::Failed);

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    TRACKER_API void starting() throw (fwTools::Failed);

    /**
     * @brief Updating method : This method is used to update the service.
     */
    TRACKER_API void updating() throw (fwTools::Failed);

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    TRACKER_API void stopping() throw (fwTools::Failed);

    /// Register matrix slot
    void doRegistration(::fwCore::HiResClock::HiResClockType timestamp);

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

    ///< ARL cameras
    std::vector< const ::arlCore::Camera* > m_arlCameras;

    ///< Mutex used to lock access of doRegistration
    ::fwCore::mt::Mutex m_mutex;
};

} //namespace tracker

#endif /* __TRACKER_SHOMOGRAPHY_HPP__ */
