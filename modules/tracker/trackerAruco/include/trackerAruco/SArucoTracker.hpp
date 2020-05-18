/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "trackerAruco/config.hpp"

#include <arServices/ITracker.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/HiResClock.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/aruco.hpp>

namespace trackerAruco
{

/**
 * @brief   Class used to track multiple tags with ArUco.
 *
 * @see ::arServices::ITracker
 *
 * @section Signals Signals
 * - \b detectionDone(::fwCore::HiResClock::HiResClockType) : This signal is emitted when the tracker find tags.
 *
 * @section Slots Slots
 * @subsection Inherited Inherited slots (from ITracker)
 * - \b track(timestamp) : Slot to fills the timeline with the new positions of the grid
 * - \b startTracking() : Slot called when the user wants to start tracking
 * - \b stopTracking() : Slot called when the user wants to stop tracking
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::trackerAruco::SArucoTracker" >
            <in key="timeline" uid="..." autoConnect="yes"/>
            <in key="camera" uid="..." />
            <inout group="tagTL" >
                <key uid="..." />
                <key uid="..." />
                <key uid="..." />
            </inout>
            <track>
                <marker id="42,1,100,54" />
                <marker id="32,10" />
                <marker id="52,45" />
            </track>
            <debugMarkers>yes</debugMarkers>
            <cornerRefinement>yes</cornerRefinement>
        </service>

        or

        <service uid="..." type="::trackerAruco::SArucoTracker" >
            <in key="camera" uid="..." />
            <inout key="frame" uid="..." autoConnect="yes" />
            <inout group="markerMap">
                <key uid="..." /> <!-- timeline of detected tag(s) -->
                <key uid="..." /> <!-- timeline of detected tag(s) -->
                <key uid="..." /> <!-- timeline of detected tag(s) -->
            </inout>
            <track>
                <marker id="42,1,100,54" />
                <marker id="32,10" />
                <marker id="52,45" />
            </track>
            <debugMarkers>yes</debugMarkers>
            <cornerRefinement>yes</cornerRefinement>
        </service>
   @endcode
 * @subsection Input Input
 * - \b timeline [::arData::FrameTL]: camera used to display video. It is the main timeline used for the tracking.
 * - \b camera [::arData::Camera]: camera calibration.
 *
 * @subsection In-Out In-Out
 * - \b frame [::fwData::Image]: video frame.
 * - \b tagTL [::arData::MarkerTL]: list of markers timelines where to extract the tags. The number of tagTL inout keys
 * must match the number of \b markers entries in the config below.
 * - \b markerMap [::arData::MarkerMap]: markers maps list where to extract the tags. The number of keys must match
 * the number of \b markers entries in the config below.
 *
 * @subsection Configuration Configuration
 *  - \b track (mandatory)
 *      - \b markers (mandatory) : list of the tracked markers.
 *           - \b id (mandatory) : ids of the markers to detect.
 *  - \b debugMarkers : if value is yes markers debugging mode is activated.
 *  - \b cornerRefinement: if yes corner refinement by subpixel will be activited
 *  not.
 */
class TRACKERARUCO_CLASS_API SArucoTracker : public ::arServices::ITracker
{
public:

    fwCoreServiceMacro(SArucoTracker, arServices::ITracker);

    typedef ::fwCom::Signal< void (::fwCore::HiResClock::HiResClockType timestamp) > DetectionDoneSignalType;
    typedef ::fwCom::Signal< void (bool) > MarkerDetectedSignalType;

    /**
     * @name Signal API
     * @{
     */
    /// Key in m_signals map of signal m_sigDetectionDone
    TRACKERARUCO_API static const ::fwCom::Signals::SignalKeyType s_DETECTION_DONE_SIG;
    /// Signal always emitted with boolean true if a least a maker from id list is found, false otherwise.
    TRACKERARUCO_API static const ::fwCom::Signals::SignalKeyType s_MARKER_DETECTED_SIG;
    /** @} */
    /**
     * @name Slots API
     * @{
     */
    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    /** @} */

    typedef std::vector< int >          MarkerIDType;
    typedef std::vector< MarkerIDType > MarkerIDVectorType;

    /**
     * @brief Constructor.
     */
    TRACKERARUCO_API SArucoTracker() noexcept;

    /**
     * @brief Destructor.
     */
    TRACKERARUCO_API virtual ~SArucoTracker() noexcept override;

protected:
    /// Depending on the configuration this connects:
    /// - the input timeline to the tracking() slot
    /// - the input frame modifications to the update() slot
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    TRACKERARUCO_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    TRACKERARUCO_API void starting() override;

    /**
     * @brief Updating method : This method does nothing
     */
    TRACKERARUCO_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    TRACKERARUCO_API void stopping() override;

    /// Detect marker
    TRACKERARUCO_API virtual void tracking(::fwCore::HiResClock::HiResClockType& timestamp) override;

private:

    /// Handles camera parameters (intrinsic matrix, distorsion coefficients and image size)
    struct Camera
    {
        ::cv::Mat intrinsic;
        ::cv::Mat distorsion;
        ::cv::Size2i size;
    };

    /// Slot called when a integer value is changed
    void setIntParameter(int _val, std::string _key);
    /// Slot called when a double value is changed
    void setDoubleParameter(double _val, std::string _key);
    /// Slot called when a boolean value is changed
    void setBoolParameter(bool _val, std::string _key);

    /// Camera parameters
    Camera m_cameraParams;
    /// Marker vector [[0,1,2],[4,5,6]]
    MarkerIDVectorType m_markers;
    /// True if tracker is initialized
    bool m_isInitialized;
    /// Display markers in the image or not
    bool m_debugMarkers;
    /// aruco detector parameters structure
    ::cv::Ptr< ::cv::aruco::DetectorParameters > m_detectorParams;
    /// Dictionary/Set of markers. It contains the inner codification
    ::cv::Ptr< ::cv::aruco::Dictionary > m_dictionary;
    /// Signal to emit when
    DetectionDoneSignalType::sptr m_sigDetectionDone;
};

} // namespace trackerAruco
