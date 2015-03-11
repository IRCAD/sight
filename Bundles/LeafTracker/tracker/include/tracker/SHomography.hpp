/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
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
#include <fwServices/Base.hpp>
#include <fwServices/IController.hpp>

#include <arlcore/Point.h>
#include <arlcore/Reconst3D.h>

#include <boost/shared_ptr.hpp>

namespace tracker
{

/**
 * @brief   SHomography Class used to compute the rigid transformation.
 * @class   SHomography.
 */
class TRACKER_CLASS_API SHomography : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SHomography)(fwServices::IController));

    static const ::fwCom::Slots::SlotKeyType s_REGISTER_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> RegisterSlotType;

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

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     * @verbatim
        <service uid="..." impl="::tracker::SHomography" autoConnect="no">
            <config>
                <markerTL>
                    <key>markerTL1</key>
                    <key>markerTL2</key>
                </markerTL>
                <camera>
                    <key>cam1</key>
                    <key>cam2</key>
                </camera>
                <extrinsic>matrix1</extrinsic>
                <matrixTL>matrixTL1</matrixTL>
                <patternWidth>80</patternWidth>
            </config>
        </service>
       @endverbatim
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
    void initARLCameras();

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    /// Marker timeline keys
    VectKeyType m_markerTLKeys;

    /// Camera timeline keys
    VectKeyType m_cameraKeys;

    /// Matrix timeline keys
    std::string m_matrixKey;

    /// Extrinsic matrix key in composite
    std::string m_extrinsicKey;

    /// True if ARL cameras not initialized
    bool m_isCamInitialized;

    /// ARL planeSystem
    ::arlCore::PlaneSystem* m_planeSystem;

    /// Slots used when the frame have been refreshed
    RegisterSlotType::sptr m_slotRegister;

    /// Connections
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    /// Points of the 3D model of the marker
    ARLPointListType m_3dModel;

    ///< ARL cameras
    std::vector< const ::arlCore::Camera* > m_arlCameras;

    ///< Mutex used to lock access of doRegistration
    ::fwCore::mt::Mutex m_mutex;
};

} //namespace tracker

#endif /* __TRACKER_SHOMOGRAPHY_HPP__ */
