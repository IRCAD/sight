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

#include "scan.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/runtime/path.hpp>
#include <core/type.hpp>

#include <data/matrix4.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <librealsense2/rs_advanced_mode.hpp>

#include <algorithm>
#include <fstream>
#include <thread>

namespace sight::module::io::realsense
{

namespace fwClock = core::clock;

static const std::string s_FPS           = "fps";
static const std::string s_COLOR_FRAME_H = "colorH";
static const std::string s_COLOR_FRAME_W = "colorW";
static const std::string s_DEPTH_FRAME_H = "depthH";
static const std::string s_DEPTH_FRAME_W = "depthW";
static const std::string s_PRESET        = "preset";
static const std::string s_ALIGNMENT     = "alignTo";

static const std::string s_IREMITTER    = "IREmitter";
static const std::string s_SWITCH_TO_IR = "switchToIR";

static const std::string s_ENABLE_SPACIAL_FILTER       = "enableSpacial";
static const std::string s_ENABLE_TEMPORAL_FILTER      = "enableTemporal";
static const std::string s_ENABLE_HOLES_FILLING_FILTER = "enableHolesFilling";
static const std::string s_SPACIAL_MAGNITUDE           = "spacialMagnitude";
static const std::string s_SPACIAL_SMOOTH_ALPHA        = "spacialSmoothAlpha";
static const std::string s_SPACIAL_SMOOTH_DELTA        = "spacialSmoothDelta";
static const std::string s_SPACIAL_HOLE_FILLING        = "spacialHoleFilling";
static const std::string s_TEMPORAL_SMOOTH_ALPHA       = "temporalSmoothAlpha";
static const std::string s_TEMPORAL_SMOOTH_DELTA       = "temporalSmoothDelta";
static const std::string s_TEMPORAL_PERSISTENCY        = "temporalPersistency";
static const std::string s_HOLE_FILLING                = "holeFilling";

static const core::com::slots::key_t CONFIGURE_RECORDING_PATH_SLOT = "configure_recording_path";

static const core::com::slots::key_t RECORD = "record";

static const core::com::signals::key_t DISTANCE_COMPUTED_SIG = "distance_computed";
static const core::com::signals::key_t DEVICE_PLAYED_SIG     = "device_played";
static const core::com::signals::key_t FILE_PLAYED_SIG       = "file_played";

// Determine depth value corresponding to one meter
static const float s_METERS_TO_MMS = 1000.F;

//-----------------------------------------------------------------------------

scan::scan() noexcept
{
    new_slot(CONFIGURE_RECORDING_PATH_SLOT, &scan::configureRecordingPath, this);
    new_slot(RECORD, &scan::record, this);

    new_signal<distance_computed_t>(DISTANCE_COMPUTED_SIG);
    new_signal<device_played_signal_t>(DEVICE_PLAYED_SIG);
    new_signal<file_played_signal_t>(FILE_PLAYED_SIG);
}

//-----------------------------------------------------------------------------

scan::~scan() noexcept
{
    this->scan::stopCamera();
}

//-----------------------------------------------------------------------------

void scan::starting()
{
    m_grabbingStarted = false;
}

//-----------------------------------------------------------------------------

void scan::stopping()
{
    this->stopCamera();

    // Clear the preset
    m_cameraSettings.reset();
}

//-----------------------------------------------------------------------------

void scan::configuring()
{
    const config_t configTree = this->get_config();
    const auto cfg            = configTree.get_child_optional("config.<xmlattr>.");

    if(cfg)
    {
        m_cameraSettings.fps = cfg->get<int>(s_FPS, m_cameraSettings.fps);

        m_cameraSettings.colorW = cfg->get<int>(s_COLOR_FRAME_W, m_cameraSettings.colorW);
        m_cameraSettings.colorH = cfg->get<int>(s_COLOR_FRAME_H, m_cameraSettings.colorH);

        m_cameraSettings.depthW = cfg->get<int>(s_DEPTH_FRAME_W, m_cameraSettings.depthW);
        m_cameraSettings.depthH = cfg->get<int>(s_DEPTH_FRAME_H, m_cameraSettings.depthH);

        m_switchInfra2Color        = cfg->get<bool>(s_SWITCH_TO_IR, m_switchInfra2Color);
        m_cameraSettings.irEmitter = cfg->get<bool>(s_IREMITTER, m_cameraSettings.irEmitter);

        const std::string alignTo = cfg->get<std::string>(s_ALIGNMENT, "None");
        this->updateAlignment(alignTo);
    }

    static const auto s_modulePath = core::runtime::get_module_resource_path("sight::module::io::realsense");

    // Parse presets files
    this->loadPresets(s_modulePath / "presets");
}

//-----------------------------------------------------------------------------

void scan::updating()
{
    this->stopCamera();
    this->startCamera();
}

//-----------------------------------------------------------------------------

std::string scan::selectDevice()
{
    // TODO: Check if a device is already selected (via Qt).

    // Obtain a list of devices currently present on the system
    rs2::context ctx;
    const auto devices          = ctx.query_devices();
    const uint32_t device_count = devices.size();
    std::string selectedDevice;

    // Only one device found -> select it.
    if(device_count == 1)
    {
        selectedDevice = devices[0].get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
    }
    // Several devices found -> open a selector dialog.
    else if(device_count > 1)
    {
        sight::ui::dialog::selector dial;
        dial.setTitle("Select realsense camera");

        std::vector<std::string> selections;
        selections.resize(devices.size());

        for(std::uint32_t i = 0 ; i < devices.size() ; ++i)
        {
            const std::string name   = devices[i].get_info(RS2_CAMERA_INFO_NAME);
            const std::string serial = devices[i].get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

            // Display index + name + serial (ex: 1.Intel RealSense D415(839112062452) ).
            const std::string nameserial = std::to_string(i + 1) + ". " + name + " (" + serial + ")";

            selections[i] = nameserial;
        }

        dial.set_choices(selections);

        const std::string selected = dial.show();

        // Get the index of selected camera.
        const std::size_t dot = selected.find('.');
        const auto index      = std::stoi(selected.substr(0, dot)) - 1;

        // Get associated serial numbers.
        selectedDevice = devices[static_cast<uint32_t>(index)].get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

        SIGHT_DEBUG("selecting camera: " << index);
    }

    return selectedDevice;
}

//-----------------------------------------------------------------------------

void scan::initialize(const rs2::pipeline_profile& _profile)
{
    const auto depthStream    = _profile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
    const auto colorStream    = _profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
    const auto infraredStream = _profile.get_stream(RS2_STREAM_INFRARED).as<rs2::video_stream_profile>();

    std::stringstream str;
    str << "-- fps : " << depthStream.fps() << std::endl;
    str << "-- depth w : " << depthStream.width() << std::endl;
    str << "-- depth h : " << depthStream.height() << std::endl;
    str << "-- depth format : " << depthStream.format() << std::endl;
    str << "-- color w : " << colorStream.width() << std::endl;
    str << "-- color h : " << colorStream.height() << std::endl;
    str << "-- color format : " << colorStream.format() << std::endl;

    SIGHT_DEBUG("Actual mode : \n" + str.str());

    const auto depthStreamW = static_cast<data::mesh::size_t>(depthStream.width());
    const auto depthStreamH = static_cast<data::mesh::size_t>(depthStream.height());
    const auto colorStreamW = static_cast<data::mesh::size_t>(colorStream.width());
    const auto colorStreamH = static_cast<data::mesh::size_t>(colorStream.height());

    {
        const auto colorTimeline = m_frame.lock();
        colorTimeline->init_pool_size(
            colorStreamW,
            colorStreamH,
            core::type::UINT8,
            data::frame_tl::PixelFormat::RGBA
        );
        colorTimeline->setMaximumSize(50);
    }

    {
        const auto depthTimeline = m_depth.lock();
        if(depthTimeline)
        {
            depthTimeline->init_pool_size(
                depthStreamW,
                depthStreamH,
                core::type::UINT16,
                data::frame_tl::PixelFormat::GRAY_SCALE
            );
            depthTimeline->setMaximumSize(50);
        }
    }

    {
// Get camera information.
        const auto camera_set = m_camera_set.lock();

        if(camera_set)
        {
            data::camera::sptr colorCamera;
            data::camera::sptr depthCamera;

            // check if there is camera
            if(camera_set->size() == 0)
            {
                depthCamera = std::make_shared<data::camera>();
                colorCamera = std::make_shared<data::camera>();

                camera_set->add_camera(depthCamera);
                camera_set->add_camera(colorCamera);
                auto sig = camera_set->signal<data::camera_set::added_camera_signal_t>(
                    data::camera_set::ADDED_CAMERA_SIG
                );
                sig->async_emit(depthCamera);
                sig->async_emit(colorCamera);
            }
            else if(camera_set->size() == 1) // missing one camera
            {
                depthCamera = camera_set->get_camera(0);
                colorCamera = std::make_shared<data::camera>();
                camera_set->add_camera(colorCamera);

                auto sig = camera_set->signal<data::camera_set::added_camera_signal_t>(
                    data::camera_set::ADDED_CAMERA_SIG
                );
                sig->async_emit(colorCamera);
            }
            else
            {
                depthCamera = camera_set->get_camera(0);
                colorCamera = camera_set->get_camera(1);
            }

            if(!depthCamera->getIsCalibrated() || !colorCamera->getIsCalibrated())
            {
                // copy device calibration into the camera series
                data::matrix4::sptr matrix = std::make_shared<data::matrix4>();

                const rs2_intrinsics depthIntrinsics = depthStream.get_intrinsics();
                const rs2_intrinsics colorIntrinsics = colorStream.get_intrinsics();
                const rs2_extrinsics extrinsic       = depthStream.get_extrinsics_to(colorStream);

                // Construct an explicit camera name: Intel RealSense D415(839112062452)
                const std::string cameraName = std::string(m_currentDevice.get_info(RS2_CAMERA_INFO_NAME)) + "("
                                               + std::string(m_currentDevice.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER))
                                               + ")";

                depthCamera->setDescription(cameraName + " Depth");
                depthCamera->setWidth(depthStreamW);
                depthCamera->setHeight(depthStreamH);
                depthCamera->setFx(static_cast<double>(depthIntrinsics.fx));
                depthCamera->setFy(static_cast<double>(depthIntrinsics.fy));
                depthCamera->setCx(static_cast<double>(depthIntrinsics.ppx));
                depthCamera->setCy(static_cast<double>(depthIntrinsics.ppy));

                depthCamera->setDistortionCoefficient(
                    static_cast<double>(depthIntrinsics.coeffs[0]),
                    static_cast<double>(depthIntrinsics.coeffs[1]),
                    static_cast<double>(depthIntrinsics.coeffs[2]),
                    static_cast<double>(depthIntrinsics.coeffs[3]),
                    static_cast<double>(depthIntrinsics.coeffs[4])
                );
                depthCamera->setIsCalibrated(true);

                colorCamera->setDescription(cameraName + "Color");
                colorCamera->setWidth(colorStreamW);
                colorCamera->setHeight(colorStreamH);
                colorCamera->setFx(static_cast<double>(colorIntrinsics.fx));
                colorCamera->setFy(static_cast<double>(colorIntrinsics.fy));
                colorCamera->setCx(static_cast<double>(colorIntrinsics.ppx));
                colorCamera->setCy(static_cast<double>(colorIntrinsics.ppy));
                colorCamera->setDistortionCoefficient(
                    static_cast<double>(colorIntrinsics.coeffs[0]),
                    static_cast<double>(colorIntrinsics.coeffs[1]),
                    static_cast<double>(colorIntrinsics.coeffs[2]),
                    static_cast<double>(colorIntrinsics.coeffs[3]),
                    static_cast<double>(colorIntrinsics.coeffs[4])
                );
                colorCamera->setIsCalibrated(true);

                std::size_t index = 0;
                for(std::size_t i = 0 ; i < 3 ; ++i)
                {
                    (*matrix)(i, 3) = static_cast<double>(extrinsic.translation[i] * s_METERS_TO_MMS);
                    for(std::size_t j = 0 ; j < 3 ; ++j)
                    {
                        (*matrix)(i, j) = static_cast<double>(extrinsic.rotation[index]);
                        ++index;
                    }
                }

                camera_set->set_extrinsic_matrix(1, matrix);

                auto sig = camera_set->signal<data::camera_set::modified_signal_t>(
                    data::camera_set::MODIFIED_SIG
                );
                sig->async_emit();
            }
        }
    }

