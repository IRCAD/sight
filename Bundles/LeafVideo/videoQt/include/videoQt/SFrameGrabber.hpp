/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_SFRAMEGRABBER_HPP__
#define __VIDEOQT_SFRAMEGRABBER_HPP__

#include <QObject>
#include <QPointer>
#include <QCamera>
#include <QImage>
#include <QMediaPlayer>
#include <QVideoFrame>

#include <fwTools/Failed.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>

#include "videoQt/config.hpp"

class QVideoFrame;
fwCorePredeclare( (arData)(Camera) )
fwCorePredeclare( (videoQt)(VideoSurfaceQt) )

namespace videoQt
{

/**
 * @brief   Defines the service which grab video frame.
 * @class   SFrameGrabber
 */
class VIDEOQT_CLASS_API SFrameGrabber : public QObject,
                                        public ::fwServices::IController
{
Q_OBJECT;
public:


    fwCoreServiceClassDefinitionsMacro ( (SFrameGrabber)(::fwServices::IController) );

    /// Constructor. Do nothing.
    VIDEOQT_API SFrameGrabber() throw();

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SFrameGrabber() throw();

    /**
     * @name Signals API
     * @{
     */

    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_POSITION_MODIFIED_SIG;
    typedef ::fwCom::Signal<void (qint64)> PositionModifiedSignalType;

    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_DURATION_MODIFIED_SIG;
    typedef ::fwCom::Signal<void (qint64)> DurationModifiedSignalType;

    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_FRAME_PRESENTED_SIG;
    typedef ::fwCom::Signal<void ()> FramePresentedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_START_CAMERA_SLOT;
    typedef ::fwCom::Slot<void ()> StartCameraSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_STOP_CAMERA_SLOT;
    typedef ::fwCom::Slot<void ()> StopCameraSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_PAUSE_CAMERA_SLOT;
    typedef ::fwCom::Slot<void ()> PauseCameraSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SELECT_CAMERA_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> SelectCameraSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_LOOP_VIDEO_SLOT;
    typedef ::fwCom::Slot<void ()> LoopVideoSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SET_POSITION_VIDEO_SLOT;
    typedef ::fwCom::Slot<void (int)> SetPositionVideoSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_PRESENT_SLOT;
    typedef ::fwCom::Slot<void ()> PresentFrameSlotType;
    ///@}

    /// Set the video frame. It is not a copy, the image data inside QVideoFrame is smart pointed.
    VIDEOQT_API void setVideoFrame(const QVideoFrame& videoFrame);

protected:

    /// Initialize the layout and the camera.
    VIDEOQT_API virtual void starting() throw( ::fwTools::Failed );

    /// Destroys the layout.
    VIDEOQT_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing.
    VIDEOQT_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @verbatim
       <service uid="${GENERIC_UID}_VideoGrabber" impl="::videoQt::SFrameGrabber" autoConnect="no">
           <cameraFwId>cameraID</cameraFwId>
       </service>
       @endverbatim
     * - \b cameraFwId: fwID of the arData::Camera used to display video.
     **/
    VIDEOQT_API virtual void configuring() throw( ::fwTools::Failed );

    /// SLOT : Initialize and start camera (restart camera if is already started)
    void startCamera();
    /// SLOT : Stop camera
    void stopCamera();
    /// SLOT : Pause camera
    void pauseCamera();
    /// SLOT : Select camera
    void selectCamera(const std::string& cameraID);
    /// SLOT : enable/disable loop in video
    void toogleLoopMode();
    /// SLOT : set the new position in the video.
    void setPosition(const int position);
    /// SLOT: copy and push video frame in the timeline.
    void presentFrame();
    /// Gets camera from m_cameraID
    SPTR(::arData::Camera) getCamera();

    /// Mirrored the frame in the desired direction (Only available on Windows platform)
    void setMirror(bool horizontallyFlip = false, bool verticallyFlip = false)
    {
        m_horizontallyFlip = horizontallyFlip;
        m_verticallyFlip   = verticallyFlip;
    }

protected Q_SLOTS:

    /// Call when duration of the video changed.
    void onDurationChanged(qint64 duration);

    /// Call when reading position changed in the video.
    void onPositionChanged(qint64 position);

private:

    /// Slot to call present frame method
    PresentFrameSlotType::sptr m_slotPresentFrame;

    /// FwID of arData::Camera
    std::string m_cameraID;

    /// Qt video surface
    QPointer<VideoSurfaceQt> m_videoSurface;

    /// Media player
    QPointer<QMediaPlayer> m_mediaPlayer;

    /// Play list
    QPointer<QMediaPlaylist> m_playlist;

    /// state of the loop mode
    bool m_loopVideo;

    /// Camera
    QPointer<QCamera> m_camera;

    /// Video frame
    QVideoFrame m_videoFrame;

    /// Worker for the m_slotPresentFrame
    ::fwThread::Worker::sptr m_worker;

    /// Mutex to protect concurrent access for m_videoFrame
    mutable ::fwCore::mt::ReadWriteMutex m_videoFrameMutex;

    /// Mirror frame in horizontal direction
    bool m_horizontallyFlip;
    /// Mirror frame in vertical direction
    bool m_verticallyFlip;
};
}

#endif /*__VIDEOQT_SFRAMEGRABBER_HPP__*/


