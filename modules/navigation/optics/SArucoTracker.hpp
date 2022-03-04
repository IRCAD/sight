/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/navigation/optics/config.hpp"

#include <data/Camera.hpp>
#include <data/Image.hpp>
#include <data/MarkerMap.hpp>

#include <service/ITracker.hpp>

#include <opencv2/aruco.hpp>

namespace sight::module::navigation::optics
{

/**
 * @brief   Class used to track multiple tags with ArUco.
 *
 * @see service::ITracker
 *
 * @section Signals Signals
 * - \b detectionDone(core::HiResClock::HiResClockType) : This signal is emitted when the tracker find tags.
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
        <service uid="..." type="sight::module::navigation::optics::SArucoTracker" >
            <in key="camera" uid="..." />
            <inout key="frame" uid="..." autoConnect="true" />
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
            <debugMarkers>true</debugMarkers>
            <cornerRefinement>true</cornerRefinement>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [sight::data::Camera]: camera calibration.
 *
 * @subsection In-Out In-Out
 * - \b frame [sight::data::Image]: video frame.
 * - \b markerMap [sight::data::MarkerMap]: markers maps list where to extract the tags. The number of keys must match
 * the number of \b markers entries in the config below.
 *
 * @subsection Configuration Configuration
 *  - \b track (mandatory)
 *      - \b markers (mandatory) : list of the tracked markers.
 *           - \b id (mandatory) : ids of the markers to detect.

 *  - \b debugMarkers : if value is true, markers debugging mode is activated.
 *  - \b cornerRefinement: if true, corner refinement by subpixel will be activated
 *  not.
 */
class MODULE_NAVIGATION_OPTICS_CLASS_API SArucoTracker : public service::ITracker
{
public:

    SIGHT_DECLARE_SERVICE(SArucoTracker, service::ITracker);

    typedef core::com::Signal<void (core::HiResClock::HiResClockType timestamp)> DetectionDoneSignalType;
    typedef core::com::Signal<void (bool)> MarkerDetectedSignalType;

    /**
     * @name Signal API
     * @{
     */

    /// Key in m_signals map of signal m_sigDetectionDone
    MODULE_NAVIGATION_OPTICS_API static const core::com::Signals::SignalKeyType s_DETECTION_DONE_SIG;

    /// Signal always emitted with boolean true if a least a maker from id list is found, false otherwise.
    MODULE_NAVIGATION_OPTICS_API static const core::com::Signals::SignalKeyType s_MARKER_DETECTED_SIG;
    /** @} */
    /**
     * @name Slots API
     * @{
     */
    MODULE_NAVIGATION_OPTICS_API static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    MODULE_NAVIGATION_OPTICS_API static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    MODULE_NAVIGATION_OPTICS_API static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    /** @} */

    typedef std::vector<int> MarkerIDType;
    typedef std::vector<MarkerIDType> MarkerIDVectorType;

    /**
     * @brief Constructor.
     */
    MODULE_NAVIGATION_OPTICS_API SArucoTracker() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_NAVIGATION_OPTICS_API ~SArucoTracker() noexcept override;

protected:

    /// Depending on the configuration this connects:
    /// - the input timeline to the tracking() slot
    /// - the input frame modifications to the update() slot
    service::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    MODULE_NAVIGATION_OPTICS_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    MODULE_NAVIGATION_OPTICS_API void starting() override;

    /**
     * @brief Updating method : This method does nothing
     */
    MODULE_NAVIGATION_OPTICS_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    MODULE_NAVIGATION_OPTICS_API void stopping() override;

    /// Detect marker
    MODULE_NAVIGATION_OPTICS_API void tracking(core::HiResClock::HiResClockType& timestamp) override;

private:

    /// Handles camera parameters (intrinsic matrix, distorsion coefficients and image size)
    struct Camera
    {
        cv::Mat intrinsic;
        cv::Mat distorsion;
        cv::Size2i size;
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
    cv::Ptr<cv::aruco::DetectorParameters> m_detectorParams;

    /// Dictionary/Set of markers. It contains the inner codification
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;

    /// Signal to emit when
    DetectionDoneSignalType::sptr m_sigDetectionDone;

    static constexpr std::string_view s_CAMERA_INPUT           = "camera";
    static constexpr std::string_view s_TAGTL_INOUT_GROUP      = "frame";
    static constexpr std::string_view s_MARKER_MAP_INOUT_GROUP = "markerMap";

    data::ptr<data::Camera, data::Access::in> m_camera {this, s_CAMERA_INPUT, true};
    data::ptr<data::Image, data::Access::inout> m_frame {this, s_TAGTL_INOUT_GROUP};
    data::ptr_vector<data::MarkerMap, data::Access::inout> m_markerMap {this, s_MARKER_MAP_INOUT_GROUP};
};

} // namespace sight::module::navigation::optics
