/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arDataCamp/Camera.hpp"

#include <fwCamp/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((arData)(Camera))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "arData")
    .base< ::fwData::Object>()
    .property("intrinsic", &::arData::Camera::m_intrinsic)
    .property("is_calibrated", &::arData::Camera::m_isCalibrated)
    .property("skew", &::arData::Camera::m_skew)
    .property("distortion_coefficient", &::arData::Camera::m_distortionCoefficient)
    .property("width", &::arData::Camera::m_width)
    .property("height", &::arData::Camera::m_height)
    .property("camera_id", &::arData::Camera::m_cameraID)
    .property("description", &::arData::Camera::m_description)
    .property("max_framerate", &::arData::Camera::m_maxFrameRate)
    .property("video_file", &::arData::Camera::m_videoFile)
    .property("stream_url", &::arData::Camera::m_streamUrl)
    .property("camera_source", &::arData::Camera::m_cameraSource)
    .property("pixel_format", &::arData::Camera::m_pixelFormat)
    .property("scale", &::arData::Camera::m_scale)
    ;
}

fwCampImplementEnumMacro((arData)(Camera)(SourceType))
{
    builder
    .value("FILE", ::arData::Camera::FILE)
    .value("STREAM", ::arData::Camera::STREAM)
    .value("DEVICE", ::arData::Camera::DEVICE)
    .value("UNKNOWN", ::arData::Camera::UNKNOWN)
    ;
}

fwCampImplementEnumMacro((arData)(Camera)(PixelFormat))
{
    using ::arData::Camera;
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
