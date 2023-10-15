/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

namespace sight::module::ui::qt::video
{

// The format of the code is shitty, that's because of uncrustify who doesn't allow us to write a pair per line.
PixelFormatTranslatorType pixelFormatTranslator = boost::assign::list_of<PixelFormatTranslatorType::relation>
                                                  (
    QVideoFrame::PixelFormat::Format_Invalid,
    data::camera::PixelFormat::INVALID
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_ARGB32,
    data::camera::PixelFormat::ARGB32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_ARGB32_Premultiplied,
    data::camera::PixelFormat::ARGB32_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB32,
    data::camera::PixelFormat::RGB32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB24,
    data::camera::PixelFormat::RGB24
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB565,
    data::camera::PixelFormat::RGB565
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_RGB555,
    data::camera::PixelFormat::RGB555
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_ARGB8565_Premultiplied,
    data::camera::PixelFormat::ARGB8565_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGRA32,
    data::camera::PixelFormat::BGRA32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGRA32_Premultiplied,
    data::camera::PixelFormat::BGRA32_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR32,
    data::camera::PixelFormat::BGR32
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR24,
    data::camera::PixelFormat::BGR24
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR565,
    data::camera::PixelFormat::BGR565
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGR555,
    data::camera::PixelFormat::BGR555
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_BGRA5658_Premultiplied,
    data::camera::PixelFormat::BGRA5658_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_AYUV444,
    data::camera::PixelFormat::AYUV444
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_AYUV444_Premultiplied,
    data::camera::PixelFormat::AYUV444_PREMULTIPLIED
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YUV444,
    data::camera::PixelFormat::YUV444
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YUV420P,
    data::camera::PixelFormat::YUV420P
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YV12,
    data::camera::PixelFormat::YV12
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_UYVY,
    data::camera::PixelFormat::UYVY
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_YUYV,
    data::camera::PixelFormat::YUYV
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_NV12,
    data::camera::PixelFormat::NV12
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_NV21,
    data::camera::PixelFormat::NV21
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC1,
    data::camera::PixelFormat::IMC1
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC2,
    data::camera::PixelFormat::IMC2
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC3,
    data::camera::PixelFormat::IMC3
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_IMC4,
    data::camera::PixelFormat::IMC4
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_Y8,
    data::camera::PixelFormat::Y8
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_Y16,
    data::camera::PixelFormat::Y16
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_Jpeg,
    data::camera::PixelFormat::JPEG
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_CameraRaw,
    data::camera::PixelFormat::CAMERARAW
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_AdobeDng,
    data::camera::PixelFormat::ADOBEDNG
                                                  )
                                                  (
    QVideoFrame::PixelFormat::Format_User,
    data::camera::PixelFormat::USER
                                                  );

} // namespace sight::module::ui::qt::video
