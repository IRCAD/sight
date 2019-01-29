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

#include <fwThread/Worker.hpp>

#include <librealsense2/rs.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

namespace videoRealSense
{

/**
 * @brief  RealSense Camera
 *
 * This service grabs the depth and color frames from a compatible device.
 * The frames are pushed into timelines.
 *
 * \b Tags: DEVICE
 *
 * @section Slots Slots
 * - \b startCamera() : This slot is called to initialize and start camera (restart camera if is already started).
 * - \b stopCamera()  : This slot is called to stop cameras.
 * - \b presentSlot() : This slot is called to push depth and color frames in their corresponding timelines.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="videoGrabber" type ="::videoRealSense::SScan" autoConnect="no">
        <inout key="depthTL" uid="..." />
        <inout key="frameTL" uid="..." />
        <inout key="cameraSeries" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b depthTL [::arData::FrameTL]: Frame timeline of the depth video.
 * - \b frameTL [::arData::FrameTL]: Frame timeline of the color video.
 * - \b cameraSeries [::arData::CameraSeries]: Camera series that will contain device camera information.
 */
class VIDEOREALSENSE_CLASS_API SScan : public ::arServices::IRGBDGrabber
{
public:

    fwCoreServiceClassDefinitionsMacro( (SScan)(::arServices::IRGBDGrabber) )

    /// Constructor. Do nothing.
    VIDEOREALSENSE_API SScan() noexcept;

    /// Destructor. Do nothing.
    VIDEOREALSENSE_API virtual ~SScan() noexcept;

protected:

    /// Initialize the layout and the streams.
    VIDEOREALSENSE_API virtual void starting() override;

    /// Destroys the layout. Shutdowns the streams.
    VIDEOREALSENSE_API virtual void stopping() override;

    /// Does nothing.
    VIDEOREALSENSE_API virtual void updating() override;

    /// Configures the service. Gets the depth and color frames timelines and gets the device used.
    VIDEOREALSENSE_API virtual void configuring() override;

private:

    /// SLOT : Initializes and starts the streams. Restarts the streams if already started.
    virtual void startCamera() override;
    /// SLOT : Stops to grab frames.
    virtual void stopCamera() override;
    /// SLOT : Pause the grabbing
    virtual void pauseCamera() override;
    /// Not used
    virtual void toggleLoopMode() override
    {
    }
    /// Not used
    virtual void setPosition(int64_t position) override
    {
    }

    /// SLOT : Callback to grab the color image.
    void onCameraImage(const uint8_t* _buffer);

    /// SLOT : Callback to grab the depth image.
    void onCameraImageDepth(const uint16_t* _buffer);

    ///SLOT : Callback to grab the rgba point_cloud
    void onRgbaPointCloud(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr& _pc);

    /**
     * @brief selectDevice is a private function called on start of the service, it will detect if realsense cameras
     * are plugged in, and show a dialog to user if several realsense cameras are detected.
     * @note This function may erase previous choice made in videoQt::SCamera.
     * @todo This function should be merged in videoQt::SCamera, or in a more generic "camera selector".
     * @return a string containing the serial number of selected camera.
     */
    std::string selectDevice();

    std::unique_ptr< ::rs2::pipeline> m_pipe; ///< Pipeline
    std::string m_device; ///< If 'sense', adjust the resolution of frames. That will make the mesh coloring work.

    std::string m_depthKey; ///< Depth frame key.
    std::string m_colorKey; ///< Color frame key.

    // Timelines
    ::arData::FrameTL::sptr m_depthTimeline; ///< Depth timeline.
    ::arData::FrameTL::sptr m_colorTimeline; ///< Color timeline.

    // Grabber worker thread
    std::thread m_thread;

    bool m_running { false };

    ::fwThread::Worker::sptr m_worker;  ///< Worker for the m_slotPresentFrame.
    mutable ::fwCore::mt::ReadWriteMutex m_videoFrameMutex; ///< Mutex to protect concurrent access for m_videoFrame.

    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr m_pointCloud; ///< pcl point cloud

    /// Determine depth value corresponding to one meter
    float m_depthScale;
};
}
