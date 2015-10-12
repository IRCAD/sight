/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/VideoPlayer.hpp"

#include <fwCore/spyLog.hpp>
#include <fwCore/exceptionmacros.hpp>

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QCameraViewfinderSettings>

#include <boost/filesystem/operations.hpp>

namespace videoQt
{


QVideoSurface::QVideoSurface(QObject *parent) : QAbstractVideoSurface(parent)
{
}

QList<QVideoFrame::PixelFormat> QVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle)
    {
        return QList<QVideoFrame::PixelFormat>()
               << QVideoFrame::Format_RGB32
               << QVideoFrame::Format_ARGB32
               << QVideoFrame::Format_ARGB32_Premultiplied;
    }
    else
    {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool QVideoSurface::present(const QVideoFrame& frame)
{
    if (surfaceFormat().pixelFormat() != frame.pixelFormat()
        || surfaceFormat().frameSize() != frame.size())
    {
        SLM_WARN( "IncorrectFormatError" );
        return false;
    }
    else
    {
        QVideoFrame currentFrame = frame;
        if (currentFrame.map(QAbstractVideoBuffer::ReadOnly))
        {
            Q_EMIT frameAvailable(currentFrame);
        }
        return true;
    }
}

//------------------------------------------------------------------------------

VideoPlayer::VideoPlayer() : m_loopVideo(false)
{
}

//------------------------------------------------------------------------------

VideoPlayer::~VideoPlayer()
{
    this->stop();
}

//-----------------------------------------------------------------------------

void VideoPlayer::initCameraFile(const ::boost::filesystem::path& videoPath)
{
    SLM_ASSERT("Invalid video path '"+videoPath.string()+"'", ::boost::filesystem::exists(videoPath));

    m_mediaPlayer = new QMediaPlayer(0, QMediaPlayer::VideoSurface);

    QString url = QString::fromStdString(videoPath.string());
    m_playlist = new QMediaPlaylist();
    QUrl videoUrl = QUrl::fromLocalFile(url);
    m_playlist->addMedia(videoUrl);

    if(m_loopVideo)
    {
        m_playlist->setPlaybackMode( QMediaPlaylist::CurrentItemInLoop);
    }
    m_playlist->setCurrentIndex(0);

    m_mediaPlayer->setPlaylist(m_playlist);
    m_videoSurface = new QVideoSurface(this);
    m_mediaPlayer->setVideoOutput(m_videoSurface);

    QObject::connect(m_videoSurface, SIGNAL(frameAvailable(QVideoFrame)), this, SIGNAL(frameAvailable(QVideoFrame)));
    QObject::connect(m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SIGNAL(positionChanged(qint64)));
    QObject::connect(m_mediaPlayer, SIGNAL(durationChanged(qint64)), this, SIGNAL(durationChanged(qint64)));
}

//-----------------------------------------------------------------------------

void VideoPlayer::initCameraStream(const std::string& strVideoUrl)
{
    m_mediaPlayer = new QMediaPlayer(0, QMediaPlayer::VideoSurface);

    QString url = QString::fromStdString(strVideoUrl);

    m_playlist = new QMediaPlaylist();
    QUrl videoUrl(url);
    m_playlist->addMedia(videoUrl);

    m_playlist->setCurrentIndex(0);

    m_mediaPlayer->setPlaylist( m_playlist );
    m_videoSurface = new QVideoSurface(this);
    m_mediaPlayer->setVideoOutput(m_videoSurface);

    QObject::connect(m_videoSurface, SIGNAL(frameAvailable(QVideoFrame)), this, SIGNAL(frameAvailable(QVideoFrame)));
}

//-----------------------------------------------------------------------------

void VideoPlayer::initCameraDevice(const std::string& cameraID, int width, int height, double maximumFrameRate)
{
    m_camera = new QCamera(QByteArray(cameraID.c_str(), static_cast<int>(cameraID.size())));
    QCameraViewfinderSettings viewfinderSettings;
    viewfinderSettings.setResolution(width, height);
    viewfinderSettings.setMaximumFrameRate(maximumFrameRate);
    m_camera->setViewfinderSettings(viewfinderSettings);

    if(m_camera->error() != QCamera::NoError)
    {
        m_camera->deleteLater();
        m_camera.clear();
        FW_RAISE("Camera not available, please choose another device.");
    }
    m_videoSurface = new QVideoSurface(this);
    m_camera->setViewfinder(m_videoSurface);
    m_camera->setCaptureMode(QCamera::CaptureVideo);

    QObject::connect(m_videoSurface, SIGNAL(frameAvailable(QVideoFrame)), this, SIGNAL(frameAvailable(QVideoFrame)));
}

//-----------------------------------------------------------------------------

void VideoPlayer::play()
{
    if(m_mediaPlayer)
    {
        m_mediaPlayer->play();
    }
    if(m_camera)
    {
        m_camera->start();
    }
}

//-----------------------------------------------------------------------------

void VideoPlayer::pause()
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

void VideoPlayer::stop()
{
    if(m_camera)
    {
        m_camera->stop();

        delete m_camera;
        m_camera.clear();
    }

    if(m_mediaPlayer)
    {
        QObject::disconnect(m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SIGNAL(positionChanged(qint64)));
        QObject::disconnect(m_mediaPlayer, SIGNAL(durationChanged(qint64)), this, SIGNAL(durationChanged(qint64)));

        m_mediaPlayer->stop();

        delete m_mediaPlayer;
        m_mediaPlayer.clear();
    }

    if(m_playlist)
    {
        m_playlist->clear();
    }

    if(m_videoSurface)
    {
        m_videoSurface->stop();
        QObject::disconnect(m_videoSurface, SIGNAL(frameAvailable(QVideoFrame)), this,
                            SIGNAL(frameAvailable(QVideoFrame)));
        delete m_videoSurface;
        m_videoSurface.clear();
    }
}

//-----------------------------------------------------------------------------

void VideoPlayer::toggleLoopMode(bool isLoopEnable)
{
    if(m_playlist)
    {
        if(isLoopEnable)
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

void VideoPlayer::setPosition(int64_t position)
{
    if(m_mediaPlayer)
    {
        m_mediaPlayer->setPosition(position);
    }
}

//-----------------------------------------------------------------------------

} //namespace videoQt
