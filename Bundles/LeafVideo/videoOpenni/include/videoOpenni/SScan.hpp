/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOOPENNI_SSCAN_HPP__
#define __VIDEOOPENNI_SSCAN_HPP__

#include "videoOpenni/config.hpp"

#include <arData/FrameTL.hpp>

#include <arServices/IGrabber.hpp>

#include <fwThread/Worker.hpp>

#include <OpenNI.h>

namespace videoOpenni
{

/**
 * @brief   This service grabs the depth and color frames from a compatible RGBD camera (Kinect, Sense, etc...).
 * The frames are pushed into timelines.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type ="::videoOpenni::SScan" >
        <inout key="frameTLDepth" uid="..." />
        <inout key="frameTLColors" uid="..." />
        <inout key="cameraSeries" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b frameDepth [::arData::FrameTL]: Frame timeline of the depth video.
 * - \b frameColor [::arData::FrameTL]: Frame timeline of the color video.
 * - \b cameraSeries [::arData::CameraSeries]: Camera series that will contain device camera information.
 */
class VIDEOOPENNI_CLASS_API SScan : public ::arServices::IGrabber
{
public:

    fwCoreServiceClassDefinitionsMacro( (SScan)(::arServices::IGrabber) );

    /// Constructor. Creates/Connects slots and creates a worker for the frame grabber slot.
    VIDEOOPENNI_API SScan() throw();

    /// Destructor. Does nothing.
    VIDEOOPENNI_API virtual ~SScan() throw();

protected:

    /// Do nothing.
    VIDEOOPENNI_API virtual void configuring() throw( ::fwTools::Failed );

    /// Initialize the layout and the cameras.
    VIDEOOPENNI_API virtual void starting() throw( ::fwTools::Failed );

    /// Destroy the layout. Shutdowns the streams.
    VIDEOOPENNI_API virtual void stopping() throw( ::fwTools::Failed );

    /// Do nothing.
    VIDEOOPENNI_API virtual void updating() throw(::fwTools::Failed);

private:

    /// SLOT : Initializes and starts the streams. Restarts the streams if already started.
    virtual void startCamera();

    /// SLOT : Stops to grab frames.
    virtual void stopCamera();

    /// SLOT : Grabs and pushes depth and color frames in their corresponding timelines.
    virtual void presentFrame();

    ///SLOT : Pause the grabbing
    virtual void pauseCamera();
    ///Not used
    virtual void toggleLoopMode()
    {
    }
    ///Not used
    virtual void setPosition(int64_t position)
    {
    }

    ::openni::VideoFrameRef m_depthFrame; ///< Frame of the depth information.
    ::openni::VideoFrameRef m_colorFrame; ///< Frame of the color information.

    // Timelines
    ::arData::FrameTL::sptr m_depthTimeline; ///< Depth timeline.
    ::arData::FrameTL::sptr m_colorTimeline; ///< Color timeline.
    ::arData::FrameTL::sptr m_positionsTimeline; ///< Positions timeline.

    // Video Streams
    ::openni::VideoStream m_depthStream; ///< Depth stream.
    ::openni::VideoStream m_colorStream; ///< Color stream.
    ::openni::VideoStream**       m_streams; ///< Ensemble of the streams.

    // Device
    ::openni::Device m_camera; ///< OpenNI camera.
    ::openni::Status m_status; ///< OpenNI status.

    ///private slot type
    typedef ::fwCom::Slot<void ()> PresentFrameSlotType;
    PresentFrameSlotType::sptr m_slotPresentFrame; ///< Slot for the present frame method.

    ::fwThread::Worker::sptr m_worker;  ///< Worker for the m_slotPresentFrame.

    mutable ::fwCore::mt::ReadWriteMutex m_videoFrameMutex; ///< Mutex to protect concurrent access of m_videoFrame.

};
}

#endif /*__VIDEOOPENNI_SSCAN_HPP__*/

