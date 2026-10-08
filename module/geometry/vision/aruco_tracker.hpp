/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include <io/tracking/base.hpp>

#include <opencv2/aruco.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief   Class used to track multiple tags with ArUco.
 *
 * @see sight::io::tracking::base
 *
 * @section Signals Signals
 * - \b detectionDone(core::clock::type) : This signal is emitted when the tracker find tags.
 *
 * @section Slots Slots
 * @subsection Inherited Inherited slots (from tracker)
 * - \b track(timestamp) : Slot to fills the timeline with the new positions of the grid
 * - \b start_tracking() : Slot called when the user wants to start tracking
 * - \b stop_tracking() : Slot called when the user wants to stop tracking
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::vision::aruco_tracker" >
            <input camera="..." data="..." />
            <output debug_frame="..." />
                <marker_map>
                    <item data="..." /> // timeline of detected tag(s) -->
                    <item data="..." /> // timeline of detected tag(s) -->
                    <item data="..." /> // timeline of detected tag(s) -->
                </marker_map>
            </output>
            <track>
                <marker id="42,1,100,54" />
                <marker id="32,10" />
                <marker id="52,45" />
            </track>
            <config >
                <corner_refinement enabled="true" win_size="5" max_iterations="10" min_accuracy="0.01" />
                <adaptive_threshold enabled="true" block_size="3" constant="7" />
                <marker_detection min_perimeter_rate="0.03" min_distance_to_border="1." ... />
                <marker border_bits="1" .../>
                <perspective_removal pixel_per_cell="8" .../>
                <error_correction rate="0.6" .../>
            </config>
        </service>
   @endcode
 * @subsection Input Input
 * - \b input.camera [sight::data::camera]: camera calibration.
 * - \b input.frame [sight::data::image]: video frame used for detection.
 *
 * @subsection In-Out In-Out
 * - \b output.debug_frame [sight::data::image] (optional): transparent RGBA image containing only the detected
 * marker overlays.
 * - \b output.marker_map.item.data [sight::data::marker_map]: markers maps list where to extract the tags. The number
 * of keys must match
 * the number of \b markers entries in the config below.
 *
 * @subsection Configuration Configuration
 *  - \b track (mandatory)
 *      - \b markers (mandatory) : list of the tracked markers.
 *           - \b id (mandatory) : ids of the markers to detect.

 *  - \b config.debug_mode: if value is true, markers debugging mode is activated.
 *  - \b config.corner_refinement.enabled: enables corner refinement by subpixel.
 *  - \b config.corner_refinement.win_size, config.corner_refinement.max_iterations,
 *     config.corner_refinement.min_accuracy: corner refinement parameters.
 *  - \b config.adaptive_threshold.*: adaptive threshold parameters.
 *  - \b config.marker_detection.*: marker detection parameters.
 *  - \b config.marker.border_bits: marker border width.
 *  - \b config.perspective_removal.*: perspective removal parameters.
 *  - \b config.error_correction.*: marker error correction parameters.
 */
class aruco_tracker final : public sight::io::tracking::base<sight::io::tracking::sensor_t>
{
public:

    struct signals
    {
        using detection_done_t  = core::com::signal<void (core::clock::type)>;
        using marker_detected_t = core::com::signal<void (bool)>;
        static inline const signal_key_t DETECTION_DONE  = "detectionDone";
        static inline const signal_key_t MARKER_DETECTED = "marker_detected";
    };

    SIGHT_DECLARE_SERVICE(aruco_tracker, sight::io::tracking::base<sight::io::tracking::sensor_t>);

    using marker_id_t        = std::vector<int>;
    using marker_id_vector_t = std::vector<marker_id_t>;

    /**
     * @brief Constructor.
     */
    aruco_tracker() noexcept;

    /**
     * @brief Destructor.
     */
    ~aruco_tracker() noexcept final = default;

protected:

    /// Depending on the configuration this connects:
    /// - the input timeline to the tracking() slot
    /// - the input frame modifications to the update() slot
    [[nodiscard]] service::connections_t auto_connections() const final;

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    void configuring(const service::config_t& _config) final;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    void starting() final;

    /**
     * @brief Updating method : This method does nothing
     */
    void updating() final;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    void stopping() final;

    /// Detect marker
    void tracking(core::clock::type& _timestamp) final;

private:

    /// Builds detector parameters from the current configuration data.
    [[nodiscard]] cv::Ptr<cv::aruco::DetectorParameters> make_detector_parameters() const;

    /// Handles camera parameters (intrinsic matrix, distorsion coefficients and image size)
    struct camera
    {
        cv::Mat intrinsic;
        cv::Mat distorsion;
        cv::Size2i size;
    };

    /// Camera parameters
    camera m_camera_params;

    /// Marker vector [[0,1,2],[4,5,6]]
    marker_id_vector_t m_markers;

    /// True if tracker is initialized
    bool m_is_initialized {false};

    /// Dictionary/Set of markers. It contains the inner codification
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;

    static constexpr std::string_view CAMERA_INPUT           = "input.camera";
    static constexpr std::string_view FRAME_INPUT            = "input.frame";
    static constexpr std::string_view DEBUG_FRAME_INOUT      = "output.debug_frame";
    static constexpr std::string_view MARKER_MAP_INOUT_GROUP = "output.marker_map.item.data";

