/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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
    FWVIDEOQT_API void toggleLoopMode(const bool isLoopEnable);

    /// set the new position in the video.
    FWVIDEOQT_API void setPosition(const int64_t position);

    FWVIDEOQT_API void initCameraFile(const ::boost::filesystem::path& videoPath);

    FWVIDEOQT_API void initCameraStream(const std::string& strVideoUrl);

    FWVIDEOQT_API void initCameraDevice(const std::string& cameraID, size_t width, size_t height,
                                        float maximumFrameRate,
                                        ::QVideoFrame::PixelFormat pxFormat);

    /// Get duration of the current video file being played
    FWVIDEOQT_API int64_t getVideoDuration() const;

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
