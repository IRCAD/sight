/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVideoQt/config.hpp"

#include <boost/filesystem/path.hpp>

#include <QAbstractVideoSurface>
#include <QCamera>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QObject>
#include <QPointer>
#include <QVideoFrame>

namespace fwVideoQt
{

class FWVIDEOQT_CLASS_API Player : public QObject
{
Q_OBJECT
public:

    FWVIDEOQT_API Player();

    FWVIDEOQT_API ~Player();

    /// Stop camera
    FWVIDEOQT_API void stop();

    /// Pause camera
    FWVIDEOQT_API void pause();

    /// Pause camera
    FWVIDEOQT_API void play();

    /// enable/disable loop in video
    FWVIDEOQT_API void toggleLoopMode(bool isLoopEnable);

    /// set the new position in the video.
    FWVIDEOQT_API void setPosition(int64_t position);

    FWVIDEOQT_API void initCameraFile(const ::boost::filesystem::path& videoPath);

    FWVIDEOQT_API void initCameraStream(const std::string& strVideoUrl);

    FWVIDEOQT_API void initCameraDevice(const std::string& cameraID, size_t width, size_t height,
                                        float maximumFrameRate,
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

    void onError(QMediaPlayer::Error) const;
};

} //namespace fwVideoQt
