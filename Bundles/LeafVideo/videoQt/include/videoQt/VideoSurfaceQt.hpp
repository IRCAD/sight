/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_VIDEOSURFACEQT_HPP__
#define __VIDEOQT_VIDEOSURFACEQT_HPP__

#include <fwCore/macros.hpp>

#include <extData/FrameTL.hpp>

#include "videoQt/config.hpp"

#include <QObject>
#include <QPointer>
#include <QVideoSurfaceFormat>
#include <QAbstractVideoSurface>

namespace videoQt
{
class SFrameGrabber;

//-----------------------------------------------------------------------------

/// Qt VideoSurface used by SFrameGrabber
class VideoSurfaceQt : public QAbstractVideoSurface
{
Q_OBJECT
public:

    /// Constructor
    VIDEOQT_API VideoSurfaceQt(SFrameGrabber* frameGrabber, QObject *parent = 0);

    /// Destructor
    VIDEOQT_API QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    /// Return true if the given format is supported by the surface
    VIDEOQT_API bool isFormatSupported(const QVideoSurfaceFormat &format) const;

    /// Called when the media player starts the video playback
    VIDEOQT_API bool start(const QVideoSurfaceFormat &format);

    /// Called when the media player stops the video playback
    VIDEOQT_API void stop();

    /// Called when a frame is decoded and ready for display
    VIDEOQT_API bool present(const QVideoFrame &frame);

    /// Set the frame timeline
    VIDEOQT_API void setTimeline(::extData::FrameTL::sptr timeline);

private:

    /// Pointer on the frame grabber (needed to trigger a signal)
    SFrameGrabber* m_frameGrabber;
    /// Format of the surface
    QImage::Format m_imageFormat;
    /// Size of the surface
    QSize m_imageSize;
    /// Pointer on the timeline
    ::extData::FrameTL::sptr m_timeline;
};

}

#endif // __VIDEOQT_VIDEOSURFACEQT_HPP__
