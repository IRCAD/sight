/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

// cspell:ignore XRGB BGRX

namespace sight::module::ui::qt::video
{

// The format of the code is shitty, that's because of uncrustify who doesn't allow us to write a pair per line.
pixel_format_translator_t pixel_format_translator =
    boost::assign::list_of<pixel_format_translator_t::relation>
    (
        QVideoFrameFormat::PixelFormat::Format_Invalid,
        data::camera::pixel_format_t::invalid
    )
    (
        QVideoFrameFormat::PixelFormat::Format_ARGB8888,
        data::camera::pixel_format_t::argb32
    )
    (
        QVideoFrameFormat::PixelFormat::Format_ARGB8888_Premultiplied,
        data::camera::pixel_format_t::argb32_premultiplied
    )
    (
        QVideoFrameFormat::PixelFormat::Format_XRGB8888,
        data::camera::pixel_format_t::rgb32
    )
    (
        QVideoFrameFormat::PixelFormat::Format_BGRA8888,
        data::camera::pixel_format_t::bgra32
    )
    (
        QVideoFrameFormat::PixelFormat::Format_BGRA8888_Premultiplied,
        data::camera::pixel_format_t::bgra32_premultiplied
    )
    (
        QVideoFrameFormat::PixelFormat::Format_BGRX8888,
        data::camera::pixel_format_t::bgr32
    )
    (
        QVideoFrameFormat::PixelFormat::Format_AYUV,
        data::camera::pixel_format_t::ayuv444
    )
    (
        QVideoFrameFormat::PixelFormat::Format_AYUV_Premultiplied,
        data::camera::pixel_format_t::ayuv444_premultiplied
    )
    (
        QVideoFrameFormat::PixelFormat::Format_YUYV,
        data::camera::pixel_format_t::yuv444
    )
    (
        QVideoFrameFormat::PixelFormat::Format_YUV420P,
        data::camera::pixel_format_t::yuv420_p
    )
    (
        QVideoFrameFormat::PixelFormat::Format_YV12,
        data::camera::pixel_format_t::yv12
    )
    (
        QVideoFrameFormat::PixelFormat::Format_UYVY,
        data::camera::pixel_format_t::uyvy
    )
    (
        QVideoFrameFormat::PixelFormat::Format_YUYV,
        data::camera::pixel_format_t::yuyv
    )
    (
        QVideoFrameFormat::PixelFormat::Format_NV12,
        data::camera::pixel_format_t::nv12
    )
    (
        QVideoFrameFormat::PixelFormat::Format_NV21,
        data::camera::pixel_format_t::nv21
    )
    (
        QVideoFrameFormat::PixelFormat::Format_IMC1,
        data::camera::pixel_format_t::imc1
    )
    (
        QVideoFrameFormat::PixelFormat::Format_IMC2,
        data::camera::pixel_format_t::imc2
    )
    (
        QVideoFrameFormat::PixelFormat::Format_IMC3,
        data::camera::pixel_format_t::imc3
    )
    (
        QVideoFrameFormat::PixelFormat::Format_IMC4,
        data::camera::pixel_format_t::imc4
    )
    (
        QVideoFrameFormat::PixelFormat::Format_Y8,
        data::camera::pixel_format_t::y8
    )
    (
        QVideoFrameFormat::PixelFormat::Format_Y16,
        data::camera::pixel_format_t::y16
    )
    (
        QVideoFrameFormat::PixelFormat::Format_Jpeg,
        data::camera::pixel_format_t::jpeg
    );

} // namespace sight::module::ui::qt::video
