/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <core/com/signal.hpp>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/location/single_file.hpp>
#include <core/thread/timer.hpp>
#include <core/timer.hpp>

#include <data/camera.hpp>
#include <data/frame_tl.hpp>
#include <data/matrix_tl.hpp>
#include <data/mesh.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>

#include <navigation/openvslam/openvslam_config.hpp>

#include <service/notifier.hpp>
#include <service/tracker.hpp>

#include <opencv2/opencv.hpp>

namespace openvslam
{

class system;
class config;

namespace publish
{

class map_publisher;
class frame_publisher;

} // namespace publish

} // namespace openvslam

/// The module openvslamTracker contains openvslam service to manage instance of OpenVSLAM.
namespace sight::module::navigation::openvslam
{

/**
 * @brief Service that manages an instance of Openvslam (Simultaneous tracking and Mapping).
 *
 * It will populate a matrixTL with camera position, and can also export the map as a pointcloud (data::mesh).
 * Note: in this version of the service only monocular (MONO) mode is available, and the downsample feature is disabled.
 *
 * @section Signals Signals
 * - \b tracking_initialized(): Emitted when the tracking is initialized.
 * - \b trackingNotInitialized(): Emitted when the tracking must be reinitialized.
 * - \b tracked(): Emitted when openvslam is tracking.
 * - \b tracking_lost(): Emitted when tracking is lost.
 * - \b voc_file_unloaded(): Emitted when the vocabulary file is unloaded.
 * - \b voc_file_loading_started(): Emitted when the vocabulary file is loading.
 * - \b voc_file_loaded(): Emitted when the vocabulary file is loaded.
 *
 * @section Slots Slots
 * - \b start_tracking(): Initialize and start the tracking process.
 * - \b stop_tracking(): Stop the tracking process.
 * - \b track(timestamp): Call openvslam with the new frame.
 * - \b enable_localization(bool): Enable the localization mode by stopping mapping thread.
 * - \b activateLocalization(): Activate the localization mode by stopping mapping thread.
 * - \b reset_point_cloud(): Reset the pointcloud.
 * - \b save_map(): Save openvslam's map.
 * - \b load_map(): Load openvslam's map.
 * - \b set_double_parameter(double, string): Calls a double parameter slot according to the given key.
 *   - scaleFactor: to rescale matrix and points (by default Monocular use an arbitrary scale).
 *   - initializer.parallaxDegThr: Parallax threshold in degree for openvslam initializer (advanced).
 *   - initializer.reprojectionErrThr: Reprojection error threshold for openvslam initializer (advanced).
 *   - initializer.scalingFactor: Initial scale magnitude for openvslam initializer (advanced).
 * - \b set_int_parameter(int, string): Calls an int parameter slot according to the given key.
 *   - nFeatures: Set the number of features per image.
 *   - nLevels: Set the number of levels in the scale pyramid.
 *   - iniThFAST: Initial FAST Threshold value .
 *   - minThFAST: Min FAST Threshold value.
 *   - initializer.numRansacIterations: Number of RANSAC iteration of openvslam initializer (advanced).
 *   - initializer.minNumTriangulatedPts: Minimal number of triangulated points for openvslam initializer (advanced).
 *   - initializer.numBAIterations: Number of iterations of the Bundle-Adjustment for openvslam initializer (advanced).
 * - \b set_bool_parameter(bool, string): Calls a bool parameter slot according to the given key.
 *   - showFeatures: Call an imshow to display internal image of openvslam (with features projected).
 * - \b set_enum_parameter(string, string): Calls an enum parameter slot according to the given key.
 * - map_t: "Global" or "Local", fill either the global map or the local map in the pointcloud output.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::navigation::openvslam::openvslam" worker="trackerWorker" >
            <in key="camera" uid="..." />
            <in key="timeline" uid="..." auto_connect="true" />
            <in key="timeline2" uid="..." />
            <in key="scale" uid="..." />
            <inout key="cameraMatrixTL" uid="..." />
            <out key="pointCloud" uid="..." />
            <mode>MONO</mode>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b camera [sight::data::camera](mandatory): camera that will be tracked.
 * - \b timeline [sight::data::frame_tl](mandatory): timeline of frames of the video on which openvslam will work.
 * - \b timeline2 [sight::data::frame_tl](optional): Only needed if STEREO/DEPTH mode is enabled !
 * if STEREO: frameTL2 will represent frame from the second camera.
 * if DEPTH: frameTL2 will represent frame from depth sensor.
 * - \b scale [sight::data::real](optional): scale to apply to each positions (and pointcloud), each position will be
 * multiplied by 1/scale
 *
 * @subsection In-Out In-Out:
 * - \b cameraMatrixTL [sight::data::matrix_tl](optional): timeLine of  matrix representing the movement of the 3D
 * camera and thus of the real camera.
 *
 * @subsection Output Output:
 * - \b pointCloud [sight::data::mesh](optional): mesh containing the 3D points given by openvslam
 *
 * @subsection Configuration Configuration:
 * - \b mode (optional): set the tracking mode (MONO, STEREO, DEPTH), by default MONO will be used.
 *  Note that if STEREO/DEPTH mode is used  the frameTL2 input will be needed.
 * - \b mapFile (optional): if specified, the service will attempt to load the specified map file when tracking is
 *   started and to save it when tracking is stopped. If this option is not specified or if the file is not found when
 *   starting the tracking, an empty map will be created instead.
 */
class openvslam final : public service::tracker,
                        private service::notifier
{
public:

    SIGHT_DECLARE_SERVICE(openvslam, service::tracker);

    /// Constructor. Initializes signals and slots.
    openvslam() noexcept;

    /// Destructor. Stops the service if started.
    ~openvslam() noexcept final;

    /**
     * @name Tracking Mode : Openvslam can be used with 3 mode.
     * - \b MONO : Use only a monocular camera.
     * - \b STEREO: Use a stereovision system.(NOT IMPLEMENTED)
     * - \b DEPTH : Use a RGB-D sensor. (NOT IMPLEMENTED)
     */
    enum class tracking_mode
    {
        mono = 0,
        stereo,
        depth
    };

protected:

    /// Configures the service by parsing XML.
    void configuring(const config_t& _config) final;

    /// Retrieves input data.
    void starting() final;

    /// Shutdown the openvslam system & reset output.
    void stopping() final;

    /// Does nothing.
    void updating() final;

private:

    /**
     * @name Slots methods
     *
     * @{
     */

    /// Slot: called to start the tracking.
    void start_tracking() final;

    /// Slot: called to stop the tracking.
    void stop_tracking() final;

    /// Slot: called to enable/disable localization mode (stop/(re)start mapping thread).
    void enable_localization(bool _enable);

    /// Slot: called to enable localization mode.
    void activate_localization();

    /// Slot: called to disable localization mode.
    void deactivate_localization();

    /// Slot: called when a integer value is changed.
    void set_int_parameter(int _val, std::string _key);

    /// Slot: called when a double value is changed.
    void set_double_parameter(double _val, std::string _key);

    /// Slot: called when a bool value is changed.
    void set_bool_parameter(bool _val, std::string _key);

    /// Slot: called when an enum value is changed.
    void set_enum_parameter(std::string _val, std::string _key);

    /// Slot: Load Openvslam map file.
    void load_map();

    /// Slot: Save Openvslam map file.
    void save_map();

    /// Slot: Save trajectories files (both frame & KeyFrames).
    void save_trajectories();

    /// Slot: Pause/resume tracker.
    void pause();

    /// Slot: called to reset the pointcloud.
    void reset_point_cloud();

    /// Slot: call openvslam with the new frame.
    void tracking(core::clock::type& _timestamp) final;
    /** @} */

    /**
     * @name Signals API
     * @{
     */

    /// Type of the signals.

    using signal_t = core::com::signal<void ()>;
    /** @} */

    /**
     * @name Signals objects
     *
     * @{
     */

    /// Signal: sent when the tracking is initialized.
    signal_t::sptr m_sig_tracking_initialized;

    /// Signal: sended when the tracking is not initialized.
    signal_t::sptr m_sig_tracking_not_initialized;

    /// Signal: sent when the tracking is found.
    signal_t::sptr m_sig_tracked;

    /// Signal: sent when the tracking is lost.
    signal_t::sptr m_sig_tracking_lost;

    /// Signal: sent when the vocabulary file is unloaded.
    signal_t::sptr m_sig_voc_file_unloaded;

    /// Signal: sent when the vocabulary file is loading.
    signal_t::sptr m_sig_voc_file_loading_started;

    /// Signal: sent when the vocabulary file is loaded.
    signal_t::sptr m_sig_voc_file_loaded;

    /// Signal: sent when the map file is loaded.
    signal_t::sptr m_sig_map_loaded;
    /** @} */

    /// Start the tracking with the path of the _mapFile.
    void start_tracking(const std::string& _map_file);

    /// Update pointcloud from openvslam's map.
    void update_point_cloud();

private:

    /// ORB Parameters structure
    sight::navigation::openvslam::orb_params m_orb_parameters;

    /// Openvslam initializer parameters (only used in monocular mode).
    sight::navigation::openvslam::init_params m_initializer_parameters;

    /// Tracking mode : MONO, STEREO, DEPTH.
    tracking_mode m_tracking_mode {tracking_mode::mono};

    /// Target width when downsampling is required.
    std::size_t m_down_sample_width {0};

    /// Mutex to lock m_slamSystem.
    std::mutex m_slam_lock;

    /// If localization mode is enable
    bool m_localization {false};

    /// Unique pointer to SLAM system.
    std::unique_ptr< ::openvslam::system> m_slam_system {nullptr};

    /// Pointer to a publisher class to get current frame.
    std::shared_ptr< ::openvslam::publish::frame_publisher> m_ovs_frame_publisher {nullptr};

    /// Pointer to a publisher class to get current map.
    std::shared_ptr< ::openvslam::publish::map_publisher> m_ovs_map_publisher {nullptr};

    /// Path to the vocabulary (could be set only once).
    std::string m_vocabulary_path;

    /// To show detected features in an external view (using cv::imshow).
    bool m_show_features {false};

    /// Switch between local or global map (pushed in m_pointcloud).
    bool m_local_map {false};

    ///Calls asynchronously updatePointCloud each 1sec.
    core::thread::timer::sptr m_timer;

    /// Worker for pointcloud update
    core::thread::worker::sptr m_pointcloud_worker;

    /// Pause state.
    bool m_is_paused {false};

    /// Stores the filepath to save map.
    std::string m_save_map_path;

    /// Stores the folder where to save trajectories files.
    core::location::single_file::sptr m_trajectories_save_path;

    /// Stores the trajectories format ("KITTI" or "TUM" are internal formats in openvslam).
    /// This is only used when saving trajectories at stop.
    /// KITTI = matrices , TUM = vectors & quaternions.
    std::string m_trajectories_format {"KITTI"};

    /// Stores the current number of landmarks in the map. (Only used in updatePointCloud thread).
    unsigned int m_number_of_landmarks {0};

    static constexpr std::string_view VIDEOPOINTS_INPUT     = "videoPoint";
    static constexpr std::string_view CAMERA_MATRIXTL_INOUT = "cameraMatrixTL";
    static constexpr std::string_view TIMELIN_E2_INPUT      = "timeline2";
    static constexpr std::string_view CAMERA_INPUT          = "camera";
    static constexpr std::string_view SCALE_INPUT           = "scale";
    static constexpr std::string_view POINTCLOUD_OUTPUT     = "pointCloud";

    sight::data::ptr<sight::data::camera, sight::data::access::in> m_camera {this, CAMERA_INPUT};
    sight::data::ptr<sight::data::frame_tl, sight::data::access::in> m_timeline2 {this, TIMELIN_E2_INPUT, true};
    sight::data::ptr<sight::data::matrix_tl, sight::data::access::inout> m_camera_matrix_tl {this,
                                                                                             CAMERA_MATRIXTL_INOUT,
                                                                                             true
    };
    sight::data::ptr<sight::data::mesh, sight::data::access::out> m_point_cloud {this, POINTCLOUD_OUTPUT, true};
    sight::data::ptr<sight::data::real, sight::data::access::in> m_scale {this, SCALE_INPUT, true};
};

} // namespace sight::module::navigation::openvslam
