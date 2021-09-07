/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "formats.hpp"

#include <boost/assign.hpp>

namespace sight::module::ui::qt
{

namespace video
{

// The format of the code is shitty, that's because of uncrustify who doesn't allow us to write a pair per line.
PixelFormatTranslatorType pixelFormatTranslator = ::boost::assign::list_of<PixelFormatTranslatorType::relation>
                                                  (
    QVideoFrame::PixelFormat::Format_Invalid,
    data::Camera::PixelFormat::INVALID
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_ARGB32,
    data::Camera::PixelFormat::ARGB32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_ARGB32_Premultiplied,
    data::Camera::PixelFormat::ARGB32_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB32,
    data::Camera::PixelFormat::RGB32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB24,
    data::Camera::PixelFormat::RGB24
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB565,
    data::Camera::PixelFormat::RGB565
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB555,
    data::Camera::PixelFormat::RGB555
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_ARGB8565_Premultiplied,
    data::Camera::PixelFormat::ARGB8565_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGRA32,
    data::Camera::PixelFormat::BGRA32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGRA32_Premultiplied,
    data::Camera::PixelFormat::BGRA32_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR32,
    data::Camera::PixelFormat::BGR32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR24,
    data::Camera::PixelFormat::BGR24
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR565,
    data::Camera::PixelFormat::BGR565
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR555,
    data::Camera::PixelFormat::BGR555
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGRA5658_Premultiplied,
    data::Camera::PixelFormat::BGRA5658_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_AYUV444,
    data::Camera::PixelFormat::AYUV444
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_AYUV444_Premultiplied,
    data::Camera::PixelFormat::AYUV444_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YUV444,
    data::Camera::PixelFormat::YUV444
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YUV420P,
    data::Camera::PixelFormat::YUV420P
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YV12,
    data::Camera::PixelFormat::YV12
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_UYVY,
    data::Camera::PixelFormat::UYVY
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YUYV,
    data::Camera::PixelFormat::YUYV
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_NV12,
    data::Camera::PixelFormat::NV12
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_NV21,
    data::Camera::PixelFormat::NV21
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC1,
    data::Camera::PixelFormat::IMC1
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC2,
    data::Camera::PixelFormat::IMC2
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC3,
    data::Camera::PixelFormat::IMC3
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC4,
    data::Camera::PixelFormat::IMC4
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_Y8,
    data::Camera::PixelFormat::Y8
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_Y16,
    data::Camera::PixelFormat::Y16
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_Jpeg,
    data::Camera::PixelFormat::JPEG
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_CameraRaw,
    data::Camera::PixelFormat::CAMERARAW
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_AdobeDng,
    data::Camera::PixelFormat::ADOBEDNG
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_User,
    data::Camera::PixelFormat::USER
                                                  );

} // video

} // sight::module::ui::qt
