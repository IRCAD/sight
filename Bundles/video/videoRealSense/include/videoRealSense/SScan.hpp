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

#include <boost/filesystem.hpp>

#include <librealsense2/rs.hpp>

namespace fs = ::boost::filesystem;

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
 * \b Tags: DEVICE
 *
 * @section Signals Signals
 * - \b distanceComputed(double): Signal sent when the distance (at the center of depth map) is computed.
 *
 * @section Slots Slots
 * - \b startCamera() : This slot is called to initialize and start camera (restart camera if is already started).
 * - \b stopCamera()  : This slot is called to stop camera streams.
 * - \b pauseCamera() : This slot is called to pause/unpause the camera streams.
 * - \b setBoolParameter(bool value, std::string key) : Slot called when a boolean parameter changes:
 *   - key 'switchToIR' : switch the color stream by infrared stream if true.
 *   - key 'IREmitter' : enable/disable the IR Emitter.
 * - \b setIntParameter(int value, std::string key): Slot called when a integer parameter changes:
 *   - key 'minRange' : min value of depth range (default 0)
 *   - key 'maxRange' : max value of depth range (default 65535)
 * -\b setEnumParameter(std::string value, std::string key) : Slot called when a enumeration parameter changes:
 *   - key 'preset' : preset name to load. (see 'preset' in subsection \ref Configuration below).
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="videoGrabber" type ="::videoRealSense::SScan" autoConnect="no">
        <inout key="depthTL" uid="..." />
        <inout key="frameTL" uid="..." />
        <out key="pointcloud" uid="..." />
        <inout key="cameraSeries" uid="..." />
        <config fps="30" colorW="1280" colorH="720" depthW="1280" depthH="720" switchToIR="true/false" preset="..." />
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
 * - \b swithToIR: push infrared frame in color TL (default false) (optional)
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
 */

class VIDEOREALSENSE_CLASS_API SScan : public ::arServices::IRGBDGrabber
{
public:

    fwCoreServiceClassDefinitionsMacro( (SScan)(::arServices::IRGBDGrabber) )

    /// Signal send when Distance is computed.
    typedef ::fwCom::Signal< void (double) > DistanceComputedSignalType;

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
    static const int maxDepthRange = 65535;
    static const int minDepthRange = 0;

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
        int maxRange          = maxDepthRange; ///< max depth range.
        int minRange          = minDepthRange; ///< min depth range.
        bool needHardReset    = false; ///< if device needs to be hard-reset before at stop.

        /// Re-init all values to default.
        void reset()
        {
            fps    = 30;
            colorH = 720;
            colorW = 1280;
            depthH = 720;
            depthW = 1280;
            presetPath.clear();
            irEmitter     = true;
            maxRange      = maxDepthRange;
            minRange      = minDepthRange;
            needHardReset = false;

        }
    };

    /// On which frame map the pointcloud.
    typedef enum PointcloudColormap
    {
        COLOR,
        DEPTH,
        INFRARED,
    } PointcloudColormapEnumType;

    // Overrided functions/slots

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
    void loadPresets(const ::boost::filesystem::path& _path);

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

};
} //namespace videoRealSense