    //Only create the pointer one time.

    auto pointcloud     = std::make_shared<data::mesh>();
    const auto dumpLock = pointcloud->dump_lock();

    const data::mesh::size_t nbPoints = depthStreamW * depthStreamH;

    // Allocate mesh.
    pointcloud->resize(nbPoints, nbPoints, data::mesh::cell_type_t::POINT, data::mesh::attribute::point_colors);

    // to display the mesh, we need to create cells with one point.
    data::mesh::cell_t i = 0;
    for(auto& cell : pointcloud->range<data::iterator::cell::point>())
    {
        cell.pt = i++;
    }

    pointcloud->truncate(nbPoints, nbPoints);

    m_pointCloudOutput = pointcloud;
}

//-----------------------------------------------------------------------------

void scan::startCamera()
{
    if(m_running)
    {
        SIGHT_WARN("Camera is still running. Nothing is done.");
        return;
    }

    {
        const auto camera_set = m_camera_set.lock();

        const auto setPlaybackMode =
            [&](data::camera::csptr camera)
            {
                if(camera->getCameraSource() == data::camera::file)
                {
                    m_playbackMode     = true;
                    m_playbackFileName = camera->getVideoFile().string();
                    this->signal<file_played_signal_t>(FILE_PLAYED_SIG)->async_emit();
                }
                else if(camera->getCameraSource() == data::camera::STREAM)
                {
                    sight::module::io::realsense::scan::popMessageDialog(
                        "RealSense grabber cannot open STREAM type, please select DEVICE or FILE."
                    );
                    return;
                }
                // CameraSource is unknow or DEVICE we open a device. This allows to work
                // without camera selector.
                else
                {
                    m_playbackMode = false;

                    // Test if previous device was kept (changing presets cases).
                    if(m_deviceID.empty())
                    {
                        m_deviceID = this->selectDevice();
                    }

                    this->signal<device_played_signal_t>(DEVICE_PLAYED_SIG)->async_emit();
                }
            };

        if(camera_set)
        {
            // Extract the first camera (source should be the same).
            data::mt::locked_ptr<data::camera> locked(camera_set->get_camera(0));
            setPlaybackMode(locked.get_shared());
        }
        else // No camera_set (called by grabberProxy for ex.).
        {
            const auto locked = m_camera.lock();
            setPlaybackMode(locked.get_shared());

            SIGHT_ASSERT(
                "Camera should not be null, check if  '"
                << s_CAMERA_SET_INOUT
                << "' or '"
                << s_CAMERA_INPUT
                << "' is present.",
                locked
            );
        }
    }

    rs2::config cfg;

    m_pipe = std::make_unique<rs2::pipeline>();

    rs2::pipeline_profile profile;
    try
    {
        if(m_deviceID.empty() && !m_playbackMode)
        {
            throw std::runtime_error("No RealSense device was found.");
        }

        // Enable selected device if playback is disabled.
        if(!m_playbackMode)
        {
            cfg.enable_device(m_deviceID);
        }

        // Enable depth stream (16 bit values)
        cfg.enable_stream(
            RS2_STREAM_DEPTH,
            m_cameraSettings.depthW,
            m_cameraSettings.depthH,
            RS2_FORMAT_Z16,
            m_cameraSettings.fps
        );

        // Enable color stream (RGBA8 is choosen to avoid conversion to sight).
        cfg.enable_stream(
            RS2_STREAM_COLOR,
            m_cameraSettings.colorW,
            m_cameraSettings.colorH,
            RS2_FORMAT_RGBA8,
            m_cameraSettings.fps
        );

        // Enable Infrared with the same parameters as the color one.
        cfg.enable_stream(
            RS2_STREAM_INFRARED,
            m_cameraSettings.colorW,
            m_cameraSettings.colorH,
            RS2_FORMAT_RGBA8,
            m_cameraSettings.fps
        );

        // Enable recording if needed.
        if(m_record)
        {
            cfg.enable_record_to_file(m_recordingFileName);
        }

        // Enable playback if needed.
        if(m_playbackMode)
        {
            cfg.enable_device_from_file(m_playbackFileName);
        }

        profile         = m_pipe->start(cfg);
        m_currentDevice = profile.get_device();

        // Test if device support advanced mode.
        if(m_currentDevice.is< ::rs400::advanced_mode>())
        {
            auto advanced_mode_dev = m_currentDevice.as< ::rs400::advanced_mode>();

            if(!advanced_mode_dev.is_enabled())
            {
                // Enable advanced-mode.
                advanced_mode_dev.toggle_advanced_mode(true);
                SIGHT_DEBUG("Enable advanced mode on realsense device.");
            }
        }
        else
        {
            // It's Ok if device is in playback mode.
            if(!m_playbackMode)
            {
                throw std::runtime_error("The selected device doesn't support advanced mode. This is required!");
            }
        }

        // Set a preset if there is one (can overwrite resolutions values).
        if(!m_cameraSettings.presetPath.empty())
        {
            // load json content
            std::ifstream json(m_cameraSettings.presetPath.string(), std::ifstream::in);
            std::string json_content((std::istreambuf_iterator<char>(json)),
                                     std::istreambuf_iterator<char>());

            // Camera needs to be in "advanced mode"
            auto advanced_mode_dev = m_currentDevice.as< ::rs400::advanced_mode>();
            advanced_mode_dev.load_json(json_content);
        }

        auto depthSensor = m_currentDevice.first<rs2::depth_sensor>();

        // Get the depth scale: depth in mm corresponding to a depth value of 1.
        m_depthScale = depthSensor.get_depth_scale() * s_METERS_TO_MMS;

        // Options are read-only when playing from files.
        if(!m_playbackMode)
        {
            depthSensor.set_option(RS2_OPTION_EMITTER_ENABLED, (m_cameraSettings.irEmitter ? 1.F : 0.F));
        }
    }
    catch(const std::exception& e)
    {
        sight::module::io::realsense::scan::popMessageDialog(
            "RealSense device is not available. Please check if it is plugged in. Error : "
            + std::string(e.what())
        );
        return;
    }

    // Initialize calibration/pointcloud.
    this->initialize(profile);

    // Launch the grabbing thread.
    m_running = true;
    m_thread  = std::thread(&scan::grab, this);

    // Set min/max range if they parameters has changed (slot called before startCamera).
    // This works only when grabbing thread is running.
    if(m_cameraSettings.minRange > s_MIN_DEPTH_RANGE || m_cameraSettings.maxRange < s_MAX_DEPTH_RANGE)
    {
        this->setMinMaxRange();
    }

    auto sigStarted = this->signal<grabber::camera_started_signal_t>(
        grabber::CAMERA_STARTED_SIG
    );
    sigStarted->async_emit();
}

