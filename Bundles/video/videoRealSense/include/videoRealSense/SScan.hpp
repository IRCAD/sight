/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "videoRealSense/config.hpp"

#include <arData/FrameTL.hpp>

#include <arServices/IRGBDGrabber.hpp>

#include <fwData/Mesh.hpp>

#include <filesystem>

#include <librealsense2/rs.hpp>

namespace fs = std::filesystem;

namespace videoRealSense
{

/**
 * @brief  RealSense Camera Grabber
 *
 * This service grabs the depth, the color frame, and the poincloud from a compatible device (Realsense D400 cameras).
 * The frames are pushed into timelines.
 * The pointloud is pushed into a ::fwData::Mesh and updated each time.
 *
 * A Complete documentation about RealSense camera can be found here:
 * https://github.com/IntelRealSense/librealsense/blob/master/doc/readme.md
 *
 * \b Tags: FILE, DEVICE
 *
 * @section Signals Signals
 * - \b distanceComputed(double): Signal sent when the distance (at the center of depth map) is computed.
 *
 * @section Slots Slots
 * - \b startCamera() : This slot is called to initialize and start camera (restart camera if is already started).
 * - \b stopCamera()  : This slot is called to stop camera streams.
 * - \b pauseCamera() : This slot is called to pause/unpause the camera streams.
 * - \b record(): This slot is called to record current device in a .bag file.
 * - \b setBoolParameter(bool value, std::string key) : Slot called when a boolean parameter changes:
 *   - key 'switchToIR': switch the color stream by infrared stream if true.
 *   - key 'IREmitter': enable/disable the IR Emitter.
 *   - key 'enableSpacial': enable/disable spacial filter. For the documentation of the filters, please refer to
 *       https://github.com/IntelRealSense/librealsense/blob/master/doc/post-processing-filters.md
 *   - key 'enableTemporal': enable/disable temporal filter
 *   - key 'enableHolesFilling': enable/disable holes filling filter
 * - \b setIntParameter(int value, std::string key): Slot called when a integer parameter changes:
 *   - key 'minRange' : min value of depth range (default 0)
 *   - key 'maxRange' : max value of depth range (default 65535)
 *   - key 'spacialMagnitude': set the number of iteration of the spacial filter [1-5]
 *   - key 'spacialSmoothDelta': step-size boundary. Establishes the threshold used to preserve "edges" [1-50]
 *   - key 'spacialHoleFilling': in-place heuristic symmetric hole-filling mode applied horizontally during the filter
 *        passes. Intended to rectify minor artefacts with minimal performance impact ([0-5] range mapped to
 *        [none,2,4,8,16,unlimited] pixels).
 *   - key 'temporalSmoothDelta': step-size boundary. Establishes the threshold used to preserve surfaces (edges)
 *      [1-100]
 *   - key 'temporalPersistency': A set of predefined rules (masks) that govern when missing pixels will be replace with
 *      the last valid value so that the data will remain persistent over time:
 *      - Disabled: Persistency filter is not activated and no hole filling occurs.
 *      - Valid in 8/8: Persistency activated if the pixel was valid in 8 out of the last 8 frames
 *      - Valid in 2/last 3: Activated if the pixel was valid in two out of the last 3 frames
 *      - Valid in 2/last 4: Activated if the pixel was valid in two out of the last 4 frames
 *      - Valid in 2/8: Activated if the pixel was valid in two out of the last 8 frames
 *      - Valid in 1/last 2: Activated if the pixel was valid in one of the last two frames
 *      - Valid in 1/last 5: Activated if the pixel was valid in one out of the last 5 frames
 *      - Valid in 1/last 8: Activated if the pixel was valid in one out of the last 8 frames
 *      - Persist Indefinitely: Persistency will be imposed regardless of the stored history: most aggressive filtering
 *   - key 'holeFilling':  control the data that will be used to fill the invalid pixels, [0-2] enumerated:
 *      - fill_from_left: Use the value from the left neighbor pixel to fill the hole
 *      - farest_from_around: Use the value from the neighboring pixel which is furthest away from the sensor
 *      - nearest_from_around: Use the value from the neighboring pixel closest to the sensor
 * - \b setEnumParameter(std::string value, std::string key) : Slot called when a enumeration parameter changes:
 *   - key 'preset' : preset name to load. (see 'preset' in subsection \ref Configuration below).
 *  -  key 'alignTo': used to change the frames alignement, all frames can be aligned on (None (default), Color, Depth
 * or Infrared Streams).
 *    (see also 'alignTo is subsection \ref Configuration below).'
 * - \b setDoubleParameter(vouble value, std::string key): Slot called when a double parameter changes:
 *   - key 'spacialSmoothAlpha': Alpha factor in an exponential moving average with Alpha=1: no filter . Alpha = 0:
 *     infinite filter [0.25-1]
 *   - key 'temporalSmoothAlpha': Alpha factor in an exponential moving average with Alpha=1: no filter . Alpha = 0:
 *     infinite filter [0-1]
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="videoGrabber" type ="::videoRealSense::SScan" autoConnect="no">
        <inout key="depthTL" uid="..." />
        <inout key="frameTL" uid="..." />
        <out key="pointcloud" uid="..." />
        <inout key="cameraSeries" uid="..." />
        <config fps="30" colorW="1280" colorH="720" depthW="1280" depthH="720" switchToIR="true/false" preset="..."
 * alignTo="Color"/>
        <recordFile>/path/to/the/file.bag</recordFile>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b depthTL [::arData::FrameTL]: Frame timeline of the depth video.
 * - \b frameTL [::arData::FrameTL]: Frame timeline of the color video.
 * - \b cameraSeries [::arData::CameraSeries]: Camera series that will contain device camera information.
 *
 * @subsection Output Output
 *- \b pointcloud [::fwData::Mesh]: pointcloud computed from depth map. (optional)
 * -\b distance [::fwData::Float]: distance (in mm) at center pixel. (optional)
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
 * - \b alignTo: align each frames to the chosen one, values can be: None (default), Color, Depth, Infrared (optionnal).
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
 *
 *
 * - \b recordFile (optionnal): path & filename where recording will be saved.
 */

class VIDEOREALSENSE_CLASS_API SScan : public ::arServices::IRGBDGrabber
{
public:

    fwCoreServiceMacro(SScan, ::arServices::IRGBDGrabber);

    /// Signal send when Distance is computed.
    typedef ::fwCom::Signal< void (double) > DistanceComputedSignalType;

    /// Signal send when stream from a realsense device, can be useful to enable/disable some gui actions.
    typedef ::fwCom::Signal< void (void) > DevicePlayedSignalType;

    /// Signal send when stream from a file (.bag), can be useful to enable/disable some gui actions.
    typedef ::fwCom::Signal< void (void) > FilePlayedSignalType;

    /// Constructor. Initializes signals/slots.
    VIDEOREALSENSE_API SScan() noexcept;

    /// Destructor. Calls stopCamera()
    VIDEOREALSENSE_API virtual ~SScan() noexcept override;

protected:

    /// Starts the service, get the timelines and set the outputs.
    VIDEOREALSENSE_API virtual void starting() override;

    /// Shutdowns the streams by calling stopCamera().
    VIDEOREALSENSE_API virtual void stopping() override;

    /// Restarts the service (stopCamera() & startCamera()).
    VIDEOREALSENSE_API virtual void updating() override;

    /// Parses the configuration.
    VIDEOREALSENSE_API virtual void configuring() override;

private:

    // Internal structures

    /// min and max depth range (from realsense-viewer application).
    static const int s_MAX_DEPTH_RANGE = 65535;
    static const int s_MIN_DEPTH_RANGE = 0;

