/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/player/QVideoPlayer.hpp"

#include "videoQt/helper/formats.hpp"
#include "videoQt/player/QVideoSurface.hpp"

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/spyLog.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <boost/filesystem/operations.hpp>

#include <QAbstractVideoSurface>
#include <QCameraViewfinderSettings>
#include <QVideoSurfaceFormat>

namespace videoQt
{
namespace player
{

QVideoPlayer::QVideoPlayer() :
    m_loopVideo(false)
{
}

//------------------------------------------------------------------------------

QVideoPlayer::~QVideoPlayer()
{
    this->stop();
}

//-----------------------------------------------------------------------------

void QVideoPlayer::initCameraFile(const ::boost::filesystem::path& videoPath)
{
    FW_RAISE_IF("Invalid video path '"+videoPath.string()+"'", !::boost::filesystem::exists(videoPath));

    m_mediaPlayer = new QMediaPlayer(0, QMediaPlayer::VideoSurface);

    // Letting the user to know when something bad happens, is generally a good idea
    QObject::connect(m_mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onError(QMediaPlayer::Error)));

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

void QVideoPlayer::initCameraStream(const std::string& strVideoUrl)
{
    m_mediaPlayer = new QMediaPlayer(0, QMediaPlayer::VideoSurface);

    // Letting the user to know when something bad happens, is generally a good idea
    QObject::connect(m_mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onError(QMediaPlayer::Error)));

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

void QVideoPlayer::initCameraDevice(const std::string& cameraID, size_t width, size_t height, float maximumFrameRate,
                                    ::QVideoFrame::PixelFormat pxFormat)
{
    m_camera = new QCamera(QByteArray(cameraID.c_str(), static_cast<int>(cameraID.size())));
    QCameraViewfinderSettings viewfinderSettings;
    viewfinderSettings.setResolution(static_cast<int>(width), static_cast<int>(height));
    viewfinderSettings.setMaximumFrameRate(maximumFrameRate);
    viewfinderSettings.setPixelFormat(pxFormat);
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

void QVideoPlayer::play()
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

//------------------------------------------------------------------------------

void QVideoPlayer::onError(QMediaPlayer::Error error) const
{
    ::fwGui::dialog::MessageDialog::showMessageDialog("QMediaPlayer error", m_mediaPlayer->errorString().toStdString());
}

//-----------------------------------------------------------------------------

void QVideoPlayer::pause()
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

void QVideoPlayer::stop()
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
        m_videoSurface->deleteLater();
    }
}

//-----------------------------------------------------------------------------

void QVideoPlayer::toggleLoopMode(bool isLoopEnable)
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

void QVideoPlayer::setPosition(int64_t position)
{
    if(m_mediaPlayer)
    {
        m_mediaPlayer->setPosition(position);
    }
}

//-----------------------------------------------------------------------------

} //namespace player
} //namespace videoQt
