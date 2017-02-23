/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_PLAYER_QVIDEOPLAYER_HPP__
#define __VIDEOQT_PLAYER_QVIDEOPLAYER_HPP__

#include "videoQt/config.hpp"

#include <boost/filesystem/path.hpp>

#include <QAbstractVideoSurface>
#include <QCamera>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QObject>
#include <QPointer>
#include <QVideoFrame>

namespace videoQt
{
namespace player
{

class VIDEOQT_CLASS_API QVideoPlayer : public QObject
{
Q_OBJECT
public:

    VIDEOQT_API QVideoPlayer();

    VIDEOQT_API ~QVideoPlayer();

    /// Stop camera
    VIDEOQT_API void stop();

    /// Pause camera
    VIDEOQT_API void pause();

    /// Pause camera
    VIDEOQT_API void play();

    /// enable/disable loop in video
    VIDEOQT_API void toggleLoopMode(bool isLoopEnable);

    /// set the new position in the video.
    VIDEOQT_API void setPosition(int64_t position);

    VIDEOQT_API void initCameraFile(const ::boost::filesystem::path& videoPath);

    VIDEOQT_API void initCameraStream(const std::string& strVideoUrl);

    VIDEOQT_API void initCameraDevice(const std::string& cameraID, size_t width, size_t height, float maximumFrameRate,
                                      ::QVideoFrame::PixelFormat pxFormat);

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

private Q_SLOTS:

    void onError(QMediaPlayer::Error);
};

} //namespace player
} //namespace videoQt

#endif // __VIDEOQT_PLAYER_QVIDEOPLAYER_HPP__
