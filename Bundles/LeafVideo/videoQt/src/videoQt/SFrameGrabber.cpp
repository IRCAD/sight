/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/VideoSurfaceQt.hpp"
#include "videoQt/SFrameGrabber.hpp"
#include "videoQt/helper/preferences.hpp"

#include <fwCore/base.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>
#include <fwComEd/helper/Image.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwTools/Type.hpp>
#include <fwServices/Base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <extData/FrameTL.hpp>
#include <arData/Camera.hpp>

#include <QImage>
#include <QSize>
#include <QVideoFrame>
#include <QMediaPlaylist>

#include <boost/detail/endian.hpp>
#include <boost/filesystem/operations.hpp>

namespace videoQt
{

//-----------------------------------------------------------------------------


fwServicesRegisterMacro( ::fwServices::IController, ::videoQt::SFrameGrabber, ::extData::FrameTL);

const ::fwCom::Signals::SignalKeyType SFrameGrabber::s_POSITION_MODIFIED_SIG = "positionModified";
const ::fwCom::Signals::SignalKeyType SFrameGrabber::s_DURATION_MODIFIED_SIG = "durationModified";
const ::fwCom::Signals::SignalKeyType SFrameGrabber::s_FRAME_PRESENTED_SIG   = "framePresented";

const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_START_CAMERA_SLOT       = "startCamera";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_STOP_CAMERA_SLOT        = "stopCamera";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_PAUSE_CAMERA_SLOT       = "pauseCamera";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_SELECT_CAMERA_SLOT      = "selectCamera";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_LOOP_VIDEO_SLOT         = "loopVideo";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_SET_POSITION_VIDEO_SLOT = "setPositionVideo";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_PRESENT_SLOT            = "presentSlot";

//-----------------------------------------------------------------------------

SFrameGrabber::SFrameGrabber() throw() : m_cameraID(""),
                                         m_loopVideo(false),
                                         m_horizontallyFlip(false),
                                         m_verticallyFlip(false)
{
    m_slotStartCamera      = ::fwCom::newSlot( &SFrameGrabber::startCamera, this );
    m_slotStopCamera       = ::fwCom::newSlot( &SFrameGrabber::stopCamera, this );
    m_slotPauseCamera      = ::fwCom::newSlot( &SFrameGrabber::pauseCamera, this );
    m_slotSelectCamera     = ::fwCom::newSlot( &SFrameGrabber::selectCamera, this );
    m_slotLoopVideo        = ::fwCom::newSlot( &SFrameGrabber::toogleLoopMode, this );
    m_slotSetPositionVideo = ::fwCom::newSlot( &SFrameGrabber::setPosition, this );
    m_slotPresentFrame     = ::fwCom::newSlot( &SFrameGrabber::presentFrame, this );

    m_sigPositionModified = PositionModifiedSignalType::New();
    m_sigDurationModified = DurationModifiedSignalType::New();
    m_sigFramePresented   = FramePresentedSignalType::New();

    ::fwCom::HasSignals::m_signals(s_POSITION_MODIFIED_SIG, m_sigPositionModified)
        (s_DURATION_MODIFIED_SIG, m_sigDurationModified)
        (s_FRAME_PRESENTED_SIG, m_sigFramePresented);

    ::fwCom::HasSlots::m_slots( s_START_CAMERA_SLOT, m_slotStartCamera)
        ( s_STOP_CAMERA_SLOT, m_slotStopCamera)
        ( s_PAUSE_CAMERA_SLOT, m_slotPauseCamera)
        ( s_SELECT_CAMERA_SLOT, m_slotSelectCamera)
        ( s_LOOP_VIDEO_SLOT, m_slotLoopVideo)
        ( s_SET_POSITION_VIDEO_SLOT, m_slotSetPositionVideo)
        ( s_PRESENT_SLOT, m_slotPresentFrame);

#ifdef COM_LOG
    ::fwCom::HasSlots::m_slots.setID();
#endif

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

    // Create a worker for the frame copy, we don't want this on the main thread
    // since it takes around 4/9ms (release/debug) to copy a full HD frame
    m_worker = ::fwThread::Worker::New();
    m_slotPresentFrame->setWorker(m_worker);

    m_connections = ::fwServices::helper::SigSlotConnection::New();
}

//-----------------------------------------------------------------------------

SFrameGrabber::~SFrameGrabber() throw()
{
}

//-----------------------------------------------------------------------------

void SFrameGrabber::starting() throw(::fwTools::Failed)
{

    m_connections->connect(this->getSptr(), ::videoQt::SFrameGrabber::s_FRAME_PRESENTED_SIG, this->getSptr(),
                           ::videoQt::SFrameGrabber::s_PRESENT_SLOT);

    // Always create the media player, it does not hurt that much if we don't use it later...
    m_mediaPlayer = new QMediaPlayer(0, QMediaPlayer::VideoSurface);

    ::extData::FrameTL::sptr timeline = this->getObject< ::extData::FrameTL >();
    m_videoSurface                    = new VideoSurfaceQt(this);
    m_videoSurface->setTimeline(timeline);
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopping() throw(::fwTools::Failed)
{
    m_connections->disconnect();
    this->stopCamera();

    delete m_mediaPlayer;
    m_mediaPlayer.clear();

    delete m_videoSurface;
    m_videoSurface.clear();
}

//-----------------------------------------------------------------------------

void SFrameGrabber::configuring()  throw ( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElement::sptr cameraCfg = m_configuration->findConfigurationElement("cameraFwId");
    SLM_ASSERT("Missing tag 'cameraFwId'", cameraCfg);
    m_cameraID = cameraCfg->getValue();
    SLM_ASSERT("tag 'cameraFwId' must not be empty", !m_cameraID.empty());
}

//-----------------------------------------------------------------------------

void SFrameGrabber::updating() throw ( ::fwTools::Failed )
{
    this->stopCamera();
}

//-----------------------------------------------------------------------------

void SFrameGrabber::startCamera()
{
    this->stopCamera();
    this->setMirror(false, false);

    ::arData::Camera::sptr camera = this->getCamera();

    std::string cameraID = camera->getCameraID();

    if(cameraID == "file")
    {
        /// Path of the video file stored in the camera description
        ::boost::filesystem::path videoPath(camera->getDescription());
        ::boost::filesystem::path videoDir(helper::getVideoDir());

        // For compatibility with old calibration with absolute path
        if (!videoPath.is_absolute())
        {
            videoPath = videoDir / videoPath;
        }

        QString url = QString::fromStdString(videoPath.string());

        m_playlist = new QMediaPlaylist();
        QUrl videoUrl = QUrl::fromLocalFile(url);
        m_playlist->addMedia(videoUrl);

        if(m_loopVideo)
        {
            m_playlist->setPlaybackMode( QMediaPlaylist::CurrentItemInLoop);
        }
        m_playlist->setCurrentIndex(0);

        m_mediaPlayer->setPlaylist( m_playlist );
        m_mediaPlayer->setVideoOutput(m_videoSurface);

        QObject::connect(m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
        QObject::connect(m_mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));

        m_mediaPlayer->play();
    }
    else if(cameraID == "stream")
    {
        /// Path of the video file stored in the camera description
        std::string videoPath = camera->getDescription();

        QString url = QString::fromStdString(videoPath);

        m_playlist = new QMediaPlaylist();
        QUrl videoUrl(url);
        m_playlist->addMedia(videoUrl);

        m_playlist->setCurrentIndex(0);

        m_mediaPlayer->setPlaylist( m_playlist );
        m_mediaPlayer->setVideoOutput(m_videoSurface);

        m_mediaPlayer->play();
    }
    else
    {
        if(!cameraID.empty())
        {
            m_camera = new QCamera(QByteArray(cameraID.c_str(), static_cast<int>(cameraID.size())));
        }
        else
        {
            SLM_WARN("NO camera defined (used default camera)");
            m_camera = new QCamera();
        }

        if(m_camera->error() != QCamera::NoError)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Camera error",
                "Camera not available, please choose another device.",
                ::fwGui::dialog::IMessageDialog::WARNING);
            m_camera->deleteLater();
            m_camera.clear();
            return;
        }
        this->setMirror(false, true);
        m_camera->setViewfinder(m_videoSurface);
        m_camera->setCaptureMode(QCamera::CaptureStillImage);
        m_camera->start();
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::pauseCamera()
{
    if(m_mediaPlayer)
    {
        if(m_mediaPlayer->state() == QMediaPlayer::PausedState)
        {
            m_mediaPlayer->play();
        }
        else
        {
            m_mediaPlayer->pause();
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopCamera()
{
    if(m_camera)
    {
        m_camera->stop();

        delete m_camera;
        m_camera.clear();
    }

    if(m_mediaPlayer)
    {
        QObject::disconnect(m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
        QObject::disconnect(m_mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));

        m_mediaPlayer->stop();
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::selectCamera(const std::string& cameraID)
{
    m_cameraID = cameraID;
    this->startCamera();
}

//-----------------------------------------------------------------------------

SPTR(::arData::Camera) SFrameGrabber::getCamera()
{
    ::fwTools::Object::sptr obj   = ::fwTools::fwID::getObject(m_cameraID);
    ::arData::Camera::sptr camera = ::arData::Camera::dynamicCast(obj);
    FW_RAISE_IF("Camera not found", !camera);
    return camera;
}

//-----------------------------------------------------------------------------

void SFrameGrabber::toogleLoopMode()
{
    m_loopVideo = !m_loopVideo;

    if(m_playlist)
    {
        if(m_loopVideo)
        {
            m_playlist->setPlaybackMode( QMediaPlaylist::CurrentItemInLoop);
        }
        else
        {
            m_playlist->setPlaybackMode( QMediaPlaylist::CurrentItemOnce);
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::setPosition(int position)
{
    if(m_mediaPlayer)
    {
        m_mediaPlayer->setPosition(position);
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::onPositionChanged(qint64 position)
{
    // notify the new position
    ::videoQt::SFrameGrabber::PositionModifiedSignalType::sptr sig;
    sig = this->signal< ::videoQt::SFrameGrabber::PositionModifiedSignalType >
              ( ::videoQt::SFrameGrabber::s_POSITION_MODIFIED_SIG );

    fwServicesNotifyMacro( this->getLightID(), sig, (position));
}

//----------------------------------------------------------------------------

void SFrameGrabber::onDurationChanged(qint64 duration)
{
    // Notify the new duration
    ::videoQt::SFrameGrabber::DurationModifiedSignalType::sptr sig;
    sig = this->signal< ::videoQt::SFrameGrabber::DurationModifiedSignalType >
              ( ::videoQt::SFrameGrabber::s_DURATION_MODIFIED_SIG );

    fwServicesNotifyMacro( SFrameGrabber->getLightID(), sig, (duration));
}

//----------------------------------------------------------------------------

void SFrameGrabber::setVideoFrame(const QVideoFrame& videoFrame)
{
    ::fwCore::mt::WriteLock lock(m_videoFrameMutex);

    // Unmap() in the case the slot present frame has not been called
    m_videoFrame.unmap();

    // Store the video frame (this doesn't make a copy, it just increases a reference counter)
    m_videoFrame = videoFrame;
}

//----------------------------------------------------------------------------

void SFrameGrabber::presentFrame()
{
    // This is called on a different worker than the service, so we must lock the frame
    ::fwCore::mt::ReadLock lock(m_videoFrameMutex);

    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

    ::extData::FrameTL::sptr timeline = this->getObject< ::extData::FrameTL >();

    SPTR(::extData::FrameTL::BufferType) buffer = timeline->createBuffer(timestamp);
    ::boost::uint64_t* destBuffer               = reinterpret_cast< ::boost::uint64_t* >( buffer->addElement(0) );

    SLM_ASSERT("Pixel format must be RGB32", m_videoFrame.pixelFormat() == QVideoFrame::Format_RGB32 ||
               m_videoFrame.pixelFormat() == QVideoFrame::Format_ARGB32_Premultiplied ||
               m_videoFrame.pixelFormat() == QVideoFrame::Format_ARGB32);

    // If we have the same output format, we can take the fast path
    const int width  = m_videoFrame.width();
    const int height = m_videoFrame.height();

#ifdef _WIN32
    QImage imgQt = QImage(m_videoFrame.bits(),
                          width,
                          height,
                          QVideoFrame::imageFormatFromPixelFormat(m_videoFrame.pixelFormat()))
                   .mirrored(m_horizontallyFlip, m_verticallyFlip);

    const ::boost::uint64_t* frameBuffer = reinterpret_cast< const ::boost::uint64_t *>( imgQt.bits() );
#else
    const ::boost::uint64_t* frameBuffer = reinterpret_cast< const ::boost::uint64_t *>( m_videoFrame.bits() );
#endif

    const unsigned int size = static_cast<unsigned int>(width*height) >> 1;

    // Buffer conversion: Qt returns frame in ARGB format, but we need RGBA.
    for(unsigned int idx = 0; idx < size; ++idx)
    {
        const ::boost::uint64_t pixel = *frameBuffer++;

        *destBuffer++ = 0xFF000000FF000000 | (pixel & 0x0000FF000000FF00) |
                        (pixel & 0x000000FF000000FF) << 16 | (pixel & 0x00FF000000FF0000) >> 16;
    }

    // Free the mapped memory
    m_videoFrame.unmap();

    // Release this frame
    m_videoFrame = QVideoFrame();

    // push buffer and notify
    timeline->pushObject(buffer);

    ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = timeline->signal< ::extData::TimeLine::ObjectPushedSignalType >(::extData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

} // namespace videoQt

