/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <data/camera.hpp>
#include <data/image.hpp>
#include <data/marker_map.hpp>

#include <service/tracker.hpp>

#include <ui/__/parameter.hpp>

#include <opencv2/aruco.hpp>

namespace sight::module::navigation::optics
{

/**
 * @brief   Class used to track multiple tags with ArUco.
 *
 * @see service::tracker
 *
 * @section Signals Signals
 * - \b detectionDone(core::hires_clock::type) : This signal is emitted when the tracker find tags.
 *
 * @section Slots Slots
 * @subsection Inherited Inherited slots (from tracker)
 * - \b track(timestamp) : Slot to fills the timeline with the new positions of the grid
 * - \b startTracking() : Slot called when the user wants to start tracking
 * - \b stopTracking() : Slot called when the user wants to stop tracking
 * - \b setParameter(ui::parameter_t, std::string): set a parameter from the UI.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::navigation::optics::aruco_tracker" >
            <in key="camera" uid="..." />
            <inout key="frame" uid="..." auto_connect="true" />
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
 * - \b camera [sight::data::camera]: camera calibration.
 *
 * @subsection In-Out In-Out
 * - \b frame [sight::data::image]: video frame.
 * - \b markerMap [sight::data::marker_map]: markers maps list where to extract the tags. The number of keys must match
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
class MODULE_NAVIGATION_OPTICS_CLASS_API aruco_tracker : public service::tracker
{
public:

    SIGHT_DECLARE_SERVICE(aruco_tracker, service::tracker);

    typedef core::com::signal<void (core::hires_clock::type _timestamp)> detection_done_signal_t;
    typedef core::com::signal<void (bool)> marker_detected_signal_t;

    /**
     * @name Signal API
     * @{
     */

    /// Key in m_signals map of signal m_sigDetectionDone
    MODULE_NAVIGATION_OPTICS_API static const core::com::signals::key_t DETECTION_DONE_SIG;

    /// Signal always emitted with boolean true if a least a maker from id list is found, false otherwise.
    MODULE_NAVIGATION_OPTICS_API static const core::com::signals::key_t MARKER_DETECTED_SIG;
    /** @} */
    /**
     * @name Slots API
     * @{
     */
    MODULE_NAVIGATION_OPTICS_API static const core::com::slots::key_t SET_PARAMETER_SLOT;
    /** @} */

    typedef std::vector<int> marker_id_t;
    typedef std::vector<marker_id_t> MarkerIDVectorType;

    /**
     * @brief Constructor.
     */
    MODULE_NAVIGATION_OPTICS_API aruco_tracker() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_NAVIGATION_OPTICS_API ~aruco_tracker() noexcept override;

protected:

    /// Depending on the configuration this connects:
    /// - the input timeline to the tracking() slot
    /// - the input frame modifications to the update() slot
    service::connections_t auto_connections() const override;

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
    MODULE_NAVIGATION_OPTICS_API void tracking(core::hires_clock::type& _timestamp) override;

private:

    /// Handles camera parameters (intrinsic matrix, distorsion coefficients and image size)
    struct Camera
    {
        cv::Mat intrinsic;
        cv::Mat distorsion;
        cv::Size2i size;
    };

    /// Slot called when a boolean value is changed
    void setParameter(sight::ui::parameter_t _val, std::string _key);

    /// Camera parameters
    Camera m_cameraParams;

    /// Marker vector [[0,1,2],[4,5,6]]
    MarkerIDVectorType m_markers;

    /// True if tracker is initialized
    bool m_isInitialized {false};

    /// Display markers in the image or not
    bool m_debugMarkers {false};

    /// aruco detector parameters structure
    cv::Ptr<cv::aruco::DetectorParameters> m_detectorParams;

    /// Dictionary/Set of markers. It contains the inner codification
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;

    /// Signal to emit when
    detection_done_signal_t::sptr m_sigDetectionDone;

    static constexpr std::string_view s_CAMERA_INPUT           = "camera";
    static constexpr std::string_view s_MARKER_MAP_INOUT_GROUP = "markerMap";

    data::ptr<data::camera, data::Access::in> m_camera {this, s_CAMERA_INPUT};
    data::ptr<data::image, data::Access::inout> m_frame {this, s_FRAME_INOUT};
    data::ptr_vector<data::marker_map, data::Access::inout> m_markerMap {this, s_MARKER_MAP_INOUT_GROUP};
};

} // namespace sight::module::navigation::optics