    data::ptr<data::camera, data::access::in> m_camera {this, CAMERA_INPUT};
    data::ptr<data::image, data::access::in> m_frame {this, FRAME_INPUT};
    data::ptr<data::image, data::access::inout> m_debug_frame {this, DEBUG_FRAME_INOUT, true};
    data::ptr_vector<data::marker_map, data::access::inout> m_marker_map {this, MARKER_MAP_INOUT_GROUP};

    /// show marker or not -->
    data::ptr<data::boolean, data::access::in> m_debug_mode {this, "config.debug_mode", false};
    ///  do corner refinement or not. -->
    data::ptr<data::boolean, data::access::in> m_corner_refinement {this, "config.corner_refinement.enabled", false};
    /// minimum window size for adaptive thresholding before finding contours -->
    data::ptr<data::integer, data::access::in> m_adaptive_th_win_size_min {this,
                                                                           "config.adaptive_threshold.win_size_min", 3
    };
    /// maximum window size for adaptive thresholding before finding contours -->
    data::ptr<data::integer, data::access::in> m_adaptive_th_win_size_max {this,
                                                                           "config.adaptive_threshold.win_size_max", 23
    };
    /// increments from adaptiveThreshWinSizeMin to adaptiveThreshWinSizeMax during the thresholding -->
    data::ptr<data::integer, data::access::in> m_adaptive_th_win_size_step {this,
                                                                            "config.adaptive_threshold.win_size_step",
                                                                            10
    };
    /// constant for adaptive thresholding before finding contours -->
    data::ptr<data::real, data::access::in> m_adaptive_th_constant {this, "config.adaptive_threshold.constant", 7.};
    /// determine minimum perimeter for marker contour to be detected.
    data::ptr<data::real, data::access::in> m_min_marker_perimeter_rate {this,
                                                                         "config.marker_detection.min_perimeter_rate",
                                                                         0.03
    };
    /// determine maximum perimeter for marker contour to be detected.
    data::ptr<data::real, data::access::in> m_max_marker_perimeter_rate {this,
                                                                         "config.marker_detection.max_perimeter_rate",
                                                                         4.0
    };
    /// minimum accuracy during the polygonal approximation process to determine which contours are squares -->
    data::ptr<data::real, data::access::in> m_polygonal_approx_accuracy_rate {this,
                                                                              "config.marker_detection.polygonal_approx_accuracy_rate",
                                                                              0.03
    };
    /// minimum distance between corners for detected markers relative to its perimeter -->
    data::ptr<data::real, data::access::in> m_min_corner_distance_rate {this,
                                                                        "config.marker_detection.min_corner_distance_rate",
                                                                        0.01
    };
    /// minimum distance of any corner to the image border for detected markers (in pixels) -->
    data::ptr<data::integer, data::access::in> m_min_distance_to_border {this,
                                                                         "config.marker_detection.min_distance_to_border",
                                                                         1
    };
    /// minimum mean distance beetween two marker corners to be considered similar,
    data::ptr<data::real, data::access::in> m_min_marker_distance_rate {this,
                                                                        "config.marker_detection.min_marker_distance_rate",
                                                                        0.
    };
    /// window size for the corner refinement process (in pixels) -->
    data::ptr<data::integer, data::access::in> m_corner_refinement_win_size {this, "config.corner_refinement.win_size",
                                                                             5
    };
    /// maximum number of iterations for stop criteria of the corner refinement process -->
    data::ptr<data::integer, data::access::in> m_corner_refinement_max_iterations {this,
                                                                                   "config.corner_refinement.max_iterations",
                                                                                   30
    };
    /// minimum error for the stop criteria of the corner refinement process -->
    data::ptr<data::real, data::access::in> m_corner_refinement_min_accuracy {this,
                                                                              "config.corner_refinement.min_accuracy",
                                                                              0.1
    };
    /// number of bits of the marker border, i.e. marker border width -->
    data::ptr<data::integer, data::access::in> m_marker_border_bits {this, "config.marker.border_bits", 1};
    /// number of bits (per dimension) for each cell of the marker when removing the perspective -->
    data::ptr<data::integer, data::access::in> m_perspective_remove_pixel_per_cell {this,
                                                                                    "config.perspective_removal.pixel_per_cell",
                                                                                    8
    };
    /// width of the margin of pixels on each cell not considered for the determination of the cell bit.
    /// Represents the rate respect to the total size of the cell,i.e. perspective_remove_pixel_per_cell
    data::ptr<data::real, data::access::in> m_perspective_remove_ignored_margin_per_cell {this,
                                                                                          "config.perspective_removal.ignored_margin_per_cell",
                                                                                          0.1
    };
    /// maximum number of accepted erroneous bits in the border (i.e. number of allowed white bits in the border).
    /// Represented as a rate respect to the total number of bits per marker
    data::ptr<data::real, data::access::in> m_max_erroneous_bits_in_border_rate {this,
                                                                                 "config.error_correction.max_erroneous_bits_in_border_rate",
                                                                                 0.3
    };
    /// minimun standard deviation in pixels values during the decode step to apply Otsu thresholding
    /// (otherwise, all the bits are set to 0 or 1 depending on mean higher than 128 or not)
    data::ptr<data::real, data::access::in> m_min_otsu_std_dev {this, "config.error_correction.min_otsu_std_dev", 5.0};
    /// error correction rate respect to the maximun error correction capability for each dictionary -->
    data::ptr<data::real, data::access::in> m_error_correction_rate {this, "config.error_correction.rate", 0.6};
};

} // namespace sight::module::geometry::vision
