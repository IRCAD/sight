/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <data/boolean.hpp>
#include <data/camera.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/marker_map.hpp>
#include <data/real.hpp>

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
 * - \b detectionDone(core::clock::type) : This signal is emitted when the tracker find tags.
 *
 * @section Slots Slots
 * @subsection Inherited Inherited slots (from tracker)
 * - \b track(timestamp) : Slot to fills the timeline with the new positions of the grid
 * - \b start_tracking() : Slot called when the user wants to start tracking
 * - \b stop_tracking() : Slot called when the user wants to stop tracking
 * - \b set_parameter(ui::parameter_t, std::string): set a parameter from the UI.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::navigation::optics::aruco_tracker" >
            <in key="camera" uid="..." />
            <inout key="frame" uid="..." auto_connect="true" />
            <inout group="markerMap">
                <key uid="..." /> // timeline of detected tag(s) -->
                <key uid="..." /> // timeline of detected tag(s) -->
                <key uid="..." /> // timeline of detected tag(s) -->
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
class aruco_tracker : public service::tracker
{
public:

    SIGHT_DECLARE_SERVICE(aruco_tracker, service::tracker);

    using detection_done_signal_t  = core::com::signal<void (core::clock::type)>;
    using marker_detected_signal_t = core::com::signal<void (bool)>;

    /**
     * @name Signal API
     * @{
     */

    /// Key in m_signals map of signal m_sigDetectionDone
    static const core::com::signals::key_t DETECTION_DONE_SIG;

    /// Signal always emitted with boolean true if a least a maker from id list is found, false otherwise.
    static const core::com::signals::key_t MARKER_DETECTED_SIG;
    /** @} */
    /**
     * @name Slots API
     * @{
     */
    static const core::com::slots::key_t SET_PARAMETER_SLOT;
    /** @} */

    using marker_id_t        = std::vector<int>;
    using marker_id_vector_t = std::vector<marker_id_t>;

    /**
     * @brief Constructor.
     */
    aruco_tracker() noexcept;

    /**
     * @brief Destructor.
     */
    ~aruco_tracker() noexcept override;

protected:

    /// Depending on the configuration this connects:
    /// - the input timeline to the tracking() slot
    /// - the input frame modifications to the update() slot
    service::connections_t auto_connections() const override;

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    void configuring(const config_t& _config) override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    void starting() override;

    /**
     * @brief Updating method : This method does nothing
     */
    void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    void stopping() override;

    /// Detect marker
    void tracking(core::clock::type& _timestamp) override;

private:

    /// Handles camera parameters (intrinsic matrix, distorsion coefficients and image size)
    struct camera
    {
        cv::Mat intrinsic;
        cv::Mat distorsion;
        cv::Size2i size;
    };

    /// Slot called when a boolean value is changed
    void on_property_set(std::string_view _key) override;

    /// Camera parameters
    camera m_camera_params;

    /// Marker vector [[0,1,2],[4,5,6]]
    marker_id_vector_t m_markers;

    /// True if tracker is initialized
    bool m_is_initialized {false};

    /// aruco detector parameters structure
    cv::Ptr<cv::aruco::DetectorParameters> m_detector_params;

    /// Dictionary/Set of markers. It contains the inner codification
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;

    /// Signal to emit when
    detection_done_signal_t::sptr m_sig_detection_done;

    static constexpr std::string_view CAMERA_INPUT           = "camera";
    static constexpr std::string_view MARKER_MAP_INOUT_GROUP = "markerMap";

    data::ptr<data::camera, data::access::in> m_camera {this, CAMERA_INPUT};
    data::ptr<data::image, data::access::inout> m_frame {this, FRAME_INOUT};
    data::ptr_vector<data::marker_map, data::access::inout> m_marker_map {this, MARKER_MAP_INOUT_GROUP};

