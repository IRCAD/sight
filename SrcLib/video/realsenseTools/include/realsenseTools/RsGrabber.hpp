/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

#pragma once

#include "realsenseTools/config.hpp"

#include <librealsense/rs.hpp>

#include <pcl/common/time.h>
#include <pcl/io/grabber.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <thread>

namespace realsenseTools
{
class Device;

/**
 * @brief The RealSense Grabber class.
 * This class will interface the librealsense api in order to grab webcam, depthMap and pointcloud.
 *
 * Code is inspired from a PCL pull-request (ttps://github.com/PointCloudLibrary/pcl/pull/1633)
 **/
class REALSENSETOOLS_CLASS_API RsGrabber : public ::pcl::Grabber
{

public:

    /**
     * Typedefs of signals
     */
    typedef
        void (sig_cb_real_sense_point_cloud)
        (const pcl::PointCloud<pcl::PointXYZ>::ConstPtr&);

    typedef
        void (sig_cb_real_sense_point_cloud_rgba)
        (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr&);

    typedef
        void (sig_cb_real_sense_depth_image)
        (const std::uint16_t*);

    typedef
        void (sig_cb_real_sense_rgb_image)
        (const std::uint8_t*);

    enum TemporalFilteringType
    {
        RealSense_None    = 0,
        RealSense_Median  = 1,
        RealSense_Average = 2,
    };

    /** A descriptor for capturing mode.
     *
     * Consists of framerate and resolutions of depth and color streams.
     * Serves two purposes: to describe the desired capturing mode when
     * creating a grabber, and to list the available modes supported by the
     * grabber (see getAvailableModes()). In the first case setting some
     * fields to zero means "don't care", i.e. the grabber is allowed to
     * decide itself which concrete values to use. */
    struct REALSENSETOOLS_CLASS_API Mode
    {
        unsigned int fps;
        unsigned int depth_width;
        unsigned int depth_height;
        unsigned int color_width;
        unsigned int color_height;

        /** Set all fields to zero (i.e. "don't care"). */
        REALSENSETOOLS_API Mode ();

        /** Set desired framerate, the rest is "don't care". */
        REALSENSETOOLS_API Mode (unsigned int fps);

        /** Set desired depth resolution, the rest is "don't care". */
        REALSENSETOOLS_API Mode (unsigned int depth_width, unsigned int depth_height);

        /** Set desired framerate and depth resolution, the rest is "don't
         * care". */
        REALSENSETOOLS_API Mode (unsigned int fps, unsigned int depth_width, unsigned int depth_height);

        /** Set desired depth and color resolution, the rest is "don't
         * care". */
        REALSENSETOOLS_API Mode (unsigned int depth_width, unsigned int depth_height, unsigned int color_width,
                                 unsigned int color_height);

        /** Set desired framerate, depth and color resolution. */
        REALSENSETOOLS_API Mode (unsigned int fps, unsigned int depth_width, unsigned int depth_height,
                                 unsigned int color_width, unsigned int color_height);

        REALSENSETOOLS_API bool operator== (const RsGrabber::Mode& m) const;
    };

    /// Static method to check if a device is available.
    REALSENSETOOLS_API static bool isAvailable();

    /** Create a grabber for a RealSense device.
     *
     * The grabber "captures" the device, making it impossible for other
     * grabbers to interact with it. The device is "released" when the
     * grabber is destructed.
     *
     * This will throw pcl::io::IOException if there are no free devices
     * that match the supplied \a device_id.
     *
     * \param[in] device_id device identifier, which can be a serial number,
     * a zero-based index (with '#' prefix), or an empty string (to select
     * the first available device)
     * \param[in] mode desired framerate and stream resolution (see Mode).
     * If the default is supplied, then the mode closest to VGA at 30 Hz
     * will be chosen.
     * \param[in] strict if set to \c true, an exception will be thrown if
     * device does not support exactly the mode requsted. Otherwise the
     * closest available mode is selected. */
    REALSENSETOOLS_API RsGrabber (const std::string& device_id = "", const Mode& mode = Mode(), bool strict = false);

    REALSENSETOOLS_API virtual ~RsGrabber () noexcept;

    REALSENSETOOLS_API virtual void start ();

    REALSENSETOOLS_API virtual void stop ();

    //------------------------------------------------------------------------------

    virtual bool isRunning () const
    {
        return m_isRunning;
    }

    //------------------------------------------------------------------------------

    virtual std::string getName () const
    {
        return (std::string("RsGrabber"));
    }

