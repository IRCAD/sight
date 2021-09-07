/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "data/detail/reflection/Camera.hpp"

#include <core/reflection/UserObject.hpp>

//------------------------------------------------------------------------------

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (Camera))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "::sight::data")
    .base<sight::data::Object>()
    .property("intrinsic", &::sight::data::Camera::m_intrinsic)
    .property("is_calibrated", &::sight::data::Camera::m_isCalibrated)
    .property("skew", &::sight::data::Camera::m_skew)
    .property("distortion_coefficient", &::sight::data::Camera::m_distortionCoefficient)
    .property("width", &::sight::data::Camera::m_width)
    .property("height", &::sight::data::Camera::m_height)
    .property("camera_id", &::sight::data::Camera::m_cameraID)
    .property("description", &::sight::data::Camera::m_description)
    .property("max_framerate", &::sight::data::Camera::m_maxFrameRate)
    .property("video_file", &::sight::data::Camera::m_videoFile)
    .property("stream_url", &::sight::data::Camera::m_streamUrl)
    .property("camera_source", &::sight::data::Camera::m_cameraSource)
    .property("pixel_format", &::sight::data::Camera::m_pixelFormat)
    .property("scale", &::sight::data::Camera::m_scale)
    ;
}

SIGHT_IMPLEMENT_ENUM_REFLECTION((sight) (data) (Camera) (SourceType))
{
    builder
    .value("FILE", ::sight::data::Camera::FILE)
    .value("STREAM", ::sight::data::Camera::STREAM)
    .value("DEVICE", ::sight::data::Camera::DEVICE)
    .value("UNKNOWN", ::sight::data::Camera::UNKNOWN)
    ;
}

SIGHT_IMPLEMENT_ENUM_REFLECTION((sight) (data) (Camera) (PixelFormat))
{
    using ::sight::data::Camera;
    builder
    .value(Camera::getPixelFormatName(Camera::INVALID), Camera::INVALID)
    .value(Camera::getPixelFormatName(Camera::ARGB32), Camera::ARGB32)
    .value(Camera::getPixelFormatName(Camera::ARGB32_PREMULTIPLIED), Camera::ARGB32_PREMULTIPLIED)
    .value(Camera::getPixelFormatName(Camera::RGB32), Camera::RGB32)
    .value(Camera::getPixelFormatName(Camera::RGB24), Camera::RGB24)
    .value(Camera::getPixelFormatName(Camera::RGB565), Camera::RGB565)
    .value(Camera::getPixelFormatName(Camera::RGB555), Camera::RGB555)
    .value(Camera::getPixelFormatName(Camera::ARGB8565_PREMULTIPLIED), Camera::ARGB8565_PREMULTIPLIED)
    .value(Camera::getPixelFormatName(Camera::BGRA32), Camera::BGRA32)
    .value(Camera::getPixelFormatName(Camera::BGRA32_PREMULTIPLIED), Camera::BGRA32_PREMULTIPLIED)
    .value(Camera::getPixelFormatName(Camera::BGR32), Camera::BGR32)
    .value(Camera::getPixelFormatName(Camera::BGR24), Camera::BGR24)
    .value(Camera::getPixelFormatName(Camera::BGR565), Camera::BGR565)
    .value(Camera::getPixelFormatName(Camera::BGR555), Camera::BGR555)
    .value(Camera::getPixelFormatName(Camera::BGRA5658_PREMULTIPLIED), Camera::BGRA5658_PREMULTIPLIED)

    .value(Camera::getPixelFormatName(Camera::AYUV444), Camera::AYUV444)
    .value(Camera::getPixelFormatName(Camera::AYUV444_PREMULTIPLIED), Camera::AYUV444_PREMULTIPLIED)
    .value(Camera::getPixelFormatName(Camera::YUV444), Camera::YUV444)
    .value(Camera::getPixelFormatName(Camera::YUV420P), Camera::YUV420P)
    .value(Camera::getPixelFormatName(Camera::YV12), Camera::YV12)
    .value(Camera::getPixelFormatName(Camera::UYVY), Camera::UYVY)
    .value(Camera::getPixelFormatName(Camera::YUYV), Camera::YUYV)
    .value(Camera::getPixelFormatName(Camera::NV12), Camera::NV12)
    .value(Camera::getPixelFormatName(Camera::NV21), Camera::NV21)
    .value(Camera::getPixelFormatName(Camera::IMC1), Camera::IMC1)
    .value(Camera::getPixelFormatName(Camera::IMC2), Camera::IMC2)
    .value(Camera::getPixelFormatName(Camera::IMC3), Camera::IMC3)
    .value(Camera::getPixelFormatName(Camera::IMC4), Camera::IMC4)
    .value(Camera::getPixelFormatName(Camera::Y8), Camera::Y8)
    .value(Camera::getPixelFormatName(Camera::Y16), Camera::Y16)

    .value(Camera::getPixelFormatName(Camera::JPEG), Camera::JPEG)

    .value(Camera::getPixelFormatName(Camera::CAMERARAW), Camera::CAMERARAW)
    .value(Camera::getPixelFormatName(Camera::ADOBEDNG), Camera::ADOBEDNG)

    .value(Camera::getPixelFormatName(Camera::RGBA32), Camera::RGBA32)

    .value(Camera::getPixelFormatName(Camera::USER), Camera::USER)
    ;
}