    /// show marker or not -->
    data::property<data::boolean> m_debug_mode {this, "debug_mode", false};
    ///  do corner refinement or not. -->
    data::property<data::boolean> m_corner_refinement {this, "corner_refinement", false};
    /// minimum window size for adaptive thresholding before finding contours -->
    data::property<data::integer> m_adaptive_th_win_size_min {this, "adaptive_th_win_size_min", 3};
    /// maximum window size for adaptive thresholding before finding contours -->
    data::property<data::integer> m_adaptive_th_win_size_max {this, "adaptive_th_win_size_max", 23};
    /// increments from adaptiveThreshWinSizeMin to adaptiveThreshWinSizeMax during the thresholding -->
    data::property<data::integer> m_adaptive_th_win_size_step {this, "adaptive_th_win_size_step", 10};
    /// constant for adaptive thresholding before finding contours -->
    data::property<data::real> m_adaptive_th_constant {this, "adaptive_th_constant", 7.};
    /// determine minimum perimeter for marker contour to be detected.
    data::property<data::real> m_min_marker_perimeter_rate {this, "min_marker_perimeter_rate", 0.03};
    /// determine maximum perimeter for marker contour to be detected.
    data::property<data::real> m_max_marker_perimeter_rate {this, "max_marker_perimeter_rate", 4.0};
    /// minimum accuracy during the polygonal approximation process to determine which contours are squares -->
    data::property<data::real> m_polygonal_approx_accuracy_rate {this, "polygonal_approx_accuracy_rate", 0.03};
    /// minimum distance between corners for detected markers relative to its perimeter -->
    data::property<data::real> m_min_corner_distance_rate {this, "min_corner_distance_rate", 0.01};
    /// minimum distance of any corner to the image border for detected markers (in pixels) -->
    data::property<data::integer> m_min_distance_to_border {this, "min_distance_to_border", 1};
    /// minimum mean distance beetween two marker corners to be considered similar,
    data::property<data::real> m_min_marker_distance_rate {this, "min_marker_distance_rate", 0.};
    /// window size for the corner refinement process (in pixels) -->
    data::property<data::integer> m_corner_refinement_win_size {this, "corner_refinement_win_size", 5};
    /// maximum number of iterations for stop criteria of the corner refinement process -->
    data::property<data::integer> m_corner_refinement_max_iterations {this, "corner_refinement_max_iterations", 30};
    /// minimum error for the stop criteria of the corner refinement process -->
    data::property<data::real> m_corner_refinement_min_accuracy {this, "corner_refinement_min_accuracy", 0.1};
    /// number of bits of the marker border, i.e. marker border width -->
    data::property<data::integer> m_marker_border_bits {this, "marker_border_bits", 1};
    /// number of bits (per dimension) for each cell of the marker when removing the perspective -->
    data::property<data::integer> m_perspective_remove_pixel_per_cell {this, "perspective_remove_pixel_per_cell", 8};
    /// width of the margin of pixels on each cell not considered for the determination of the cell bit.
    /// Represents the rate respect to the total size of the cell,i.e. perspective_remove_pixel_per_cell
    data::property<data::real> m_perspective_remove_ignored_margin_per_cell {this,
                                                                             "perspective_remove_ignored_margin_per_cell",
                                                                             0.1
    };
    /// maximum number of accepted erroneous bits in the border (i.e. number of allowed white bits in the border).
    /// Represented as a rate respect to the total number of bits per marker
    data::property<data::real> m_max_erroneous_bits_in_border_rate {this, "max_erroneous_bits_in_border_rate", 0.3};
    /// minimun standard deviation in pixels values during the decode step to apply Otsu thresholding
    /// (otherwise, all the bits are set to 0 or 1 depending on mean higher than 128 or not)
    data::property<data::real> m_min_otsu_std_dev {this, "min_otsu_std_dev", 5.0};
    /// error correction rate respect to the maximun error correction capability for each dictionary -->
    data::property<data::real> m_error_correction_rate {this, "error_correction_rate", 0.6};
};

} // namespace sight::module::navigation::optics