    /**
     * @brief CameraSettings is a structure that handles parameters such as resolution of streams,
     *  fps, and other options.
     */
    struct CameraSettings
    {
        int fps               = 30; ///< Default FPS of all streams.
        int colorH            = 720; ///< Default Height of color/infrared streams.
        int colorW            = 1280; ///< Default Width of color/infrared streams.
        int depthH            = 720; ///< Default Height of depth stream.
        int depthW            = 1280; ///< Default Width of depth stream.
        ::fs::path presetPath = ""; ///< Path to the preset to load (default none).
        bool irEmitter        = true; ///< enable/disable ir emitter (default on).
        int maxRange          = s_MAX_DEPTH_RANGE; ///< max depth range.
        int minRange          = s_MIN_DEPTH_RANGE; ///< min depth range.
        bool needHardReset    = false; ///< if device needs to be hard-reset before at stop.
        ///If frames needs to be aligned on in a particular STREAM.
        /// Value can be RS2_STREAM_COUNT = No alignement, RS_STREAM_DEPTH, RS_STREAM_COLOR, RS_STREAM_INFRARED, others
        // values are ignored.
        ::rs2_stream streamToAlignTo = RS2_STREAM_COUNT;

        /// Re-init all values to default.
        void reset()
        {
            fps    = 30;
            colorH = 720;
            colorW = 1280;
            depthH = 720;
            depthW = 1280;
            presetPath.clear();
            irEmitter       = true;
            maxRange        = s_MAX_DEPTH_RANGE;
            minRange        = s_MIN_DEPTH_RANGE;
            needHardReset   = false;
            streamToAlignTo = RS2_STREAM_COUNT;

        }
    };

    /// Store the depth frame filters settings
    /// (see https://github.com/IntelRealSense/librealsense/blob/master/doc/post-processing-filters.md)
    struct FilterSettings
    {
        bool enableSpacial{false}; ///< Enable Spatial Edge-Preserving filter
        bool enableTemporal{false}; ///< Enable Temporal filter
        bool enableHolesFilling{false}; ///< Enable Holes Filling filter

        // spacial filter settings
        std::uint8_t spacialMagnitude{2}; ///<  Number of filter iterations [1-5]
        /// Alpha factor in an exponential moving average with Alpha=1 - no filter. Alpha = 0 - infinite filter [0.25-1]
        float spacialSmoothAlpha{0.5f};
        /// Step-size boundary. Establishes the threshold used to preserve "edges" [1-50]
        std::uint8_t spacialSmoothDelta{20};
        /// An in-place heuristic symmetric hole-filling mode applied horizontally during the filter passes. Intended to
        /// rectify minor artefacts with minimal performance impact
        /// [0-5] range mapped to [none,2,4,8,16,unlimited] pixels.
        std::uint8_t spacialHoleFilling{0};

        // temporal filter settings
        /// Alpha factor in an exponential moving average with Alpha=1 - no filter . Alpha = 0 - infinite filter [0-1]
        float temporalSmoothAlpha{0.4f};
        /// Step-size boundary. Establishes the threshold used to preserve surfaces (edges) [1-100]
        std::uint8_t temporalSmoothDelta{20};
        /// Persistency index [0-8]: set of predefined rules (masks) that govern when missing pixels will be replace
        /// with the last valid value so that the data will remain persistent over time.
        std::uint8_t temporalPersistency{3};

        // holes filling settings
        /// Control the data that will be used to fill the invalid pixels [0-2]
        std::uint8_t holeFilling{1};
    };

    /// On which frame map the pointcloud.
    typedef enum PointcloudColormap
    {
        COLOR,
        DEPTH,
        INFRARED,
    } PointcloudColormapEnumType;

    // Overriden functions/slots

    /// SLOT : Initializes and starts the streams. Restarts the streams if already started.
    virtual void startCamera() override;
    /// SLOT : Stops to grab frames.
    virtual void stopCamera() override;
    /// SLOT : Pause the grabbing
    virtual void pauseCamera() override;

    /// Does nothing (re-implemented from IGrabber)
    virtual void toggleLoopMode() override
    {

    }
    /// Does nothing (re-implemented from IGrabber)
    virtual void setPosition(int64_t) override
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
     * @note This function may erase previous choice made in videoQt::SCamera.
     * @todo This function should be merged in videoQt::SCamera, or in a more generic "camera selector".
     * @return a string containing the serial number of selected camera.
     */
    std::string selectDevice();

