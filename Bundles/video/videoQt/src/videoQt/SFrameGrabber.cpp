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

#include "videoQt/SFrameGrabber.hpp"

#include "fwVideoQt/Registry.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/helper/Image.hpp>

#include <filesystem>
#include <QImage>
#include <QSize>
#include <QVideoFrame>

namespace videoQt
{

//-----------------------------------------------------------------------------

SFrameGrabber::SFrameGrabber() noexcept :
    m_loopVideo(false),
    m_videoPlayer(nullptr),
    m_horizontallyFlip(false),
    m_verticallyFlip(false)
{
    // Do not register the slot in the service, we want to put it on its own worker
    m_slotPresentFrame = ::fwCom::newSlot( &SFrameGrabber::presentFrame, this );
}

//-----------------------------------------------------------------------------

SFrameGrabber::~SFrameGrabber() noexcept
{
}

//-----------------------------------------------------------------------------

void SFrameGrabber::starting()
{
    SLM_ASSERT("m_videoPlayer must be null - have you called starting() twice ?", nullptr == m_videoPlayer);

    // Create a worker for the frame copy, we don't want this on the main thread
    // since it takes around 4/9ms (release/debug) to copy a full HD frame
    m_workerPresentFrame = ::fwThread::Worker::New();
    m_slotPresentFrame->setWorker(m_workerPresentFrame);
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopping()
{
    this->stopCamera();

    ::fwCore::mt::WriteLock lock(m_videoFrameMutex);
    m_workerPresentFrame->stop();
    m_workerPresentFrame.reset();
}

//-----------------------------------------------------------------------------

void SFrameGrabber::configuring()
{
}

//-----------------------------------------------------------------------------

void SFrameGrabber::updating()
{
}

//-----------------------------------------------------------------------------

void SFrameGrabber::startCamera()
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera>("camera");
    FW_RAISE_IF("Camera not found", !camera);
    ::arData::Camera::SourceType eSourceType = camera->getCameraSource();

    // Make sure the user has selected a valid source
    if( ::arData::Camera::UNKNOWN != eSourceType )
    {
        this->stopCamera();
        this->setMirror(false, false);

    #ifdef WIN32
        if( ::arData::Camera::DEVICE == eSourceType )
        {
            this->setMirror(false, true);
        }
    #endif

        auto& registry = ::fwVideoQt::Registry::getInstance();
        m_videoPlayer = registry.requestPlayer(camera);
        if(m_videoPlayer)
        {
            QObject::connect(m_videoPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
            QObject::connect(m_videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
            QObject::connect(m_videoPlayer, SIGNAL(frameAvailable(QVideoFrame)), this,
                             SLOT(onPresentFrame(QVideoFrame)));

            m_videoPlayer->play();

            auto sig = this->signal< ::arServices::IGrabber::CameraStartedSignalType >(
                ::arServices::IGrabber::s_CAMERA_STARTED_SIG);
            sig->asyncEmit();

            this->setStartState(true);
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::pauseCamera()
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the
    // "pause" button
    if(m_videoPlayer)
    {
        m_videoPlayer->pause();
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopCamera()
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the
    // "pause" button
    if(m_videoPlayer)
    {
        m_videoPlayer->stop();

        auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
        sigPosition->asyncEmit(static_cast<std::int64_t>(-1));

        auto sigDuration = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
        sigDuration->asyncEmit(static_cast<std::int64_t>(-1));

        QObject::disconnect(m_videoPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
        QObject::disconnect(m_videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
        QObject::disconnect(m_videoPlayer, SIGNAL(frameAvailable(QVideoFrame)), this,
                            SLOT(onPresentFrame(QVideoFrame)));

        auto& registry = ::fwVideoQt::Registry::getInstance();
        registry.releasePlayer(m_videoPlayer);

        m_videoPlayer = nullptr;

        // Reset the timeline and send a black frame
        ::arData::FrameTL::sptr timeline = this->getInOut< ::arData::FrameTL >("frameTL");
        this->clearTimeline(timeline);

        auto sig = this->signal< ::arServices::IGrabber::CameraStoppedSignalType >(
            ::arServices::IGrabber::s_CAMERA_STOPPED_SIG);
        sig->asyncEmit();

        this->setStartState(false);
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::toggleLoopMode()
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the
    // "pause" button
    if(m_videoPlayer)
    {
        m_loopVideo = !m_loopVideo;
        m_videoPlayer->toggleLoopMode(m_loopVideo);
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::setPosition(std::int64_t position)
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the
    // "pause" button
    if(m_videoPlayer)
    {
        m_videoPlayer->setPosition(position);
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::onPositionChanged(qint64 position)
{
    auto sig = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    sig->asyncEmit(static_cast<std::int64_t>(position));
}

//----------------------------------------------------------------------------

void SFrameGrabber::onDurationChanged(qint64 duration)
{
    auto sig = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
    sig->asyncEmit(static_cast<std::int64_t>(duration));
}

//----------------------------------------------------------------------------

void SFrameGrabber::onPresentFrame(const QVideoFrame& frame)
{
    m_slotPresentFrame->asyncRun(std::ref(frame));
}

//----------------------------------------------------------------------------

void SFrameGrabber::presentFrame(const QVideoFrame& frame)
{
    ::fwCore::mt::WriteLock lock(m_videoFrameMutex);

    if( frame.pixelFormat() == QVideoFrame::Format_Invalid )
    {
        SLM_WARN("Dropped frame");
        return;
    }

    // const_cast required to call function map() - but it's safe, since we map for READ_ONLY access !
    QVideoFrame& mappedFrame = const_cast< QVideoFrame& >( frame );
    // Make sure the frame has been mapped before accessing .bits() !!
    mappedFrame.map(QAbstractVideoBuffer::ReadOnly);

    // If we have the same output format, we can take the fast path
    const int width  = frame.width();
    const int height = frame.height();

    ::arData::FrameTL::sptr timeline = this->getInOut< ::arData::FrameTL >("frameTL");
    if(static_cast<unsigned int>(height) != timeline->getHeight() ||
       static_cast<unsigned int>(width) != timeline->getWidth())
    {
        timeline->initPoolSize(static_cast<size_t>(width), static_cast<size_t>(height), ::fwTools::Type::s_UINT8, 4);
    }

    SLM_ASSERT("Pixel format must be RGB32", frame.pixelFormat() == QVideoFrame::Format_RGB32 ||
               frame.pixelFormat() == QVideoFrame::Format_ARGB32_Premultiplied ||
               frame.pixelFormat() == QVideoFrame::Format_ARGB32);

    // Keep this QImage in the global scope, until we are done with it !
    QImage imgFlipped;

    const std::uint64_t* frameBuffer = nullptr;
    if( m_horizontallyFlip || m_verticallyFlip )
    {
        imgFlipped = QImage(mappedFrame.bits(),
                            width,
                            height,
                            QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()))
                     .mirrored(m_horizontallyFlip, m_verticallyFlip);

        frameBuffer = reinterpret_cast< const std::uint64_t*>( imgFlipped.bits() );
    }
    else
    {
        frameBuffer = reinterpret_cast< const std::uint64_t*>( mappedFrame.bits() );
    }

    // Unmap when we don't need access to .bits() any longer
    mappedFrame.unmap();

    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    SPTR(::arData::FrameTL::BufferType) buffer = timeline->createBuffer(timestamp);
    std::uint64_t* destBuffer = reinterpret_cast< std::uint64_t* >( buffer->addElement(0) );

    const unsigned int size = static_cast<unsigned int>(width*height) >> 1;

    // Buffer conversion: Qt returns frame in ARGB format, but we need RGBA.
    for(unsigned int idx = 0; idx < size; ++idx)
    {
        const std::uint64_t pixel = *frameBuffer++;

        *destBuffer++ = 0xFF000000FF000000 | (pixel & 0x0000FF000000FF00) |
                        (pixel & 0x000000FF000000FF) << 16 | (pixel & 0x00FF000000FF0000) >> 16;
    }

    // push buffer and notify
    timeline->pushObject(buffer);

    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = timeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

} // namespace videoQt
