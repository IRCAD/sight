/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_PLAYER_QVIDEOSURFACE_HPP__
#define __VIDEOQT_PLAYER_QVIDEOSURFACE_HPP__

#include "videoQt/config.hpp"

#include <QAbstractVideoSurface>
#include <QObject>
#include <QVideoFrame>

class QAbstractVideoSurface;

namespace videoQt
{
namespace player
{

class VIDEOQT_CLASS_API QVideoSurface : public QAbstractVideoSurface
{
Q_OBJECT
public:

    VIDEOQT_API QVideoSurface(QObject *parent);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;

    VIDEOQT_API bool present(const QVideoFrame &frame);

Q_SIGNALS:
    void frameAvailable(const QVideoFrame& frame);
};

} //namespace player
} //namespace videoQt

#endif // __VIDEOQT_PLAYER_QVIDEOSURFACE_HPP__
