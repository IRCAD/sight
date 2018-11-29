/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "videoOpenni/config.hpp"

#include <arData/FrameTL.hpp>

#include <arServices/IRGBDGrabber.hpp>

#include <fwThread/Worker.hpp>

#include <OpenNI.h>

namespace videoOpenni
{

/**
 * @brief   Kinect or Sense camera
 *
 * This service grabs the depth, color, ir frames from a compatible RGBD camera (Kinect, Sense, etc...).
 * The frames are pushed into the timelines that are configured.
 * According to the device, it may not be possible to grab all streams simultaneously. On the Sense and the Kinect for
 * instance, it is not possible to get the color and the IR streams
 * together.
 *
 * \b Tags: DEVICE
 *
 * @section Slots Slots
 * - \b startCamera(): Start capturing frames from the camera.
 * - \b stopCamera(): Stop capturing frames from the camera.
 * - \b pauseCamera(): Pause frame capture (the device is still opened though).
 * - \b takeSnapshot(): Push a frame inside each of the configured snapshot timelines.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type ="::videoOpenni::SScan" >
        <inout key="depthTL" uid="..." />
        <inout key="frameTL" uid="..." />
        <inout key="irTL"    uid="..." />
        <inout key="snapshotTLDepth" uid="..." />
        <inout key="snapshotTLColors" uid="..." />
        <inout key="snapshotTLIR" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b depthTL [::arData::FrameTL]: Frame timeline of the depth stream.
 * - \b frameTL [::arData::FrameTL]: Frame timeline of the color stream.
 * - \b irTL [::arData::FrameTL]: Frame timeline of the infrared stream.
 * - \b snapshotTLDepth [::arData::FrameTL]: Frame timeline used to store snaphots of the color stream.
 * - \b snapshotTLColors [::arData::FrameTL]: Frame timeline used to store snaphots of the depth stream.
 * - \b snapshotTLIR [::arData::FrameTL]: Frame timeline used to store snaphots of the infrared stream.
 */
class VIDEOOPENNI_CLASS_API SScan : public ::arServices::IRGBDGrabber
{
public:

    fwCoreServiceClassDefinitionsMacro( (SScan)(::arServices::IRGBDGrabber) );

    /// Constructor. Creates/Connects slots and creates a worker for the frame grabber slot.
    VIDEOOPENNI_API SScan() noexcept;

    /// Destructor. Does nothing.
    VIDEOOPENNI_API virtual ~SScan() noexcept;

    /**
     * @name Data API
     * @{
     */
    VIDEOOPENNI_API static const ::fwServices::IService::KeyType s_IRTL_INOUT;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VIDEOOPENNI_API static const ::fwCom::Slots::SlotKeyType s_TAKE_SNAPSHOT_FRAME;
    ///@}

protected:

    /// Do nothing.
    VIDEOOPENNI_API virtual void configuring() override;

    /// Initialize the layout and the cameras.
    VIDEOOPENNI_API virtual void starting() override;

    /// Destroy the layout. Shutdowns the streams.
    VIDEOOPENNI_API virtual void stopping() override;

    /// Do nothing.
    VIDEOOPENNI_API virtual void updating() override;

private:

    /// SLOT : Initializes and starts the streams. Restarts the streams if already started.
    virtual void startCamera() override;

    /// SLOT : Stops to grab frames.
    virtual void stopCamera() override;

    /// SLOT : Grabs and pushes depth and color frames in their corresponding timelines.
    virtual void presentFrame();

    /// SLOT : Pause the grabbing
    virtual void pauseCamera() override;

    ///Not used
    virtual void toggleLoopMode() override
    {
    }
    ///Not used
    virtual void setPosition(int64_t position) override
    {
    }

    /// SLOT : Take a snapshot
    void takeSnapshot();

    ::openni::VideoFrameRef m_depthFrame; ///< Frame of the depth information.
    ::openni::VideoFrameRef m_colorFrame; ///< Frame of the color information.
    ::openni::VideoFrameRef m_irFrame; ///< Frame of the infrared information.

    // Timelines
    ::arData::FrameTL::sptr m_depthTL; ///< Depth timeline.
    ::arData::FrameTL::sptr m_colorTL; ///< Color timeline.
    ::arData::FrameTL::sptr m_irTL; ///< Infrared timeline.

    // Snapshot timelines
    ::arData::FrameTL::sptr m_snapshotDepthTL; ///< Depth timeline.
    ::arData::FrameTL::sptr m_snapshotColorTL; ///< Color timeline.
    ::arData::FrameTL::sptr m_snapshotIRTL; ///< Infrared timeline.

    // Video Streams
    ::openni::VideoStream m_depthStream;   ///< Depth stream.
    ::openni::VideoStream m_colorStream;   ///< Color stream.
    ::openni::VideoStream m_irStream;   ///< Infrared stream.

    ::openni::VideoStream** m_streams; ///< All streams together, needed to provide to OpenNI.
    std::map<size_t, ::openni::VideoStream*> m_streamMap; ///< Video streams map

    // Device
    ::openni::Device m_camera; ///< OpenNI camera.
    ::openni::Status m_status; ///< OpenNI status.

    ///private slot type
    typedef ::fwCom::Slot<void ()> PresentFrameSlotType;
    PresentFrameSlotType::sptr m_slotPresentFrame; ///< Slot for the present frame method.

    ::fwThread::Worker::sptr m_worker;  ///< Worker for the m_slotPresentFrame.

    mutable ::fwCore::mt::ReadWriteMutex m_videoFrameMutex; ///< Mutex to protect concurrent access of m_videoFrame.

    bool m_capture; ///< Whether we are capturing frames.

    bool m_pause; ///< Whether we are in pause
};
}
