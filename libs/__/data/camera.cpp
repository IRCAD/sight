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

#include "data/camera.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

#include <boost/assign.hpp>
#include <boost/bimap/bimap.hpp>

SIGHT_REGISTER_DATA(sight::data::camera);

namespace sight::data
{

const core::com::signals::key_t camera::INTRINSIC_CALIBRATED_SIG = "intrinsicCalibrated";
const core::com::signals::key_t camera::ID_MODIFIED_SIG          = "idModified";

//------------------------------------------------------------------------------

camera::camera()
{
    m_intrinsic.fill(0.);
    m_distortionCoefficient.fill(0.);

    new_signal<intrinsic_calibrated_signal_t>(INTRINSIC_CALIBRATED_SIG);
    new_signal<id_modified_signal_t>(ID_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

using pixel_format_translator_t = boost::bimaps::bimap<camera::PixelFormat, std::string>;
pixel_format_translator_t pixel_format_translator =
    boost::assign::list_of<pixel_format_translator_t::relation>
        (camera::PixelFormat::INVALID, std::string("INVALID"))
        (camera::PixelFormat::ARGB32, std::string("ARGB32"))
        (camera::PixelFormat::ARGB32_PREMULTIPLIED, std::string("ARGB32_PREMULTIPLIED"))
        (camera::PixelFormat::RGB32, std::string("RGB32"))
        (camera::PixelFormat::RGB24, std::string("RGB24"))
        (camera::PixelFormat::RGB565, std::string("RGB565"))
        (camera::PixelFormat::RGB555, std::string("RGB555"))
        (camera::PixelFormat::ARGB8565_PREMULTIPLIED, std::string("ARGB8565_PREMULTIPLIED"))
        (camera::PixelFormat::BGRA32, std::string("BGRA32"))
        (camera::PixelFormat::BGRA32_PREMULTIPLIED, std::string("BGRA32_PREMULTIPLIED"))
        (camera::PixelFormat::BGR32, std::string("BGR32"))
        (camera::PixelFormat::BGR24, std::string("BGR24"))
        (camera::PixelFormat::BGR565, std::string("BGR565"))
        (camera::PixelFormat::BGR555, std::string("BGR555"))
        (camera::PixelFormat::BGRA5658_PREMULTIPLIED, std::string("BGRA5658_PREMULTIPLIED"))
        (camera::PixelFormat::AYUV444, std::string("AYUV444"))
        (camera::PixelFormat::AYUV444_PREMULTIPLIED, std::string("AYUV444_PREMULTIPLIED"))
        (camera::PixelFormat::YUV444, std::string("YUV444"))
        (camera::PixelFormat::YUV420P, std::string("YUV420P"))
        (camera::PixelFormat::YV12, std::string("YV12"))
        (camera::PixelFormat::UYVY, std::string("UYVY"))
        (camera::PixelFormat::YUYV, std::string("YUYV"))
        (camera::PixelFormat::NV12, std::string("NV12"))
        (camera::PixelFormat::NV21, std::string("NV21"))
        (camera::PixelFormat::IMC1, std::string("IMC1"))
        (camera::PixelFormat::IMC2, std::string("IMC2"))
        (camera::PixelFormat::IMC3, std::string("IMC3"))
        (camera::PixelFormat::IMC4, std::string("IMC4"))
        (camera::PixelFormat::Y8, std::string("Y8"))
        (camera::PixelFormat::Y16, std::string("Y16"))
        (camera::PixelFormat::JPEG, std::string("JPEG"))
        (camera::PixelFormat::CAMERARAW, std::string("CAMERARAW"))
        (camera::PixelFormat::ADOBEDNG, std::string("ADOBEDNG"))
        (camera::PixelFormat::RGBA32, std::string("RGBA32"))
        (camera::PixelFormat::USER, std::string("USER"));

//------------------------------------------------------------------------------

camera::PixelFormat camera::getPixelFormat(const std::string& _name)
{
    PixelFormat format = PixelFormat::INVALID;

    pixel_format_translator_t::right_const_iterator right_iter = pixel_format_translator.right.find(_name);
    if(right_iter != pixel_format_translator.right.end())
    {
        format = right_iter->second;
    }

    return format;
}

//------------------------------------------------------------------------------

std::string camera::getPixelFormatName(PixelFormat _format)
{
    std::string name                                         = "INVALID";
    pixel_format_translator_t::left_const_iterator left_iter = pixel_format_translator.left.find(_format);
    if(left_iter != pixel_format_translator.left.end())
    {
        name = left_iter->second;
    }

    return name;
}

//------------------------------------------------------------------------------

void camera::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const camera>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_width                 = other->m_width;
    m_height                = other->m_height;
    m_intrinsic             = other->m_intrinsic;
    m_distortionCoefficient = other->m_distortionCoefficient;
    m_skew                  = other->m_skew;
    m_isCalibrated          = other->m_isCalibrated;
    m_cameraID              = other->m_cameraID;
    m_maxFrameRate          = other->m_maxFrameRate;
    m_pixelFormat           = other->m_pixelFormat;
    m_videoFile             = other->m_videoFile;
    m_streamUrl             = other->m_streamUrl;
    m_cameraSource          = other->m_cameraSource;
    m_scale                 = other->m_scale;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void camera::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const camera>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_width                 = other->m_width;
    m_height                = other->m_height;
    m_intrinsic             = other->m_intrinsic;
    m_distortionCoefficient = other->m_distortionCoefficient;
    m_skew                  = other->m_skew;
    m_isCalibrated          = other->m_isCalibrated;
    m_cameraID              = other->m_cameraID;
    m_maxFrameRate          = other->m_maxFrameRate;
    m_pixelFormat           = other->m_pixelFormat;
    m_videoFile             = other->m_videoFile;
    m_streamUrl             = other->m_streamUrl;
    m_cameraSource          = other->m_cameraSource;
    m_scale                 = other->m_scale;

    base_class::deep_copy(other, _cache);
}

// -------------------------------------------------------------------------

void camera::setDistortionCoefficient(double _k1, double _k2, double _p1, double _p2, double _k3)
{
    m_distortionCoefficient[0] = _k1;
    m_distortionCoefficient[1] = _k2;
    m_distortionCoefficient[2] = _p1;
    m_distortionCoefficient[3] = _p2;
    m_distortionCoefficient[4] = _k3;
}

// -------------------------------------------------------------------------

bool camera::operator==(const camera& _other) const noexcept
{
    if(m_width != _other.m_width
       || m_height != _other.m_height
       || !core::tools::is_equal(m_intrinsic, _other.m_intrinsic)
       || !core::tools::is_equal(m_distortionCoefficient, _other.m_distortionCoefficient)
       || !core::tools::is_equal(m_skew, _other.m_skew)
       || m_isCalibrated != _other.m_isCalibrated
       || m_cameraID != _other.m_cameraID
       || !core::tools::is_equal(m_maxFrameRate, _other.m_maxFrameRate)
       || m_pixelFormat != _other.m_pixelFormat
       || m_videoFile != _other.m_videoFile
       || m_streamUrl != _other.m_streamUrl
       || m_cameraSource != _other.m_cameraSource
       || !core::tools::is_equal(m_scale, _other.m_scale))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool camera::operator!=(const camera& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
