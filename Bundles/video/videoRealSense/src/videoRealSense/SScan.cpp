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

#include "videoRealSense/SScan.hpp"

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Type.hpp>

#include <boost/make_unique.hpp>

#include <librealsense2/rs.hpp>

namespace videoRealSense
{

const ::fwServices::IService::KeyType s_CAMERA_SERIES_INOUT = "cameraSeries";

// The number of meters represented by a single depth unit
float get_depth_unit_value(const ::rs2::device& dev)
{
    auto sensorList = dev.query_sensors();

    float depth_scale = 0;
    for (auto& sensor : sensorList)
    {
        if (rs2::depth_sensor depthSensor = sensor.as<rs2::depth_sensor>())
        {
            depth_scale = depthSensor.get_depth_scale();
            break;
        }
    }

    return depth_scale;
}

//-----------------------------------------------------------------------------

SScan::SScan() noexcept
{
}

//-----------------------------------------------------------------------------

SScan::~SScan() noexcept
{
}

//-----------------------------------------------------------------------------

void SScan::starting()
{
    m_depthTimeline = this->getInOut< ::arData::FrameTL>(s_DEPTHTL_INOUT);
    m_colorTimeline = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);
}

//-----------------------------------------------------------------------------

void SScan::stopping()
{
    this->stopCamera();
}

//-----------------------------------------------------------------------------

void SScan::configuring()
{
}

//-----------------------------------------------------------------------------

void SScan::updating()
{
    this->stopCamera();
}

//-----------------------------------------------------------------------------

void SScan::onRgbaPointCloud(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr& _pc)
{
    //TODO: use the pointCloud
}

//-----------------------------------------------------------------------------

std::string SScan::selectDevice()
{
    // Obtain a list of devices currently present on the system
    ::rs2::context ctx;
    auto devices          = ctx.query_devices();
    uint32_t device_count = devices.size();
    std::string selectedDevice;

    // Only one device found -> select it
    if(device_count == 1)
    {
        selectedDevice = devices[0].get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
    }
    // Several devices found -> open a selector dialog
    else if(device_count > 1)
    {
        ::fwGui::dialog::SelectorDialog dial;
        dial.setTitle("Select realsense camera");

        std::vector< std::string > selections;
        selections.resize(devices.size());

        for(uint32_t i = 0; i < devices.size(); ++i)
        {
            const std::string name   = devices[i].get_info(RS2_CAMERA_INFO_NAME);
            const std::string serial = devices[i].get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

            // Display index + name + serial (ex: 1.Intel RealSense D415(839112062452) )
            const std::string nameserial = std::to_string(i + 1) + ". " + name + " (" + serial + ")";

            selections[i] = nameserial;
        }
        dial.setSelections(selections);

        std::string selected = dial.show();
        // Get the index of selected camera
        auto index = std::atoi(&selected.at(0)) - 1;
        // Get associated serial numbers
        selectedDevice = devices[static_cast<uint32_t>(index)].get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
    }

    return selectedDevice;
}

//-----------------------------------------------------------------------------

