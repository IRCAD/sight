/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVideoQt/config.hpp"

#include <QAbstractVideoSurface>
#include <QObject>
#include <QVideoFrame>

class QAbstractVideoSurface;

namespace fwVideoQt
{

class FWVIDEOQT_CLASS_API Surface : public QAbstractVideoSurface
{
Q_OBJECT
public:

    FWVIDEOQT_API Surface(QObject* parent);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;

    FWVIDEOQT_API bool present(const QVideoFrame& frame);

Q_SIGNALS:
    void frameAvailable(const QVideoFrame& frame);
};

} //namespace fwVideoQt