    REALSENSETOOLS_API virtual float getFramesPerSecond () const;

    /** Set the confidence threshold for depth data.
     *
     * Valid range is [0..15]. Discarded points will have their coordinates
     * set to NaNs). */
    REALSENSETOOLS_API void setConfidenceThreshold (unsigned int threshold);

    /** Enable temporal filtering of the depth data received from the device.
     *
     * The window size parameter is not relevant for `RealSense_None`
     * filtering type.
     *
     * \note if the grabber is running and the new parameters are different
     * from the current parameters, grabber will be restarted. */
    REALSENSETOOLS_API void enableTemporalFiltering (TemporalFilteringType type, size_t window_size);

    /** Disable temporal filtering. */
    REALSENSETOOLS_API void disableTemporalFiltering ();

    /** Get the serial number of device captured by the grabber. */
    REALSENSETOOLS_API const std::string& getDeviceSerialNumber () const;

    /** Get a list of capturing modes supported by the PXC device
     * controlled by this grabber.
     *
     * \param[in] only_depth list depth-only modes
     *
     * \note: this list exclude modes where framerates of the depth and
     * color streams do not match. */
    REALSENSETOOLS_API std::vector<Mode> getAvailableModes (bool only_depth = false) const;

    /** Set desired capturing mode.
     *
     * \note if the grabber is running and the new mode is different the
     * one requested previously, grabber will be restarted. */
    REALSENSETOOLS_API void setMode (const Mode& mode, bool strict = false);

    /** Get currently active capturing mode.
     *
     * \note: capturing mode is selected when start() is called; output of
     * this function before grabber was started is undefined. */
    const Mode& getMode () const
    {
        return m_modeSelected;
    }

    /// Return the depth camera intrinsic parameters
    REALSENSETOOLS_API const ::rs::intrinsics getDepthIntrinsics() const;

    /// Return the color camera intrinsic parameters
    REALSENSETOOLS_API const ::rs::intrinsics getColorIntrinsics() const;

    /// Return the extrinsic parameter depth to color
    REALSENSETOOLS_API const ::rs::extrinsics getExtrinsics() const;

    /**
     * @brief get the max distance (default 6 meters)
     * @return float the distance max
     */
    float getMaxDistance() const
    {
        return m_maxDistance;
    }

    /**
     * @brief setMaxDistance to take into account when compute the pointcloud.
     * @param _maxDistance float
     */
    void setMaxDistance(float _maxDistance)
    {
        m_maxDistance = _maxDistance;
    }

private:

    void run();

    void createDepthBuffer();

    void selectMode();

    /// Signals to indicate whether new clouds are available
    ::boost::signals2::signal<sig_cb_real_sense_point_cloud>* m_pointCloudSignal;
    ::boost::signals2::signal<sig_cb_real_sense_point_cloud_rgba>* m_pointCloudRgbaSignal;
    /// Signal to indicate whether new depth image is available
    ::boost::signals2::signal<sig_cb_real_sense_depth_image>* m_depthImageSignal;
    /// Signal to indicate whether new rgb image is available
    ::boost::signals2::signal<sig_cb_real_sense_rgb_image>* m_rgbImageSignal;

    ::boost::shared_ptr< realsenseTools::Device> m_device;

    bool m_isRunning;
    unsigned int m_confidenceThreshold;

    TemporalFilteringType m_temporalFilteringType;
    size_t m_temporalFilteringWindowSize;

    /// Capture mode requested by the user at construction time
    Mode m_modeRequested;

    /// Whether or not selected capture mode should strictly match what the user
    /// has requested
    bool m_strict;

    /// Capture mode selected by grabber (among the modes supported by the
    /// device), computed and stored on start()
    Mode m_modeSelected;

    /// Indicates whether there are subscribers for PointXYZ signal, computed
    /// and stored on start()
    bool m_needXYZ;

    /// Indicates whether there are subscribers for PointXYZRGBA signal,
    /// computed and stored on start()
    bool m_needXYZRGBA;

    /// Indicates whether there are subscribers for depth_image signal, computed
    /// and stored on start()
    bool m_needDepthImage;

    /// Indicates whether there are subscribers for rgb_image signal,
    /// computed and stored on start()
    bool m_needRGBImage;

    ::pcl::EventFrequency m_frequency;
    mutable ::boost::mutex m_fpsMutex;

    std::thread m_thread;

    float m_maxDistance;///< Max range distance to take into account( default: 6meters)

};

}//namespace realsense
