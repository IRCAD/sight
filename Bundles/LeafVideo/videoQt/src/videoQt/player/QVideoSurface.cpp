/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/player/QVideoSurface.hpp"

#include <fwCore/spyLog.hpp>
#include <fwCore/exceptionmacros.hpp>

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

namespace videoQt
{
namespace player
{

//-----------------------------------------------------------------------------

QVideoSurface::QVideoSurface(QObject *parent) : QAbstractVideoSurface(parent)
{
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

} //namespace player
} //namespace videoQt