void SScan::onCameraImage(const uint8_t* _buffer)
{
    // Filling timeline's buffer
    const auto now = std::chrono::system_clock::now();
    const auto res = std::chrono::duration_cast< std::chrono::microseconds >(now.time_since_epoch()).count();
    const double timestamp( res );

    SPTR(::arData::FrameTL::BufferType) colorBuffer = m_colorTimeline->createBuffer(timestamp);

    uint8_t* destColorBuffer = reinterpret_cast< uint8_t* >( colorBuffer->addElement(0) );

    memcpy(destColorBuffer, _buffer, colorBuffer->getSize());

    // Push buffer to timeline and notify
    m_colorTimeline->pushObject(colorBuffer);
    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = m_colorTimeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
        ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

void SScan::onCameraImageDepth(const std::uint16_t* _buffer)
{
    // Filling the depth image buffer in the timeline
    const auto now = std::chrono::system_clock::now();
    const auto res = std::chrono::duration_cast< std::chrono::microseconds >(now.time_since_epoch()).count();
    const double timestamp( res );

    SPTR(::arData::FrameTL::BufferType) depthTL = m_depthTimeline->createBuffer(timestamp);

    const auto width  = m_depthTimeline->getWidth();
    const auto height = m_depthTimeline->getHeight();

    std::uint16_t* depthBuffer = reinterpret_cast< std::uint16_t* >( depthTL->addElement(0) );
    const auto sizeBuffer      = width * height;

    for (size_t idx = 0; idx < sizeBuffer; ++idx)
    {
        *depthBuffer++ = *_buffer++ *m_depthScale;
    }

    // Push buffer to timeline and notify
    m_depthTimeline->pushObject(depthTL);
    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = m_depthTimeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
        ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

void SScan::startCamera()
{
    // Get camera information
    ::arData::CameraSeries::sptr cameraSeries = this->getInOut< ::arData::CameraSeries>(s_CAMERA_SERIES_INOUT);

    auto dev = this->selectDevice();

    ::rs2::config cfg;

    m_pipe = ::boost::make_unique< ::rs2::pipeline>();

    ::rs2::pipeline_profile profile;
    try
    {
        if(dev.empty())
        {
            throw std::runtime_error("No RealSense device was found.");
        }

        // Enable selected device
        cfg.enable_device(dev);

        // Enable depth and color
        // TODO: let user change resolution and fps ?
        cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30);
        cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_RGBA8, 30);

        profile = m_pipe->start(cfg);
    }
    catch(const std::exception& e)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "RealSense Error",
            "RealSense device is not available. Please check if it is plugged in. Error : " + std::string(e.what()),
            ::fwGui::dialog::IMessageDialog::CRITICAL);
        return;
    }
    /* Determine depth value corresponding to one meter */
    static const float s_METERS_TO_MMS = 1000.f;

    m_depthScale = get_depth_unit_value(profile.get_device()) * s_METERS_TO_MMS;

    m_colorTimeline = this->getInOut< ::arData::FrameTL>(s_FRAMETL_INOUT);

    auto depthStream = profile.get_stream(RS2_STREAM_DEPTH).as< ::rs2::video_stream_profile>();
    auto colorStream = profile.get_stream(RS2_STREAM_COLOR).as< ::rs2::video_stream_profile>();

    std::stringstream str;
    str << "-- fps : " << depthStream.fps() << std::endl;
    str << "-- depth w : " << depthStream.width() << std::endl;
    str << "-- depth h : " << depthStream.height() << std::endl;
    str << "-- depth format : " << depthStream.format() << std::endl;
    str << "-- color w : " << colorStream.width() << std::endl;
    str << "-- color h : " << colorStream.height() << std::endl;
    str << "-- color format : " << colorStream.format() << std::endl;

    SLM_DEBUG("Actual mode : \n" + str.str());

    const size_t depthStreamW = static_cast<size_t>(depthStream.width());
    const size_t depthStreamH = static_cast<size_t>(depthStream.height());
    const size_t colorStreamW = static_cast<size_t>(colorStream.width());
    const size_t colorStreamH = static_cast<size_t>(colorStream.height());

    m_depthTimeline->initPoolSize(depthStreamW, depthStreamH, ::fwTools::Type::s_UINT16, 1);
    m_colorTimeline->initPoolSize(colorStreamW, colorStreamH, ::fwTools::Type::s_UINT8, 4);
    m_depthTimeline->setMaximumSize(50);
    m_colorTimeline->setMaximumSize(50);

    if(cameraSeries)
    {
        ::arData::Camera::sptr depthCamera = ::arData::Camera::New();
        ::arData::Camera::sptr colorCamera = ::arData::Camera::New();

        ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::New();
        cameraSeries->addCamera(depthCamera);
        cameraSeries->addCamera(colorCamera);

        rs2_intrinsics depthIntrinsics = depthStream.get_intrinsics();
        rs2_intrinsics colorIntrinsics = colorStream.get_intrinsics();
        rs2_extrinsics extrinsic       = depthStream.get_extrinsics_to(colorStream);

        depthCamera->setDescription("Depth camera");
        depthCamera->setWidth(depthStreamW);
        depthCamera->setHeight(depthStreamH);
        depthCamera->setFx(static_cast<double>(depthIntrinsics.fx));
        depthCamera->setFy(static_cast<double>(depthIntrinsics.fy));
        depthCamera->setCx(static_cast<double>(depthIntrinsics.ppx));
        depthCamera->setCy(static_cast<double>(depthIntrinsics.ppy));

        depthCamera->setDistortionCoefficient(static_cast<double>(depthIntrinsics.coeffs[0]),
                                              static_cast<double>(depthIntrinsics.coeffs[1]),
                                              static_cast<double>(depthIntrinsics.coeffs[2]),
                                              static_cast<double>(depthIntrinsics.coeffs[3]),
                                              static_cast<double>(depthIntrinsics.coeffs[4]));
        depthCamera->setIsCalibrated(true);

        colorCamera->setDescription("Color camera");
        colorCamera->setWidth(colorStreamW);
        colorCamera->setHeight(colorStreamH);
        colorCamera->setFx(static_cast<double>(colorIntrinsics.fx));
        colorCamera->setFy(static_cast<double>(colorIntrinsics.fy));
        colorCamera->setCx(static_cast<double>(colorIntrinsics.ppx));
        colorCamera->setCy(static_cast<double>(colorIntrinsics.ppy));
        colorCamera->setDistortionCoefficient(static_cast<double>(colorIntrinsics.coeffs[0]),
                                              static_cast<double>(colorIntrinsics.coeffs[1]),
                                              static_cast<double>(colorIntrinsics.coeffs[2]),
                                              static_cast<double>(colorIntrinsics.coeffs[3]),
                                              static_cast<double>(colorIntrinsics.coeffs[4]));
        colorCamera->setIsCalibrated(true);

        size_t index = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            matrix->setCoefficient(i, 3, static_cast<double>(extrinsic.translation[i] * s_METERS_TO_MMS));
            for (size_t j = 0; j < 3; ++j)
            {
                matrix->setCoefficient(i, j, static_cast<double>(extrinsic.rotation[index]));
                ++index;
            }
        }
        cameraSeries->setExtrinsicMatrix(1, matrix);

        auto sig = cameraSeries->signal< ::arData::CameraSeries::ModifiedSignalType >(
            ::arData::CameraSeries::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

    m_running = true;
    m_thread  = std::thread([this]
        {
            while(m_running)
            {
                // Wait for the next set of frames from the camera
                auto frames = m_pipe->wait_for_frames();

                auto depth = frames.get_depth_frame();

                // Generate the pointcloud and texture mappings

                // Declare pointcloud object, for calculating pointclouds and texture mappings
                rs2::pointcloud pc;
                auto points = pc.calculate(depth);

                auto color = frames.get_color_frame();

                this->onCameraImageDepth(reinterpret_cast<const std::uint16_t*>(depth.get_data()));
                this->onCameraImage(reinterpret_cast<const std::uint8_t*>(color.get_data()));
            }
        }
                            );

    auto sigStarted = this->signal< ::arServices::IGrabber::CameraStartedSignalType >(
        ::arServices::IGrabber::s_CAMERA_STARTED_SIG);
    sigStarted->asyncEmit();
}

//-----------------------------------------------------------------------------

void SScan::stopCamera()
{
    if(m_running)
    {
        m_running = false;

        m_thread.join();

        m_pipe->stop();
        m_pipe.reset();

        auto sig = this->signal< ::arServices::IGrabber::CameraStoppedSignalType >(
            ::arServices::IGrabber::s_CAMERA_STOPPED_SIG);
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SScan::pauseCamera()
{
    //TODO
}

//-----------------------------------------------------------------------------

} // namespace videoRealSense
