/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/SFrameGrabber.hpp"
#include "videoQt/VideoSurfaceQt.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <QVideoFrame>

namespace videoQt
{

//------------------------------------------------------------------------------

VideoSurfaceQt::VideoSurfaceQt(SFrameGrabber* frameGrabber, QObject* parent) :
    QAbstractVideoSurface(parent),
    m_frameGrabber(frameGrabber),
    m_imageFormat(QImage::Format_Invalid)
{
}

//------------------------------------------------------------------------------

QList<QVideoFrame::PixelFormat> VideoSurfaceQt::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType)
const
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

//------------------------------------------------------------------------------

bool VideoSurfaceQt::isFormatSupported(const QVideoSurfaceFormat& format) const
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size                 = format.frameSize();

    return imageFormat != QImage::Format_Invalid
           && !size.isEmpty()
           && format.handleType() == QAbstractVideoBuffer::NoHandle;
}

//-----------------------------------------------------------------------------

bool VideoSurfaceQt::start(const QVideoSurfaceFormat& format)
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize imageSize            = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !imageSize.isEmpty())
    {
        this->m_imageFormat = imageFormat;

        const size_t width  = static_cast<size_t>(imageSize.width());
        const size_t height = static_cast<size_t>(imageSize.height());
        m_timeline->initPoolSize(width, height, ::fwTools::Type::s_UINT8, 4);

        QAbstractVideoSurface::start(format);

        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------

void VideoSurfaceQt::stop()
{
    QAbstractVideoSurface::stop();
}

//-----------------------------------------------------------------------------

bool VideoSurfaceQt::present(const QVideoFrame& frame)
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
            m_frameGrabber->setVideoFrame(frame);
            auto sig = m_frameGrabber->signal< ::videoQt::SFrameGrabber::FramePresentedSignalType >(
                ::videoQt::SFrameGrabber::s_FRAME_PRESENTED_SIG );
            sig->asyncEmit();
        }
        return true;
    }
}

//-----------------------------------------------------------------------------

void VideoSurfaceQt::setTimeline(extData::FrameTL::sptr timeline)
{
    m_timeline = timeline;
}

}
