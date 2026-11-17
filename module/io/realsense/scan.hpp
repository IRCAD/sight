/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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
#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/string.hpp>
#include <data/ptr.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/mesh.hpp>

#include <io/__/service/rgbd_grabber.hpp>

#include <librealsense2/rs.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace sight::module::io::realsense
{

/**
 * @brief  RealSense Camera Grabber
 *
 * This service grabs the depth, the color frame, and the pointcloud from a compatible device (Realsense D400 cameras).
 * The frames are pushed into timelines.
 * The pointloud is pushed into a data::mesh and updated each time.
 *
 * A Complete documentation about RealSense camera can be found here:
 * https://github.com/IntelRealSense/librealsense/blob/master/doc/readme.md
 *
 * \b Tags: FILE, DEVICE
 *
 * @section Signals Signals
 * - \b distance_computed(double): Signal sent when the distance (at the center of depth map) is computed.
 *
 * @section Slots Slots
 * - \b start_camera() : This slot is called to initialize and start camera (restart camera if is already started).
 * - \b stop_camera()  : This slot is called to stop camera streams.
 * - \b pause_camera() : This slot is called to pause/unpause the camera streams.
 * - \b record(): This slot is called to record current device in a .bag file.
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="videoGrabber" type ="sight::module::io::realsense::scan" auto_connect="false">
        <timeline depth="..." />
        <timeline image="..." />
        <output pointcloud="..." />
        <config camera_set="..." fps="30" colorW="1280" colorH="720" depthW="1280" depthH="720" switch_to_ir="false"
 * preset="..."
            align_to="Color"/>
        <recordFile>/path/to/the/file.bag</recordFile>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b timeline.depth [sight::data::frame_tl]: Frame timeline of the depth video.
 * - \b timeline.image [sight::data::frame_tl]: Frame timeline of the color video.
 * - \b config.camera_set [sight::data::camera_set]: Camera series that will contain device camera information.
 *
 * @subsection Output Output
 * - \b output.pointcloud [sight::data::mesh]: pointcloud computed from depth map. (optional)
 *
 * @subsection Configuration Configuration
 * - \b fps: desired framerate (value can be [6-15-25-30-60-90]), note that fps is correlated to resolution (default
 * 30).
 * - \b depthW: desired depth frame width (default: 1280, max: 1280, min: 480) (optional).
 * - \b depthH:  desired depth frame height (default: 720, max: 720, min: 270) (optional).
 * - \b colorW: desired color frame width (default: 1280, max: 1920, min: 320) (optional).
 * - \b colorH:  desired color frame height (default: 720, max: 1080, min: 180) (optional).
 * - \b switchToIR: push infrared frame in color TL (default false) (optional)
 * - \b IREmitter: enable infrared emitter (default true) (optional)
 * - \b alignTo: align each frames to the chosen one, values can be: None (default), Color, Depth, Infrared (optional).
 * - \b preset: (advanced option): load a json preset ( overwrite previous resolution values) (optional).
 *   - Default: Default preset
 *   - HighResHighAccuracy
 *   - HighResHighDensity
 *   - HighResMidDensity
 *   - MidResHighAccuracy
 *   - MidResHighDensity
 *   - MidResMidDensity
 *   - LowResHighAccuracy
 *   - LowResHighDensity
 *   - LowResMidDensity
 *   - Hand
 *   - ShortRange
 *   - BodyScan
 *   - RemoveIR
 *
 * To add a custom preset please see the README.md in videoRealsense/rc.
 *
 * Fps is strongly related to resolution ( width x height).
 * Here's a list of supported fps and resolutions (on D415 Camera):
 * - \b Depth Camera (stereo infrared)
 *   - 480 * 270, available fps [6, 15 ,30, 60, 90].
 *   - 640 * 360, available fps [6, 15 ,30, 60, 90].
 *   - 640 * 480, available fps [6, 15 ,30, 60, 90].
 *   - 848 * 480, available fps [6, 15 ,30, 60, 90].
 *   - 960 * 540, available fps [6, 15 ,30].
 *   - 1280 * 720, available fps [6, 15 ,30].
 * - \b Color Camera
 *   - 320 * 180, available fps [6, 15, 30, 60].
 *   - 320 * 240, available fps [6, 15, 30, 60].
 *   - 424 * 240, available fps [6, 15, 30, 60].
 *   - 640 * 360, available fps [6, 15 ,30, 60].
 *   - 640 * 480, available fps [6, 15 ,30, 60].
 *   - 848 * 480, available fps [6, 15 ,30, 60].
 *   - 960 * 540, available fps [6, 15 ,30].
 *   - 1280 * 720, available fps [6, 15 ,30].
 *   - 1920 * 1080, available fps [6, 15 ,30].
 *
 * For more convenience we set the same fps for both color and depth streams.
 * - \b recordFile (optional): path & filename where recording will be saved.
 *
 * @subsection ConfigBindings Configuration data bindings
 * Configuration inputs accept literals or shared data objects. The values below are the defaults when no value is
 * provided.
 * - \b config.enable_holes_filling [sight::data::boolean]: enable filling holes in the depth image (default: false).
 * - \b config.preset [sight::data::string]: load a named JSON preset, overriding previous resolution values
 *   (default: empty, built-in default preset). Available presets: Default, HighResHighAccuracy, HighResHighDensity,
 *   HighResMidDensity, MidResHighAccuracy, MidResHighDensity, MidResMidDensity, LowResHighAccuracy,
 *   LowResHighDensity, LowResMidDensity, Hand, ShortRange, BodyScan and RemoveIR. See the videoRealsense/rc
 *   README to add a custom preset.
 * - \b config.align_to [sight::data::string]: align each frame to None (default), Color, Depth or Infrared.
 * - \b config.ir_emitter [sight::data::boolean]: enable the infrared emitter (default: true).
 * - \b config.switch_to_ir [sight::data::boolean]: send the infrared frame to the color timeline instead of the
 *   color frame (default: false).
 * - \b config.max_range [sight::data::integer]: maximum depth value in millimeters (default: 65535; values above
 *   65535 are rejected).
 * - \b config.min_range [sight::data::integer]: minimum depth value in millimeters (default: 0; negative values are
 *   rejected).
 * - \b config.enable_spacial [sight::data::boolean]: enable the spatial depth post-processing filter
 *   (default: false).
 * - \b config.enable_temporal [sight::data::boolean]: enable the temporal depth post-processing filter
 *   (default: false).
 * - \b config.spacial_magnitude [sight::data::integer]: number of spatial filter iterations; accepted range [1, 5]
 *   (default: 2).
 * - \b config.spacial_smooth_alpha [sight::data::real]: spatial filter exponential moving average factor. Alpha 1
 *   disables smoothing; lower values increase it. Accepted range [0.25, 1] (default: 0.5).
 * - \b config.spacial_smooth_delta [sight::data::integer]: spatial filter step-size boundary used to preserve edges;
 *   accepted range [1, 50] (default: 20).
 * - \b config.spacial_hole_filling [sight::data::integer]: spatial filter horizontal hole-filling mode, accepted
 *   range [0, 5] (default: 0): none, then 2, 4, 8, 16 or unlimited pixels. This heuristic targets minor artifacts
 *   with minimal performance impact.
 * - \b config.temporal_smooth_alpha [sight::data::real]: temporal filter exponential moving average factor. Alpha 1
 *   disables smoothing; lower values increase it. Accepted range [0, 1] (default: 0.4).
 * - \b config.temporal_smooth_delta [sight::data::integer]: temporal filter step-size boundary used to preserve
 *   surfaces and edges; accepted range [1, 100] (default: 20).
 * - \b config.temporal_persistency [sight::data::integer]: temporal persistency rule, accepted range [0, 8]
 *   (default: 3). Rules decide when a missing pixel reuses its last valid value: disabled; valid in 8/8; valid in 2
 *   of the last 3, 4 or 8 frames; valid in 1 of the last 2, 5 or 8 frames; or persist indefinitely (most aggressive).
 * - \b config.hole_filling [sight::data::integer]: choose how invalid pixels are filled, accepted values [0, 2]
 *   (default: 1): fill from left, from the furthest neighboring pixel, or from the nearest neighboring pixel.
 *
 * See the RealSense post-processing filter documentation for details on the spatial and temporal filters:
 * https://github.com/IntelRealSense/librealsense/blob/master/doc/post-processing-filters.md
 */

class scan : public sight::io::service::rgbd_grabber
{
public:

    struct signals
    {
        using distance_computed_t = core::com::signal<void (double)>;
        using device_played_t     = core::com::signal<void (void)>;
        using file_played_t       = core::com::signal<void (void)>;
        static inline const signal_key_t DISTANCE_COMPUTED = "distance_computed";
        static inline const signal_key_t DEVICE_PLAYED     = "device_played";
        static inline const signal_key_t FILE_PLAYED       = "file_played";
    };

    struct slots
    {
        static inline const slot_key_t CONFIGURE_RECORDING_PATH           = "configure_recording_path";
        static inline const slot_key_t RECORD                             = "record";
        static inline const slot_key_t APPLY_CONFIG_PRESET                = "apply_config_preset";
        static inline const slot_key_t APPLY_CONFIG_ALIGN_TO              = "apply_config_align_to";
        static inline const slot_key_t APPLY_CONFIG_IR_EMITTER            = "apply_config_ir_emitter";
        static inline const slot_key_t APPLY_CONFIG_MAX_RANGE             = "apply_config_max_range";
        static inline const slot_key_t APPLY_CONFIG_MIN_RANGE             = "apply_config_min_range";
        static inline const slot_key_t APPLY_CONFIG_ENABLE_SPACIAL        = "apply_config_enable_spacial";
        static inline const slot_key_t APPLY_CONFIG_ENABLE_TEMPORAL       = "apply_config_enable_temporal";
        static inline const slot_key_t APPLY_CONFIG_SPACIAL_MAGNITUDE     = "apply_config_spacial_magnitude";
        static inline const slot_key_t APPLY_CONFIG_SPACIAL_SMOOTH_ALPHA  = "apply_config_spacial_smooth_alpha";
        static inline const slot_key_t APPLY_CONFIG_SPACIAL_SMOOTH_DELTA  = "apply_config_spacial_smooth_delta";
        static inline const slot_key_t APPLY_CONFIG_SPACIAL_HOLE_FILLING  = "apply_config_spacial_hole_filling";
        static inline const slot_key_t APPLY_CONFIG_TEMPORAL_SMOOTH_ALPHA = "apply_config_temporal_smooth_alpha";
        static inline const slot_key_t APPLY_CONFIG_TEMPORAL_SMOOTH_DELTA = "apply_config_temporal_smooth_delta";
        static inline const slot_key_t APPLY_CONFIG_TEMPORAL_PERSISTENCY  = "apply_config_temporal_persistency";
        static inline const slot_key_t APPLY_CONFIG_HOLE_FILLING          = "apply_config_hole_filling";
    };

    SIGHT_DECLARE_SERVICE(scan, sight::io::service::rgbd_grabber);

    /// Signal send when Distance is computed.
    typedef signals::distance_computed_t distance_computed_t;

    /// Signal send when stream from a realsense device, can be useful to enable/disable some gui actions.
    typedef signals::device_played_t device_played_signal_t;

    /// Signal send when stream from a file (.bag), can be useful to enable/disable some gui actions.
    typedef signals::file_played_t file_played_signal_t;

    /// Constructor. Initializes signals/slots.
    scan() noexcept;

    /// Destructor. Calls stopCamera()
    ~scan() noexcept override;

protected:

    sight::service::connections_t auto_connections() const override;

    /// Starts the service, get the timelines and set the outputs.
    void starting() override;

    /// Shutdowns the streams by calling stopCamera().
    void stopping() override;

    /// Restarts the service (stopCamera() & startCamera()).
    void updating() override;

    /// Parses the configuration.
    void configuring() override;

private:

    /// Apply settings from bound data at startup and after external modifications.
    void apply_config_preset();
    void apply_config_align_to();
    void apply_config_ir_emitter();
    void apply_config_max_range();
    void apply_config_min_range();
    void apply_config_spacial_magnitude();
    void apply_config_spacial_smooth_alpha();
    void apply_config_spacial_smooth_delta();
    void apply_config_spacial_hole_filling();
    void apply_config_temporal_smooth_alpha();
    void apply_config_temporal_smooth_delta();
    void apply_config_temporal_persistency();
    void apply_config_hole_filling();

    // Internal structures

    /// min and max depth range (from realsense-viewer application).
    static const int s_MAX_DEPTH_RANGE = 65535;
    static const int s_MIN_DEPTH_RANGE = 0;

    /**
     * @brief camera_settings is a structure that handles parameters such as resolution of streams,
     *  fps, and other options.
     */
    struct camera_settings
    {
        int fps               = 30;    ///< Default FPS of all streams.
        int colorH            = 720;   ///< Default Height of color/infrared streams.
        int colorW            = 1280;  ///< Default Width of color/infrared streams.
        int depthH            = 720;   ///< Default Height of depth stream.
        int depthW            = 1280;  ///< Default Width of depth stream.
        ::fs::path presetPath = "";    ///< Path to the preset to load (default none).
        bool needHardReset    = false; ///< if device needs to be hard-reset before at stop.
        ///If frames needs to be aligned on in a particular STREAM.
        /// Value can be RS2_STREAM_COUNT = No alignment, RS_STREAM_DEPTH, RS_STREAM_COLOR, RS_STREAM_INFRARED, others
        // values are ignored.
        rs2_stream streamToAlignTo = RS2_STREAM_COUNT;

        /// Re-init all values to default.
        void reset()
        {
            fps    = 30;
            colorH = 720;
            colorW = 1280;
            depthH = 720;
            depthW = 1280;
            presetPath.clear();
            needHardReset   = false;
            streamToAlignTo = RS2_STREAM_COUNT;
        }
    };

    /// On which frame map the pointcloud.
    typedef enum PointcloudColormap
    {
        COLOR,
        DEPTH,
        INFRARED
    } pointcloud_colormap_enum_t;

    // Overriden functions/slots

    /// SLOT : Initializes and starts the streams. Restarts the streams if already started.
    void start_camera() override;

    /// SLOT : Stops to grab frames.
    void stop_camera() override;

    /// SLOT : Pause the grabbing
    void pause_camera() override;

    /// Does nothing (re-implemented from grabber)
    void toggle_loop_mode() override
    {
    }

    /// Does nothing (re-implemented from grabber)
    void set_position(int64_t /*position*/) override
    {
    }

    ///SLOT: Start recording session, also open dialog to select filename if m_recordingFileName is not set.
    void record();

    /// SLOT: Configure recording filename path.
    void configureRecordingPath();

    // Internal functions

    /**
     * @brief selectDevice is a private function called on start of the service, it will detect if realsense cameras
     * are plugged in, and show a dialog to user if several realsense cameras are detected.
     * @note This function may erase previous choice made in module::io::qt::camera.
     * @todo This function should be merged in module::io::qt::camera, or in a more generic "camera selector".
     * @return a string containing the serial number of selected camera.
     */
    static std::string selectDevice();

    /**
     * @brief initialize is called at "start_camera" and will do the initialization of sight data.
     * @param[in] _profile: realsense pipeline_profile created in 'startCamera'
     */
    void initialize(const rs2::pipeline_profile& _profile);

    /**
     * @brief  grabbing thread (launched from startCamera)
     */
    void grab();

    /**
     * @brief loadPresets scans "presets" folder and generate the map
     * @param[in] _path: preset path (module/io/realsense/rc/presets)
     */
    void loadPresets(const std::filesystem::path& _path);

    /**
     * @brief updateAlignment changes the reference frame where all frames will be aligned.
     * @param[in] _alignTo: reference frame name in string can be None, Color, Infrared or Depth.
     * All others values are ignored.
     * @return bool: true if new alignment is registered, false if _alignTo value is ignored (avoid an unnecessary
     * call to stop/start).
     */
    bool updateAlignment(const std::string& _alignTo);

    /**
     * @brief setMinMaxRange updates the min/max range of the depth stream. Need advanced mode.
     */
    void setMinMaxRange();

    /**
     * @brief callback called to push color/infrared frame in timeline.
     * @param[in] _buffer: pointer to the uint8_t buffer.
     */
    void onCameraImage(const uint8_t* _buffer);

    /**
     * @brief callback called to push depth frame in timeline.
     * @param[in] _buffer: pointer to the uint16_t depth buffer.
     */
    void onCameraImageDepth(const uint16_t* _buffer);

    /**
     * @brief callback called to update the pointcloud.
     * @param[in] _pc : realsense points.
     * @param[in] _texture : realsense video_frame used to add color on points.
     */
    void onPointCloud(const rs2::points& _pc, const rs2::video_frame& _texture);

    // Slots

    /// Pop a message dialog to the user with _message.
    /// Title is set to "RealSense Error".
    static void popMessageDialog(const std::string& _message);

    // Members

    std::unique_ptr<rs2::pipeline> m_pipe; ///< RealSense Pipeline

    std::string m_depthKey; ///< Depth frame key.
    std::string m_colorKey; ///< Color frame key.

    /// Grabber thread
    std::thread m_thread;

    /// Determine depth value corresponding to one meter.
    float m_depthScale = 0.F;

    /// Handle on which frame the pointcloud is mapped.
    pointcloud_colormap_enum_t m_pointcloudColorMap = PointcloudColormap::COLOR;

    /// Struct that contains basic camera settings (fps, resolution, preset, ...).
    camera_settings m_camera_settings;

    /// Contain the current realsense device (needed when parameters changed live).
    rs2::device m_currentDevice;

    /// Map of json presets (in RC folder).
    std::map<std::string, ::fs::path> m_jsonPresets;

    /// Current device ID (to sort cameras if multiple realsense are plugged-in).
    std::string m_deviceID;

    /// True when grabbing thread is running, set to false to stop the thread.
    std::atomic_bool m_running {false};

    /// True to pause the streaming, false otherwise
    std::atomic_bool m_pause {false};

    /// True when the first real frame has been grabbed. Set the point cloud output.
    std::atomic_bool m_grabbingStarted {false};

    /// True if device needs to be recorded.
    bool m_record {false};

    /// Name of the recording file.
    std::string m_recordingFileName;

    /// Name of the file to playback.
    std::string m_playbackFileName;

    /// Enable if camera source = FILE, enable playing from .bag files.
    bool m_playbackMode {false};

    /// Condition Variable used to pause grabbing thread
    std::condition_variable m_pauseConditionVariable;

    /// Mutex used for the Condition Variable
    std::mutex m_pauseMutex;

    static constexpr std::string_view s_CAMERA_SET_INOUT = "config.camera_set";

    static constexpr std::string_view s_POINTCLOUD_OUTPUT = "output.pointcloud";
    data::mesh::sptr m_pointCloud;

    ptr_inout<sight::data::boolean> m_enable_holes_filling {this, "config.enable_holes_filling", false};
    ptr_inout<sight::data::string> m_preset {this, "config.preset", ""};
    ptr_inout<sight::data::string> m_align_to {this, "config.align_to", "None"};
    ptr_inout<sight::data::boolean> m_ir_emitter {this, "config.ir_emitter", true};
    ptr_inout<sight::data::boolean> m_switch_to_ir {this, "config.switch_to_ir", false};
    ptr_inout<sight::data::integer> m_max_range {this, "config.max_range", s_MAX_DEPTH_RANGE};
    ptr_inout<sight::data::integer> m_min_range {this, "config.min_range", s_MIN_DEPTH_RANGE};
    ptr_inout<sight::data::boolean> m_enable_spacial {this, "config.enable_spacial", false};
    ptr_inout<sight::data::boolean> m_enable_temporal {this, "config.enable_temporal", false};
    ptr_inout<sight::data::integer> m_spacial_magnitude {this, "config.spacial_magnitude", 2};
    ptr_inout<sight::data::real> m_spacial_smooth_alpha {this, "config.spacial_smooth_alpha", 0.5};
    ptr_inout<sight::data::integer> m_spacial_smooth_delta {this, "config.spacial_smooth_delta", 20};
    ptr_inout<sight::data::integer> m_spacial_hole_filling {this, "config.spacial_hole_filling", 0};
    ptr_inout<sight::data::real> m_temporal_smooth_alpha {this, "config.temporal_smooth_alpha", 0.4};
    ptr_inout<sight::data::integer> m_temporal_smooth_delta {this, "config.temporal_smooth_delta", 20};
    ptr_inout<sight::data::integer> m_temporal_persistency {this, "config.temporal_persistency", 3};
    ptr_inout<sight::data::integer> m_hole_filling {this, "config.hole_filling", 1};
    ptr_in<data::camera> m_camera {this, s_CAMERA_INPUT, true};

    ptr_inout<data::camera_set> m_camera_set {this, s_CAMERA_SET_INOUT, true};

    ptr_out<data::mesh> m_pointCloudOutput {this, s_POINTCLOUD_OUTPUT, true};
};

} //namespace sight::module::io::realsense