    /**
     * @brief initialize is called at "startCamera" and will do the initialization of sight data.
     * @param[in] _profile: realsense pipeline_profile created in 'startCamera'
     */
    void initialize(const ::rs2::pipeline_profile& _profile);

    /**
     * @brief  grabbing thread (launched from startCamera)
     */
    void grab();

    /**
     * @brief loadPresets scans "presets" folder and generate the map
     * @param[in] _path: preset path (videoRealSense/rc/presets)
     */
    void loadPresets(const std::filesystem::path& _path);

    /**
     * @brief updateAlignment changes the reference frame where all frames will be aligned.
     * @param[in] _alignTo: reference frame name in string can be None, Color, Infrared or Depth.
     * All others values are ignored.
     * @return bool: true if new alignement is register, false if _alignTo value is ignored (avoid an unnecessary
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
    void onPointCloud(const ::rs2::points& _pc, const ::rs2::video_frame& _texture);

    // Slots

    ///SLOT: When "boolean" parameter changes
    void setBoolParameter(bool, std::string);

    /// SLOT: When "enum" parameter changes
    void setEnumParameter(std::string, std::string);

    ///SLOT: When "int" parameter changes
    void setIntParameter(int, std::string);

    ///SLOT: When "double" parameter changes
    void setDoubleParameter(double, std::string);

    /// Pop a message dialog to the user with _message.
    /// Title is set to "RealSense Error".
    void popMessageDialog(const std::string& _message);

    // Members

    std::unique_ptr< ::rs2::pipeline> m_pipe; ///< RealSense Pipeline

    std::string m_depthKey; ///< Depth frame key.
    std::string m_colorKey; ///< Color frame key.

    /// Timelines
    ::arData::FrameTL::sptr m_depthTimeline; ///< Depth timeline.
    ::arData::FrameTL::sptr m_colorTimeline; ///< Color timeline.

    /// Grabber thread
    std::thread m_thread;

    /// Determine depth value corresponding to one meter.
    float m_depthScale = 0.f;

    /// Handle on which frame the pointcloud is mapped.
    PointcloudColormapEnumType m_pointcloudColorMap = PointcloudColormap::COLOR;

    /// Pointer to the (optional) ::fwData::Mesh output.
    ::fwData::Mesh::sptr m_pointcloud = nullptr;

    /// Struct that contains basic camera settings (fps, resolution, preset, ...).
    CameraSettings m_cameraSettings;

    /// Struct that contains the settings to apply filter on the depth frame
    FilterSettings m_filterSettings;

    /// Contain the current realsense device (needed when parameters changed live).
    ::rs2::device m_currentDevice;

    /// Map of json presets (in RC folder).
    std::map< std::string, ::fs::path > m_jsonPresets;

    /// Current device ID (to sort cameras if multiple realsense are plugged-in).
    std::string m_deviceID;

    /// True when grabbing thread is running, set to false to stop the thread.
    std::atomic_bool m_running { false };

    /// True to pause the streaming, false otherwise
    std::atomic_bool m_pause { false };

    /// True to push infrared frames in color TL.
    std::atomic_bool m_switchInfra2Color { false };

    /// True when the first real frame has been grabbed. Set the point cloud output.
    std::atomic_bool m_grabbingStarted { false };

    /// True if device needs to be recorded.
    bool m_record { false };

    /// Name of the recording file.
    std::string m_recordingFileName;

    /// Name of the file to playback.
    std::string m_playbackFileName;

    /// Enable if camera source = FILE, enable playing from .bag files.
    bool m_playbackMode { false };

    /// Condition Variable used to pause grabbing thread
    std::condition_variable m_pauseConditionVariable;

    /// Mutex used for the Condition Variable
    std::mutex m_pauseMutex;
};
} //namespace videoRealSense
