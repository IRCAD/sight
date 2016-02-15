/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_HELPER_FORMATS_HPP__
#define __VIDEOQT_HELPER_FORMATS_HPP__

#include "arData/Camera.hpp"

#include "videoQt/config.hpp"

#include <boost/bimap/bimap.hpp>
#include <boost/assign.hpp>

#include <QVideoFrame>

namespace videoQt
{
namespace helper
{

//-----------------------------------------------------------------------------

typedef ::boost::bimaps::bimap< QVideoFrame::PixelFormat, ::arData::Camera::PixelFormat > PixelFormatTranslatorType;
extern VIDEOQT_API PixelFormatTranslatorType pixelFormatTranslator;

} // helper
} // videoQt

#endif // __VIDEOQT_HELPER_FORMATS_HPP__
