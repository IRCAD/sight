
#pragma once

#include "videoRealSense/config.hpp"

#include <arData/FrameTL.hpp>

#include <arServices/IRGBDGrabber.hpp>

#include <fwThread/Worker.hpp>

#include <realsenseTools/RsGrabber.hpp>

#include <pcl/point_cloud.h>

namespace videoRealSense
{

/**
 * @brief   RealSense camera
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
   <service uid="videoGrabber" type ="::videoRealSense::SScanRS" autoConnect="no">
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
class VIDEOREALSENSE_CLASS_API SScanRS : public ::arServices::IRGBDGrabber
{
public:

    fwCoreServiceClassDefinitionsMacro( (SScanRS)(::arServices::IRGBDGrabber) );

    /// Constructor. Do nothing.
    VIDEOREALSENSE_API SScanRS() noexcept;

    /// Destructor. Do nothing.
    VIDEOREALSENSE_API virtual ~SScanRS() noexcept;

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
    virtual void startCamera();
    /// SLOT : Stops to grab frames.
    virtual void stopCamera();
    /// SLOT : Pause the grabbing
    virtual void pauseCamera();
    /// Not used
    virtual void toggleLoopMode()
    {
    }
    /// Not used
    virtual void setPosition(int64_t position)
    {
    }

    /// SLOT : Callback to grab the color image.
    void onCameraImage(const uint8_t* _buffer);

    /// SLOT : Callback to grab the depth image.
    void onCameraImageDepth(const uint16_t* _buffer);

    ///SLOT : Callback to grab the rgba point_cloud
    void onRgbaPointCloud(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr& _pc);

    std::string m_device; ///< If 'sense', adjust the resolution of frames. That will make the mesh coloring work.

    std::string m_depthKey; ///< Depth frame key.
    std::string m_colorKey; ///< Color frame key.

    // Timelines
    ::arData::FrameTL::sptr m_depthTimeline; ///< Depth timeline.
    ::arData::FrameTL::sptr m_colorTimeline; ///< Color timeline.

    std::unique_ptr< ::realsenseTools::RsGrabber > m_rsgrabber; ///< RealSense Grabber

    ::fwThread::Worker::sptr m_worker;  ///< Worker for the m_slotPresentFrame.
    mutable ::fwCore::mt::ReadWriteMutex m_videoFrameMutex; ///< Mutex to protect concurrent access for m_videoFrame.

    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr m_pointCloud; ///< pcl point cloud

};
}
