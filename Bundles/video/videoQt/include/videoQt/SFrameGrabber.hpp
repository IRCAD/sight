/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "videoQt/config.hpp"

#include <arServices/IGrabber.hpp>

#include <fwThread/Worker.hpp>

#include <fwVideoQt/Player.hpp>

#include <QImage>
#include <QObject>
#include <QPointer>

namespace arData
{
class Camera;
}

namespace videoQt
{

/**
 * @brief   QtMultimedia player
 *
 * Defines the service grabbing video frame.
 *
 * \b Tags: FILE,STREAM,DEVICE
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t) : Emitted when the position in the video is modified during playing.
 * - \b durationModified(std::int64_t) : Emitted when the duration of the video is modified.
 *
 * @section Slots Slots
 * - \b startCamera() : Start playing the camera or the video.
 * - \b stopCamera() : Stop playing the camera or the video.
 * - \b pauseCamera() : Pause the video, it has no effect when playing a camera.
 * - \b loopVideo() : Toggle the loop of the playing.
 * - \b setPositionVideo(int) : Force the current time in the video.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoQt::SFrameGrabber">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [::arData::FrameTL]: timeline where to extract the video frames.
 */
class VIDEOQT_CLASS_API SFrameGrabber : public QObject,
                                        public ::arServices::IGrabber
{
Q_OBJECT;
public:

    fwCoreServiceMacro(SFrameGrabber, ::arServices::IGrabber);

    /// Constructor. Do nothing.
    VIDEOQT_API SFrameGrabber() noexcept;

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SFrameGrabber() noexcept;

protected:

    /// Initialize the layout and the camera.
    VIDEOQT_API virtual void starting() override;

    /// Destroy the layout.
    VIDEOQT_API virtual void stopping() override;

    /// Do nothing.
    VIDEOQT_API virtual void updating() override;

    /// Do nothing.
    VIDEOQT_API virtual void configuring() override;

    /// SLOT : Initialize and start camera (restart camera if is already started)
    virtual void startCamera() override;

    /// SLOT : Stop camera
    virtual void stopCamera() override;

    /// SLOT : Pause camera
    virtual void pauseCamera() override;

    /// SLOT : enable/disable loop in video
    virtual void toggleLoopMode() override;

    /// SLOT : set the new position in the video.
    virtual void setPosition(int64_t position) override;

protected Q_SLOTS:

    /// Call when duration of the video changed.
    void onDurationChanged(qint64 duration);

    /// Call when reading position changed in the video.
    void onPositionChanged(qint64 position);

    /// Call our internal slots.
    void onPresentFrame(const QVideoFrame& frame);

private:
    /// SLOT: copy and push video frame in the timeline.
    void presentFrame(const QVideoFrame& frame);

    /// Mirrored the frame in the desired direction
    void setMirror(bool horizontallyFlip = false, bool verticallyFlip = false)
    {
        m_horizontallyFlip = horizontallyFlip;
        m_verticallyFlip   = verticallyFlip;
    }

    /// FwID of arData::Camera
    std::string m_cameraID;

    /// state of the loop mode
    bool m_loopVideo;

    /// Video frame
    QVideoFrame m_videoFrame;

    /// Camera
    ::fwVideoQt::Player* m_videoPlayer;

    /// Slot to call present frame method
    ::fwCom::Slot<void(const QVideoFrame& frame)>::sptr m_slotPresentFrame;

    /// Worker for the m_slotPresentFrame
    ::fwThread::Worker::sptr m_workerPresentFrame;

    /// Mutex to protect concurrent access for m_videoFrame
    mutable ::fwCore::mt::ReadWriteMutex m_videoFrameMutex;

    /// Mirror frame in horizontal direction
    bool m_horizontallyFlip;

    /// Mirror frame in vertical direction
    bool m_verticallyFlip;
};

} // namespace videoQt