//-----------------------------------------------------------------------------

void scan::stopCamera()
{
    // Grabbing thread is running
    if(m_running.exchange(false))
    {
        if(m_pause)
        {
            std::lock_guard<std::mutex> lock(m_pauseMutex);
            m_pause = false;
        }

        m_pauseConditionVariable.notify_all();

        m_thread.join();

        // If a preset was loaded we should hard-reset the camera,
        // If we don't some parameters are stored on the Camera ROM.
        if(m_cameraSettings.needHardReset)
        {
            // Reset the device (if preset was loaded, ...)
            m_currentDevice.hardware_reset();

            // Wait until hardware_reset as been sent to the camera.
            std::this_thread::sleep_for(std::chrono::seconds(1));
            m_deviceID.clear();
        }

        m_pipe->stop();
        m_pipe.reset();

        auto sig = this->signal<grabber::camera_stopped_signal_t>(grabber::CAMERA_STOPPED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void scan::pauseCamera()
{
    if(m_running)
    {
        // Enable/disable pause mode.
        {
            std::lock_guard<std::mutex> lock(m_pauseMutex);
            m_pause = !m_pause;
        }

        m_pauseConditionVariable.notify_all();

        // Also pause the recording if needed.
        if(m_record)
        {
            if(m_pause)
            {
                m_currentDevice.as<rs2::recorder>().pause();
            }
            else
            {
                m_currentDevice.as<rs2::recorder>().resume();
            }
        }
    }
}

//-----------------------------------------------------------------------------

void scan::record()
{
    // Cannot record when playback a file.
    if(m_playbackMode)
    {
        sight::module::io::realsense::scan::popMessageDialog("Cannot record when grabber playback a file !");
        return;
    }

    // If already recording, stop it.
    if(m_record)
    {
        m_record = false;
        if(m_running)
        {
            // Restart camera to stop recording pipeline.
            this->stopCamera();
            this->startCamera();
        }

        return;
    }

    // Check recording file first.
    bool erase = true;

    // If file already exists, should we erase it ?
    if(std::filesystem::exists(m_recordingFileName))
    {
        sight::ui::dialog::message warnDial;
        warnDial.setIcon(ui::dialog::message::warning);
        warnDial.setTitle("File already exists");
        warnDial.setMessage(
            "File: " + m_recordingFileName
            + " already exists, are you sure you want to erase it ?"
        );
        warnDial.addButton(ui::dialog::message::Buttons::YES);
        warnDial.addButton(ui::dialog::message::Buttons::NO);
        warnDial.setDefaultButton(ui::dialog::message::Buttons::NO);

        const auto res = warnDial.show();

        if(res == sight::ui::dialog::message::Buttons::NO)
        {
            erase = false;
        }
    }

    // Ask user for a new file if filename is empty OR if filename exists but user don't want to erase it.
    if(m_recordingFileName.empty() || (!erase && !m_recordingFileName.empty()))
    {
        // Configure recording path.
        this->configureRecordingPath();
    }

    // If filename is still empty at this point = user cancel the location dialog or location is not valid, so skip
    // recording.
    if(m_recordingFileName.empty())
    {
        return;
    }

    // Everything is ok at this point, we can start recording.
    m_record = true;

    // If grabbing thread is running.
    if(m_running)
    {
        // restart camera to enable recording pipeline.
        this->stopCamera();
        this->startCamera();
    }
}

//-----------------------------------------------------------------------------

void scan::configureRecordingPath()
{
    // Ask user for a new file name.
    sight::ui::dialog::location dial;
    dial.setTitle("Name of recording file");
    dial.setType(ui::dialog::location::single_file);
    dial.setOption(ui::dialog::location::WRITE);

    dial.addFilter("Bag files", "*.bag");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dial.show());

    // If filename is ok.
    if(result)
    {
        m_recordingFileName = result->get_file().string();
    }
}

//------------------------------------------------------------------------------

void scan::set_parameter(ui::parameter_t _value, std::string _key)
{
    try
    {
        // BOOL
        // IR key
        if(_key == s_IREMITTER)
        {
            // Save the value in cameraSettings.
            m_cameraSettings.irEmitter = std::get<bool>(_value);

            // Change the parameter live if grabber is running, otherwise it will be changed on next startCamera.
            if(!m_deviceID.empty() && m_running)
            {
                auto depthSensor = m_currentDevice.first<rs2::depth_sensor>();
                depthSensor.set_option(RS2_OPTION_EMITTER_ENABLED, (std::get<bool>(_value) ? 1.F : 0.F));
            }
        }
        else if(_key == s_SWITCH_TO_IR)
        {
            m_switchInfra2Color = std::get<bool>(_value);
        }
        else if(_key == s_ENABLE_SPACIAL_FILTER)
        {
            m_filterSettings.enableSpacial = std::get<bool>(_value);
        }
        else if(_key == s_ENABLE_TEMPORAL_FILTER)
        {
            m_filterSettings.enableTemporal = std::get<bool>(_value);
        }
        else if(_key == s_ENABLE_HOLES_FILLING_FILTER)
        {
            m_filterSettings.enableHolesFilling = std::get<bool>(_value);
        }
        else if(_key == "minRange")
        {
            if(std::get<int>(_value) < s_MIN_DEPTH_RANGE)
            {
                throw std::runtime_error("cannot set value < 0");
            }

            m_cameraSettings.minRange      = std::get<int>(_value);
            m_cameraSettings.needHardReset = true;
        }
        else if(_key == "maxRange")
        {
            if(std::get<int>(_value) > s_MAX_DEPTH_RANGE)
            {
                throw std::runtime_error("cannot set value > 65535");
            }

            m_cameraSettings.maxRange      = std::get<int>(_value);
            m_cameraSettings.needHardReset = true;
        }
        else if(_key == s_SPACIAL_MAGNITUDE)
        {
            if(std::get<int>(_value) < 1 || std::get<int>(_value) > 5)
            {
                throw std::runtime_error(s_SPACIAL_MAGNITUDE + " value must be in [1-5].");
            }

            m_filterSettings.spacialMagnitude = static_cast<std::uint8_t>(std::get<int>(_value));
        }
        else if(_key == s_SPACIAL_SMOOTH_DELTA)
        {
            if(std::get<int>(_value) < 1 || std::get<int>(_value) > 50)
            {
                throw std::runtime_error(s_SPACIAL_SMOOTH_DELTA + " value must be in [1-50].");
            }

            m_filterSettings.spacialSmoothDelta = static_cast<std::uint8_t>(std::get<int>(_value));
        }
        else if(_key == s_SPACIAL_HOLE_FILLING)
        {
            // Skip if we have the value as a string, only use the index.
            if(std::holds_alternative<std::string>(_value))
            {
                return;
            }

            if(std::get<int>(_value) < 0 || std::get<int>(_value) > 5)
            {
                throw std::runtime_error(s_SPACIAL_HOLE_FILLING + " value must be in [0-5].");
            }

            m_filterSettings.spacialHoleFilling = static_cast<std::uint8_t>(std::get<int>(_value));
        }
        else if(_key == s_TEMPORAL_SMOOTH_DELTA)
        {
            if(std::get<int>(_value) < 1 || std::get<int>(_value) > 100)
            {
                throw std::runtime_error(s_TEMPORAL_SMOOTH_DELTA + " value must be in [1-100].");
            }

            m_filterSettings.temporalSmoothDelta = static_cast<std::uint8_t>(std::get<int>(_value));
        }
        else if(_key == s_TEMPORAL_PERSISTENCY)
        {
            // Skip if we have the value as a string, only use the index.
            if(std::holds_alternative<std::string>(_value))
            {
                return;
            }

            if(std::get<int>(_value) < 0 || std::get<int>(_value) > 8)
            {
                throw std::runtime_error(s_TEMPORAL_PERSISTENCY + " value must be in [0-8].");
            }

            m_filterSettings.temporalPersistency = static_cast<std::uint8_t>(std::get<int>(_value));
        }
        else if(_key == s_HOLE_FILLING)
        {
            // Skip if we have the value as a string, only use the index.
            if(std::holds_alternative<std::string>(_value))
            {
                return;
            }

            if(std::get<int>(_value) < 0 || std::get<int>(_value) > 2)
            {
                throw std::runtime_error(s_HOLE_FILLING + " value must be in [0-2].");
            }

            m_filterSettings.holeFilling = static_cast<std::uint8_t>(std::get<int>(_value));
        }
        // ENUM
        // Change preset (advanced option).
        else if(_key == s_PRESET)
        {
            // Skip value is stored as index.
            if(std::holds_alternative<int>(_value))
            {
                return;
            }

            const auto presetPathToLoad = m_jsonPresets[std::get<std::string>(_value)];

            if(!presetPathToLoad.empty())
            {
                m_cameraSettings.presetPath = presetPathToLoad;

                if(!m_deviceID.empty() && m_running)
                {
                    // We need to restart the same camera.
                    // Make sure no hard-reset is performed in this particular case.
                    m_cameraSettings.needHardReset = false;
                    this->stopCamera();
                    this->startCamera();
                }

                // Ok now we should hard-reset if stopCamera() is called.
                m_cameraSettings.needHardReset = true;
            }
            else
            {
                SIGHT_ERROR("Cannot load preset named: " + std::get<std::string>(_value) + ". Nothing append");
            }
        }
        else if(_key == s_ALIGNMENT)
        {
            // Skip value is stored as index.
            if(std::holds_alternative<int>(_value))
            {
                return;
            }

            if(this->updateAlignment(std::get<std::string>(_value)))
            {
                this->stopCamera();
                this->startCamera();
            }
        }
        // DOUBLE
        else if(_key == s_SPACIAL_SMOOTH_ALPHA)
        {
            if(std::get<double>(_value) < 0.25 || std::get<double>(_value) > 1)
            {
                throw std::runtime_error(s_SPACIAL_SMOOTH_ALPHA + " must be in [0.25-1]");
            }

            m_filterSettings.spacialSmoothAlpha = static_cast<float>(std::get<double>(_value));
        }
        else if(_key == s_TEMPORAL_SMOOTH_ALPHA)
        {
            if(std::get<double>(_value) < 0 || std::get<double>(_value) > 1)
            {
                throw std::runtime_error(s_TEMPORAL_SMOOTH_ALPHA + " must be in [0-1]");
            }

            m_filterSettings.temporalSmoothAlpha = static_cast<float>(std::get<double>(_value));
        }
        else
        {
            SIGHT_ERROR("Key '" + _key + "' is not recognized.");
        }

        // Change parameters live if grabber is running, otherwise it will be changed on next call to startCamera.
        if(!m_deviceID.empty() && m_running)
        {
            this->setMinMaxRange();
        }
    }
    catch(const std::exception& e)
    {
        sight::module::io::realsense::scan::popMessageDialog("RealSense device error:" + std::string(e.what()));
        return;
    }
}

//-----------------------------------------------------------------------------

void scan::popMessageDialog(const std::string& _message)
{
    sight::ui::dialog::message::show(
        "RealSense Error",
        _message,
        sight::ui::dialog::message::CRITICAL
    );
}

//-----------------------------------------------------------------------------

void scan::grab()
{
    // Declare pointcloud object, for calculating pointclouds and texture mappings
    rs2::pointcloud pc;
    rs2::spatial_filter spatialFilter;                // Spatial    - edge-preserving spatial smoothing
    rs2::temporal_filter temporalFilter;              // Temporal   - reduces temporal noise
    rs2::hole_filling_filter holesFilter;             // Holes filling
    rs2::disparity_transform depthToDisparity(true);  // transform depth to disparity
    rs2::disparity_transform disparityToDepth(false); // transform disparity to depth

    bool needAlignment = false;

    rs2::align alignFrames(RS2_STREAM_COLOR);

    // Since RS2_STREAM_* is an enum, and DEPTH, COLOR & INFRARED are stored in this order, we can eliminate other
    // value, and default one.
    if(m_cameraSettings.streamToAlignTo != RS2_STREAM_ANY && m_cameraSettings.streamToAlignTo < RS2_STREAM_FISHEYE)
    {
        alignFrames   = rs2::align(m_cameraSettings.streamToAlignTo);
        needAlignment = true;
    }

    // We want the points object to be persistent so we can display the last cloud when a frame drops
    rs2::points points;

    while(m_running)
    {
        while(m_pause && m_running)
        {
            std::unique_lock<std::mutex> lock(m_pauseMutex);
            m_pauseConditionVariable.wait(lock);
        }

        try
        {
            // Wait for the next set of frames from the camera
            auto frames = m_pipe->wait_for_frames();

            // Align each frames in the chosen coordinate frames.
            if(needAlignment)
            {
                frames = alignFrames.process(frames);
            }

            auto depth = frames.get_depth_frame();
            auto color = frames.get_color_frame();
            auto infra = frames.get_infrared_frame();
            rs2::frame mapframe;
            rs2::frame colorOrInfra;

            // push infrared in color TL if needed.
            m_switchInfra2Color ? colorOrInfra = infra : colorOrInfra = color;

            // Generate the pointcloud and texture mappings
            if(depth)
            {
                // transform depth to disparity to apply spacial and temporal filter
                if(m_filterSettings.enableSpacial || m_filterSettings.enableTemporal)
                {
                    depth = depthToDisparity.process(depth);
                }

                if(m_filterSettings.enableSpacial)
                {
                    spatialFilter.set_option(RS2_OPTION_FILTER_MAGNITUDE, m_filterSettings.spacialMagnitude);
                    spatialFilter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, m_filterSettings.spacialSmoothAlpha);
                    spatialFilter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, m_filterSettings.spacialSmoothDelta);
                    spatialFilter.set_option(RS2_OPTION_HOLES_FILL, m_filterSettings.spacialHoleFilling);
                    depth = spatialFilter.process(depth);
                }

                if(m_filterSettings.enableTemporal)
                {
                    temporalFilter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, m_filterSettings.temporalSmoothAlpha);
                    temporalFilter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, m_filterSettings.temporalSmoothDelta);
                    temporalFilter.set_option(RS2_OPTION_HOLES_FILL, m_filterSettings.temporalPersistency);
                    depth = temporalFilter.process(depth);
                }

                // transform disparity to depth
                if(m_filterSettings.enableSpacial || m_filterSettings.enableTemporal)
                {
                    depth = disparityToDepth.process(depth);
                }

                if(m_filterSettings.enableHolesFilling)
                {
                    holesFilter.set_option(RS2_OPTION_HOLES_FILL, m_filterSettings.holeFilling);
                    depth = holesFilter.process(depth);
                }

                points = pc.calculate(depth);
                if(m_pointcloudColorMap == PointcloudColormap::COLOR)
                {
                    mapframe = color;
                    pc.map_to(color);
                }
                else if(m_pointcloudColorMap == PointcloudColormap::DEPTH)
                {
                    mapframe = depth;
                    pc.map_to(depth);
                }
                else if(m_pointcloudColorMap == PointcloudColormap::INFRARED)
                {
                    mapframe = infra;
                    pc.map_to(infra);
                }

                this->onPointCloud(points, mapframe);
            }

            if(m_depth.lock().get_shared())
            {
                this->onCameraImageDepth(reinterpret_cast<const std::uint16_t*>(depth.get_data()));
            }

            this->onCameraImage(reinterpret_cast<const std::uint8_t*>(colorOrInfra.get_data()));

            // Compute the z value of the center pixel, to give the distance "object-camera" in mm.
            const auto distanceToCenter = depth.get_distance(depth.get_width() / 2, depth.get_height() / 2);
            this->signal<distance_computed_t>(DISTANCE_COMPUTED_SIG)->async_emit(
                static_cast<double>(distanceToCenter * s_METERS_TO_MMS)
            );
        }
        catch(const std::exception& e)
        {
            this->popMessageDialog("RealSense device error:" + std::string(e.what()));
            return;
        }
    }
}

