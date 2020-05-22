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

#include "fwVideoQt/Surface.hpp"

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/spyLog.hpp>

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

namespace fwVideoQt
{

//-----------------------------------------------------------------------------

Surface::Surface(QObject* parent) :
    QAbstractVideoSurface(parent)
{
}

//-----------------------------------------------------------------------------

QList<QVideoFrame::PixelFormat> Surface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
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

bool Surface::present(const QVideoFrame& frame)
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

} //namespace fwVideoQt
