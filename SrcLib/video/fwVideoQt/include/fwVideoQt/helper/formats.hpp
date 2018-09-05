/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVideoQt/config.hpp"

#include <arData/Camera.hpp>

#include <boost/bimap/bimap.hpp>

#include <QVideoFrame>

namespace fwVideoQt
{
namespace helper
{

//-----------------------------------------------------------------------------

typedef ::boost::bimaps::bimap< QVideoFrame::PixelFormat, ::arData::Camera::PixelFormat > PixelFormatTranslatorType;
extern FWVIDEOQT_API PixelFormatTranslatorType pixelFormatTranslator;

} // helper
} // fwVideoQt
