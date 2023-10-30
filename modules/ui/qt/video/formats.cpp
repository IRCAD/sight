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
pixel_format_translator_t pixel_format_translator = boost::assign::list_of<pixel_format_translator_t::relation>
                                                    (
    QVideoFrame::PixelFormat::Format_Invalid,
    data::camera::pixel_format::invalid
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_ARGB32,
    data::camera::pixel_format::argb32
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_ARGB32_Premultiplied,
    data::camera::pixel_format::argb32_premultiplied
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_RGB32,
    data::camera::pixel_format::rgb32
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_RGB24,
    data::camera::pixel_format::rgb24
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_RGB565,
    data::camera::pixel_format::rgb565
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_RGB555,
    data::camera::pixel_format::rgb555
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_ARGB8565_Premultiplied,
    data::camera::pixel_format::argb8565_premultiplied
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_BGRA32,
    data::camera::pixel_format::bgra32
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_BGRA32_Premultiplied,
    data::camera::pixel_format::bgra32_premultiplied
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_BGR32,
    data::camera::pixel_format::bgr32
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_BGR24,
    data::camera::pixel_format::bgr24
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_BGR565,
    data::camera::pixel_format::bgr565
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_BGR555,
    data::camera::pixel_format::bgr555
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_BGRA5658_Premultiplied,
    data::camera::pixel_format::bgra5658_premultiplied
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_AYUV444,
    data::camera::pixel_format::ayuv444
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_AYUV444_Premultiplied,
    data::camera::pixel_format::ayuv444_premultiplied
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_YUV444,
    data::camera::pixel_format::yuv444
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_YUV420P,
    data::camera::pixel_format::yuv420_p
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_YV12,
    data::camera::pixel_format::yv12
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_UYVY,
    data::camera::pixel_format::uyvy
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_YUYV,
    data::camera::pixel_format::yuyv
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_NV12,
    data::camera::pixel_format::nv12
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_NV21,
    data::camera::pixel_format::nv21
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_IMC1,
    data::camera::pixel_format::imc1
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_IMC2,
    data::camera::pixel_format::imc2
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_IMC3,
    data::camera::pixel_format::imc3
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_IMC4,
    data::camera::pixel_format::imc4
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_Y8,
    data::camera::pixel_format::y8
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_Y16,
    data::camera::pixel_format::y16
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_Jpeg,
    data::camera::pixel_format::jpeg
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_CameraRaw,
    data::camera::pixel_format::cameraraw
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_AdobeDng,
    data::camera::pixel_format::adobedng
                                                    )
                                                    (
    QVideoFrame::PixelFormat::Format_User,
    data::camera::pixel_format::user
                                                    );

} // namespace sight::module::ui::qt::video
