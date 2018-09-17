/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2015-, Open Perception, Inc.
 *  Copyright (c) 2016, Intel Corporation
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "realsenseTools/RsGrabber.hpp"

#include "realsenseTools/DeviceManager.hpp"

#include <fwCore/spyLog.hpp>

#include <boost/lexical_cast.hpp>

#include <pcl/common/io.h>

namespace realsenseTools
{

// ### Mode ##
//-----------------------------------------------------------------------------

RsGrabber::Mode::Mode () :
    fps(0),
    depth_width(0),
    depth_height(0),
    color_width(0),
    color_height(0)
{
}

//-----------------------------------------------------------------------------

RsGrabber::Mode::Mode (unsigned int f) :
    fps(f),
    depth_width(0),
    depth_height(0),
    color_width(0),
    color_height(0)
{
}

//-----------------------------------------------------------------------------

RsGrabber::Mode::Mode (unsigned int dw, unsigned int dh) :
    fps(0),
    depth_width(dw),
    depth_height(dh),
    color_width(0),
    color_height(0)
{
}

//-----------------------------------------------------------------------------

RsGrabber::Mode::Mode (unsigned int f, unsigned int dw, unsigned int dh) :
    fps(f),
    depth_width(dw),
    depth_height(dh),
    color_width(0),
    color_height(0)
{
}

//-----------------------------------------------------------------------------

RsGrabber::Mode::Mode (unsigned int dw, unsigned int dh, unsigned int cw, unsigned int ch) :
    fps(0),
    depth_width(dw),
    depth_height(dh),
    color_width(cw),
    color_height(ch)
{
}

//-----------------------------------------------------------------------------

RsGrabber::Mode::Mode (unsigned int f, unsigned int dw, unsigned int dh, unsigned int cw, unsigned int ch) :
    fps(f),
    depth_width(dw),
    depth_height(dh),
    color_width(cw),
    color_height(ch)
{
}

//-----------------------------------------------------------------------------

bool RsGrabber::Mode::operator== (const RsGrabber::Mode& m) const
{
    return (this->fps == m.fps &&
            this->depth_width == m.depth_width &&
            this->depth_height == m.depth_height &&
            this->color_width == m.color_width &&
            this->color_height == m.color_height);
}

//## RsGrabber ##
//-----------------------------------------------------------------------------

bool RsGrabber::isAvailable()
{
    bool available = true;
    try
    {
        DeviceManager::Ptr deviceM = DeviceManager::getInstance();
        size_t numDevice           = deviceM->getNumDevices();
        available = (numDevice > 0);
    }
    catch(std::runtime_error& e)
    {
        OSLM_ERROR("Catching exception : "<<e.what());
        available = false;
    }
    return available;
}

//-----------------------------------------------------------------------------

RsGrabber::RsGrabber (const std::string& device_id, const Mode& mode, bool strict) :
    ::pcl::Grabber(),
    m_isRunning(false),
    m_confidenceThreshold(6),
    m_temporalFilteringType(RealSense_None),
    m_temporalFilteringWindowSize(1),
    m_modeRequested(mode),
    m_strict(strict),
    m_maxDistance(6)
{
    if (device_id == "")
    {
        m_device = DeviceManager::getInstance()->captureDevice();
    }
    else if (device_id[0] == '#')
    {
        m_device =
            DeviceManager::getInstance()->captureDevice( ::boost::lexical_cast<size_t> (device_id.substr(1)) - 1);
    }
    else
    {
        m_device = DeviceManager::getInstance()->captureDevice(device_id);
    }

    m_pointCloudSignal     = createSignal<sig_cb_real_sense_point_cloud> ();
    m_pointCloudRgbaSignal = createSignal<sig_cb_real_sense_point_cloud_rgba> ();

    m_depthImageSignal = createSignal<sig_cb_real_sense_depth_image> ();
    m_rgbImageSignal   = createSignal<sig_cb_real_sense_rgb_image> ();
}

//-----------------------------------------------------------------------------

RsGrabber::~RsGrabber() noexcept
{
    stop();

    disconnect_all_slots<sig_cb_real_sense_point_cloud> ();
    disconnect_all_slots<sig_cb_real_sense_point_cloud_rgba> ();

    disconnect_all_slots<sig_cb_real_sense_depth_image> ();
    disconnect_all_slots<sig_cb_real_sense_rgb_image> ();
}

//-----------------------------------------------------------------------------

void RsGrabber::stop()
{
    if (m_isRunning)
    {
        m_isRunning = false;
        m_thread.join();
    }
}

//-----------------------------------------------------------------------------

void RsGrabber::start ()
{
    if (!m_isRunning)
    {
        m_needXYZ     = num_slots<sig_cb_real_sense_point_cloud> () > 0;
        m_needXYZRGBA = num_slots<sig_cb_real_sense_point_cloud_rgba> () > 0;

        m_needDepthImage = num_slots<sig_cb_real_sense_depth_image> () > 0;
        m_needRGBImage   = num_slots<sig_cb_real_sense_rgb_image> () > 0;

        if (m_needXYZ || m_needXYZRGBA || m_needDepthImage )
        {
            if (m_temporalFilteringType != RealSense_None)
            {
                m_temporalFilteringType = RealSense_None;

                SLM_WARN("This device does not support temporal filter with librealsense now");
            }
            this->selectMode();
            m_device->getDevice()->enable_stream(::rs::stream::depth, static_cast<int>(m_modeSelected.depth_width),
                                                 static_cast<int>(m_modeSelected.depth_height), ::rs::format::z16,
                                                 static_cast<int>(m_modeSelected.fps));
        }
        if (m_needXYZRGBA || m_needRGBImage)
        {
            m_device->getDevice()->enable_stream(::rs::stream::color, static_cast<int>(m_modeSelected.color_width),
                                                 static_cast<int>(m_modeSelected.color_height), ::rs::format::rgb8,
                                                 static_cast<int>(m_modeSelected.fps));
        }
        m_frequency.reset();
        m_isRunning = true;
        m_thread    = std::thread(&RsGrabber::run, this);
    }
}

//-----------------------------------------------------------------------------

const std::string& RsGrabber::getDeviceSerialNumber () const
{
    return (m_device->getSerialNumber());
}

//-----------------------------------------------------------------------------

void RsGrabber::setConfidenceThreshold (unsigned int threshold)
{
    if (threshold > 15)
    {
        SLM_WARN("Attempted to set threshold outside valid range (0-15)");
    }
    else if (!m_device->getDevice()->supports_option(::rs::option::f200_confidence_threshold))
    {
        SLM_WARN("This device does not support setting confidence threshold with librealsense now");
    }
    else
    {
        m_device->getDevice()->set_option(::rs::option::f200_confidence_threshold, threshold);
    }
}

//-----------------------------------------------------------------------------

std::vector< RsGrabber::Mode > RsGrabber::getAvailableModes (bool only_depth) const
{
    std::vector<Mode> modes;
    // Check if device is available
    if(!m_device)
    {
        SLM_ERROR("No Device available, please check previous error");
        return modes;
    }
    int depth_total = m_device->getDevice()->get_stream_mode_count(::rs::stream::depth);
    int color_total = m_device->getDevice()->get_stream_mode_count(::rs::stream::color);
    int depth_width = 0, depth_height = 0, depth_fps = 0, depth_width_old = 0, depth_fps_old = 0,
        color_width = 0, color_height = 0, color_fps = 0, color_width_old = 0, color_fps_old = 0;
    ::rs::format format;
    for (int i = 0; i < depth_total; ++i)
    {
        m_device->getDevice()->get_stream_mode(::rs::stream::depth, i, depth_width, depth_height, format, depth_fps);
        if ( depth_width == depth_width_old && depth_fps == depth_fps_old)
        {
            continue;
        }
        depth_width_old = depth_width;
        depth_fps_old   = depth_fps;
        if (!only_depth)
        {
            for (int i = 0; i < color_total; ++i)
            {
                m_device->getDevice()->get_stream_mode(::rs::stream::color, i, color_width, color_height, format,
                                                       color_fps);
                if ((color_width == color_width_old && color_fps == color_fps_old) || (depth_fps != color_fps))
                {
                    continue;
                }
                color_width_old = color_width;
                color_fps_old   = color_fps;
                Mode mode;
                mode.fps          = static_cast<unsigned int>(depth_fps);
                mode.depth_width  = static_cast<unsigned int>(depth_width);
                mode.depth_height = static_cast<unsigned int>(depth_height);
                mode.color_width  = static_cast<unsigned int>(color_width);
                mode.color_height = static_cast<unsigned int>(color_height);
                modes.push_back(mode);
            }
        }
        else
        {
            Mode mode;
            mode.fps          = static_cast<unsigned int>(depth_fps);
            mode.depth_width  = static_cast<unsigned int>(depth_width);
            mode.depth_height = static_cast<unsigned int>(depth_height);
            modes.push_back(mode);
        }
    }
    return modes;
}

//-----------------------------------------------------------------------------

void RsGrabber::run ()
{
    ::rs::device* device = m_device->getDevice();
    device->start();
    unsigned int depth_width  = m_modeSelected.depth_width;
    unsigned int depth_height = m_modeSelected.depth_height;
    unsigned int depth_size   = depth_width * depth_height;
    unsigned int color_size   = m_modeSelected.color_width * m_modeSelected.color_height * 3;
    int color_width           = static_cast<int>(m_modeSelected.color_width);
    int color_height          = static_cast<int>(m_modeSelected.color_height);

    ///Buffer to store depth data
    std::vector<std::uint16_t> depth_data;
    ///Buffer to store color data
    std::vector<std::uint8_t> color_data;
    while (m_isRunning)
    {
        if (device->is_streaming())
        {
            device->wait_for_frames();
        }
        m_fpsMutex.lock();
        m_frequency.event();
        m_fpsMutex.unlock();
        // Retrieve our images
        const std::uint16_t* depth_image = (const std::uint16_t*)device->get_frame_data(
            ::rs::stream::depth_aligned_to_rectified_color);
        const std::uint8_t* color_image = (const std::uint8_t*) device->get_frame_data(::rs::stream::rectified_color);

        // Retrieve camera parameters for mapping between depth and color
        ::rs::intrinsics depth_intrin =
            device->get_stream_intrinsics(::rs::stream::depth_aligned_to_rectified_color);
        ::rs::intrinsics color_intrin   = device->get_stream_intrinsics(::rs::stream::rectified_color);
        ::rs::extrinsics depth_to_color = device->get_extrinsics(::rs::stream::depth_aligned_to_rectified_color,
                                                                 ::rs::stream::rectified_color);
        float scale = device->get_depth_scale();

        depth_data.clear();
        depth_data.resize(depth_size);
        ::pcl::PointCloud< ::pcl::PointXYZ>::Ptr xyz_cloud;
        ::pcl::PointCloud< ::pcl::PointXYZRGBA>::Ptr xyzrgba_cloud;
        static const float nan = std::numeric_limits<float>::quiet_NaN();

        memcpy(depth_data.data(), &depth_image[0], depth_size * sizeof (std::uint16_t));

        // TODO : copy data before sending
        if(m_needDepthImage)
        {
            m_depthImageSignal->operator()(depth_image);
        }

        if(m_needRGBImage)
        {
            m_rgbImageSignal->operator()(color_image);
        }

        if (m_needXYZRGBA)
        {
            color_data.clear();
            color_data.resize(color_size);
            memcpy(color_data.data(), &color_image[0], color_size * sizeof (std::uint8_t));
            xyzrgba_cloud.reset(new ::pcl::PointCloud< ::pcl::PointXYZRGBA> (depth_width, depth_height));
            xyzrgba_cloud->is_dense = false;
            if (m_needXYZ)
            {
                xyz_cloud.reset(new ::pcl::PointCloud< ::pcl::PointXYZ> (depth_width, depth_height));
                xyz_cloud->is_dense = false;
            }
            for (unsigned int dy = 0; dy < depth_height; ++dy)
            {
                //FIXME : if dy= 0 then i = -1 !! It works with line 380 (i++) but it can be more elegant.
                std::uint32_t i = dy * depth_width - 1;
                for (unsigned int dx = 0; dx < depth_width; ++dx)
                {
                    i++;
                    // Retrieve the 16-bit depth value and map it into a depth in meters
                    std::uint16_t depth_value = depth_data[i];

                    float depth_in_meters = depth_value * scale;

                    // Map from pixel coordinates in the depth image to real world co-ordinates
                    ::rs::float2 depth_pixel = {static_cast<float>(dx), static_cast<float>(dy)};
                    ::rs::float3 depth_point = depth_intrin.deproject(depth_pixel, depth_in_meters);
                    ::rs::float3 color_point = depth_to_color.transform(depth_point);
                    ::rs::float2 color_pixel = color_intrin.project(color_point);

                    const int cx = static_cast<int>(std::round(color_pixel.x)),
                              cy = static_cast<int>(std::round(color_pixel.y));

                    std::uint8_t red = 0, green = 0, blue = 0;
                    if (cx < 0 || cy < 0 || cx >= color_width || cy >= color_height)
                    {
                        red = 255; green = 255; blue = 255;
                    }
                    else
                    {
                        unsigned int pos = static_cast<unsigned int>((cy * color_width + cx) * 3);
                        red   = color_data[pos];
                        green = color_data[pos + 1];
                        blue  = color_data[pos + 2];
                    }
                    if (depth_value == 0 || depth_point.z > m_maxDistance)
                    {
                        xyzrgba_cloud->points[i].x = xyzrgba_cloud->points[i].y = xyzrgba_cloud->points[i].z =
                                                                                      (float) nan;
                        if (m_needXYZ)
                        {
                            xyz_cloud->points[i].x = xyz_cloud->points[i].y = xyz_cloud->points[i].z = (float) nan;
                        }
                        continue;
                    }
                    else
                    {
                        xyzrgba_cloud->points[i].x = -depth_point.x;
                        xyzrgba_cloud->points[i].y = -depth_point.y;
                        xyzrgba_cloud->points[i].z = depth_point.z;
                        xyzrgba_cloud->points[i].r = red;
                        xyzrgba_cloud->points[i].g = green;
                        xyzrgba_cloud->points[i].b = blue;
                        if (m_needXYZ)
                        {
                            xyz_cloud->points[i].x = -depth_point.x;
                            xyz_cloud->points[i].y = -depth_point.y;
                            xyz_cloud->points[i].z = depth_point.z;
                        }
                    }
                }
            }
            m_pointCloudRgbaSignal->operator () (xyzrgba_cloud);
            if (m_needXYZ)
            {
                m_pointCloudSignal->operator () (xyz_cloud);
            }
        }
        else if (m_needXYZ)
        {
            xyz_cloud.reset(new ::pcl::PointCloud< ::pcl::PointXYZ > (depth_width, depth_height));
            xyz_cloud->is_dense = false;
            for (unsigned int dy = 0; dy < depth_height; ++dy)
            {
                //FIXME : if dy= 0 then i = -1 !! It works with line 449 (i++) but it can be more elegant.
                std::uint32_t i = dy * depth_width - 1;
                for (unsigned int dx = 0; dx < depth_width; ++dx)
                {
                    i++;
                    // Retrieve the 16-bit depth value and map it into a depth in meters
                    std::uint16_t depth_value = depth_data[i];
                    float depth_in_meters     = depth_value * scale;
                    ::rs::float2 depth_pixel = {(float)dx, (float)dy};
                    ::rs::float3 depth_point = depth_intrin.deproject(depth_pixel, depth_in_meters);
                    if (depth_value == 0 || depth_point.z > m_maxDistance)
                    {
                        xyz_cloud->points[i].x = xyz_cloud->points[i].y = xyz_cloud->points[i].z = (float) nan;
                        continue;
                    }
                    else
                    {
                        xyz_cloud->points[i].x = -depth_point.x;
                        xyz_cloud->points[i].y = -depth_point.y;
                        xyz_cloud->points[i].z = depth_point.z;
                    }
                }
            }
            m_pointCloudSignal->operator () (xyz_cloud);
        }
    }
    device->stop();
}

//-----------------------------------------------------------------------------

float RsGrabber::getFramesPerSecond () const
{
    ::boost::mutex::scoped_lock lock(m_fpsMutex);
    return static_cast<float>(m_frequency.getFrequency());
}

//-----------------------------------------------------------------------------

void RsGrabber::enableTemporalFiltering (TemporalFilteringType type, size_t window_size)
{
    if (m_temporalFilteringType != type ||
        (type != RealSense_None && m_temporalFilteringWindowSize != window_size))
    {
        m_temporalFilteringType       = type;
        m_temporalFilteringWindowSize = window_size;
        if (m_isRunning)
        {
            stop();
            start();
        }
    }
}

//-----------------------------------------------------------------------------

void RsGrabber::disableTemporalFiltering ()
{
    enableTemporalFiltering(RealSense_None, 1);
}

//-----------------------------------------------------------------------------

void RsGrabber::setMode (const Mode& mode, bool strict)
{
    if (mode == m_modeRequested && strict == m_strict)
    {
        return;
    }
    m_modeRequested = mode;
    m_strict        = strict;
    if (m_isRunning)
    {
        stop();
        start();
    }
}

//-----------------------------------------------------------------------------

void RsGrabber::selectMode ()
{
    if (m_modeRequested == Mode())
    {
        m_modeRequested = Mode(30, 640, 480, 640, 480);
    }

    m_modeSelected = m_modeRequested;
}

//-----------------------------------------------------------------------------

const ::rs::intrinsics RsGrabber::getDepthIntrinsics() const
{
    ::rs::device* device          = m_device->getDevice();
    ::rs::intrinsics depth_intrin = device->get_stream_intrinsics(::rs::stream::depth_aligned_to_rectified_color);
    return depth_intrin;
}

//-----------------------------------------------------------------------------

const ::rs::intrinsics RsGrabber::getColorIntrinsics() const
{
    ::rs::device* device          = m_device->getDevice();
    ::rs::intrinsics color_intrin = device->get_stream_intrinsics(::rs::stream::rectified_color);
    return color_intrin;
}

//-----------------------------------------------------------------------------

const ::rs::extrinsics RsGrabber::getExtrinsics() const
{
    ::rs::device* device            = m_device->getDevice();
    ::rs::extrinsics depth_to_color = device->get_extrinsics(::rs::stream::depth_aligned_to_rectified_color,
                                                             ::rs::stream::rectified_color);
    return depth_to_color;
}

//-----------------------------------------------------------------------------

} //namespace realsenseTools
