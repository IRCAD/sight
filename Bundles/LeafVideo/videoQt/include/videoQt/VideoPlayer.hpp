/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_VIDEOPLAYER_HPP__
#define __VIDEOQT_VIDEOPLAYER_HPP__

#include "videoQt/config.hpp"

#include <QAbstractVideoSurface>
#include <QCamera>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QObject>
#include <QPointer>
#include <QVideoFrame>
#include <QVideoProbe>

#include <boost/filesystem/path.hpp>

class QAbstractVideoSurface;

namespace videoQt
{

class QVideoSurface : public QAbstractVideoSurface
{
Q_OBJECT
public:

    QVideoSurface(QObject *parent);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;

    bool present(const QVideoFrame &frame);

Q_SIGNALS:
    void frameAvailable(const QVideoFrame& frame);

};

//-----------------------------------------------------------------------------

class VideoPlayer : public QObject
{
Q_OBJECT

public:

    VideoPlayer();

    ~VideoPlayer();

    /// Stop camera
    void stop();

    /// Pause camera
    void pause();

    /// Pause camera
    void play();

    /// enable/disable loop in video
    void toggleLoopMode(bool isLoopEnable);

    /// set the new position in the video.
    void setPosition(int64_t position);

    void initCameraFile(const ::boost::filesystem::path& videoPath);

    void initCameraStream(const std::string& strVideoUrl);

    void initCameraDevice(const std::string& cameraID, int width, int height, double maximumFrameRate);

Q_SIGNALS:

    /// Emitted when duration of the video changed.
    void durationChanged(qint64 duration);

    /// Emitted when reading position changed in the video.
    void positionChanged(qint64 position);

    /// Emitted when video frame is processed in the media service
    void frameAvailable(const QVideoFrame& frame);

private:

    /// Qt video surface
    QPointer<QAbstractVideoSurface> m_videoSurface;

    /// Media player
    QPointer<QMediaPlayer> m_mediaPlayer;

    /// Play list
    QPointer<QMediaPlaylist> m_playlist;

    /// Camera
    QPointer<QCamera> m_camera;

    /// state of the loop mode
    bool m_loopVideo;
};

}

#endif // __VIDEOQT_VIDEOPLAYER_HPP__