//-----------------------------------------------------------------------------

void scan::loadPresets(const ::fs::path& _path)
{
    // 1. List all .json files in the folder.
    ::fs::directory_iterator end_itr;

    // 1.1. Cycle through the directory
    for(::fs::directory_iterator itr(_path) ; itr != end_itr ; ++itr)
    {
        // 1.2. Check only files with .json extension.
        if(::fs::is_regular_file(itr->path()) && itr->path().extension() == ".json")
        {
            // 2. Generate "readable name" by removing "Preset*.json".
            const std::string current_file = itr->path().filename().string();
            const auto p                   = current_file.find("Preset");
            const std::string name         = current_file.substr(0, p);

            // 3. Push in Map [Name, path].
            m_jsonPresets[name] = itr->path();
        }
    }
}

//-----------------------------------------------------------------------------

bool scan::updateAlignment(const std::string& _alignTo)
{
    if(_alignTo == "None")
    {
        m_cameraSettings.streamToAlignTo = RS2_STREAM_COUNT; // Equivalent to No alignment.
    }
    else if(_alignTo == "Color")
    {
        m_cameraSettings.streamToAlignTo = RS2_STREAM_COLOR;
    }
    else if(_alignTo == "Depth")
    {
        m_cameraSettings.streamToAlignTo = RS2_STREAM_DEPTH;
    }
    else if(_alignTo == "Infrared")
    {
        m_cameraSettings.streamToAlignTo = RS2_STREAM_INFRARED;
    }
    else
    {
        SIGHT_ERROR("'" + _alignTo + "' is not a valid alignment option (None, Color, Depth or Infrared).");
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

void scan::setMinMaxRange()
{
    if(!m_playbackMode)
    {
        try
        {
            // Use the "advanced_mode" to set DepthClampMax value.
            auto advanced_mode_dev = m_currentDevice.as<rs400::advanced_mode>();
            auto depth_table       = advanced_mode_dev.get_depth_table();
            depth_table.depthClampMin = m_cameraSettings.minRange;
            depth_table.depthClampMax = m_cameraSettings.maxRange;
            advanced_mode_dev.set_depth_table(depth_table);
        }
        catch(const std::exception& e)
        {
            this->popMessageDialog("RealSense device error:" + std::string(e.what()));
            return;
        }
    }
}

//-----------------------------------------------------------------------------

void scan::onCameraImage(const uint8_t* _buffer)
{
    // Filling timeline's buffer
    const fwClock::type timestamp(fwClock::get_time_in_milli_sec());
    data::timeline::signals::pushed_t::sptr sig;

    {
        const auto colorTimeline = m_frame.lock();
        const auto colorBuffer   = colorTimeline->create_buffer(timestamp);

        auto* destColorBuffer = reinterpret_cast<uint8_t*>(colorBuffer->addElement(0));

        memcpy(destColorBuffer, _buffer, colorBuffer->size());

        // Push buffer to timeline and notify
        colorTimeline->pushObject(colorBuffer);

        sig = colorTimeline->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
    }

    sig->async_emit(timestamp);
}

//-----------------------------------------------------------------------------

void scan::onCameraImageDepth(const std::uint16_t* _buffer)
{
    // Filling the depth image buffer in the timeline
    const fwClock::type timestamp(fwClock::get_time_in_milli_sec());
    data::timeline::signals::pushed_t::sptr sig;

    {
        const auto depthTimeline = m_depth.lock();
        const auto depthTL       = depthTimeline->create_buffer(timestamp);

        const auto width  = depthTimeline->getWidth();
        const auto height = depthTimeline->getHeight();

        auto* depthBuffer     = reinterpret_cast<std::uint16_t*>(depthTL->addElement(0));
        const auto sizeBuffer = width * height;

        // Re-map depth frame in mm.
        for(std::size_t i = 0 ; i < sizeBuffer ; ++i)
        {
            *depthBuffer++ = static_cast<std::uint16_t>(static_cast<float>(*_buffer++) * m_depthScale);
        }

        // Push buffer to timeline and notify
        depthTimeline->pushObject(depthTL);

        sig = depthTimeline->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
    }

    sig->async_emit(timestamp);
}

//-----------------------------------------------------------------------------

void scan::onPointCloud(const rs2::points& _pc, const rs2::video_frame& _texture)
{
    auto pointcloud = m_pointCloudOutput.lock();

    if(pointcloud)
    {
        // Get Width and Height coordinates of texture
        const int textureW = _texture.get_width();  // Frame width in pixels
        const int textureH = _texture.get_height(); // Frame height in pixels

        const int textureBytePerPix   = _texture.get_bytes_per_pixel();
        const int textureStrides      = _texture.get_stride_in_bytes();
        const auto* const textureBuff = reinterpret_cast<const uint8_t*>(_texture.get_data());

        /* this segment actually prints the pointcloud */
        const auto* const vertices     = _pc.get_vertices();
        const auto* const textureCoord = _pc.get_texture_coordinates();

        const std::size_t pcSize = _pc.size();

        // Parallelization of the loop is possible since each element is independent.

        auto points = pointcloud->begin<data::iterator::point::xyz>();
        auto colors = pointcloud->begin<data::iterator::point::rgba>();

        for(std::int64_t i = 0 ; i < static_cast<std::int64_t>(pcSize) ; ++i)
        {
            // Fill the point buffer (x = +0, y = +1, z = +2).
            points->x = static_cast<float>(vertices[i].x) * s_METERS_TO_MMS;
            points->y = static_cast<float>(vertices[i].y) * s_METERS_TO_MMS;
            points->z = static_cast<float>(vertices[i].z) * s_METERS_TO_MMS * m_depthScale; // Re-map to mm.

            // Normals to texture Coordinates conversion
            const int x_value = std::min(
                std::max(
                    static_cast<int>(std::lround(textureCoord[i].u * static_cast<float>(textureW))),
                    0
                ),
                textureW - 1
            );
            const int y_value = std::min(
                std::max(
                    static_cast<int>(std::lround(textureCoord[i].v * static_cast<float>(textureH))),
                    0
                ),
                textureH - 1
            );

            const int bytes   = x_value * textureBytePerPix; // Get # of bytes per pixel
            const int strides = y_value * textureStrides;    // Get line width in bytes
            const int index   = (bytes + strides);

            // Fill the color buffer (R = +0, G = +1, B = +2).
            colors->r = textureBuff[index + 0];
            colors->g = textureBuff[index + 1];
            colors->b = textureBuff[index + 2];
            colors->a = 255;

            ++points;
            ++colors;
        }

        const auto sigVertex = pointcloud->signal<data::mesh::signal_t>(data::mesh::VERTEX_MODIFIED_SIG);
        sigVertex->async_emit();

        const auto sigcolor = pointcloud->signal<data::mesh::signal_t>(data::mesh::POINT_COLORS_MODIFIED_SIG);
        sigcolor->async_emit();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::realsense
