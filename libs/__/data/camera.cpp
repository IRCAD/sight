/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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
const core::com::signals::key_t camera::ID_MODIFIED_SIG          = "id_modified";

//------------------------------------------------------------------------------

camera::camera()
{
    m_intrinsic.fill(0.);
    m_distortion_coefficient.fill(0.);

    new_signal<intrinsic_calibrated_signal_t>(INTRINSIC_CALIBRATED_SIG);
    new_signal<id_modified_signal_t>(ID_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

using pixel_format_translator_t = boost::bimaps::bimap<enum camera::pixel_format, std::string>;
pixel_format_translator_t pixel_format_translator =
    boost::assign::list_of<pixel_format_translator_t::relation>
        (camera::pixel_format::invalid, std::string("invalid"))
        (camera::pixel_format::argb32, std::string("argb32"))
        (camera::pixel_format::argb32_premultiplied, std::string("argb32_premultiplied"))
        (camera::pixel_format::rgb32, std::string("rgb32"))
        (camera::pixel_format::rgb24, std::string("rgb24"))
        (camera::pixel_format::rgb565, std::string("rgb565"))
        (camera::pixel_format::rgb555, std::string("rgb555"))
        (camera::pixel_format::argb8565_premultiplied, std::string("argb8565_premultiplied"))
        (camera::pixel_format::bgra32, std::string("bgra32"))
        (camera::pixel_format::bgra32_premultiplied, std::string("bgra32_premultiplied"))
        (camera::pixel_format::bgr32, std::string("bgr32"))
        (camera::pixel_format::bgr24, std::string("bgr24"))
        (camera::pixel_format::bgr565, std::string("bgr565"))
        (camera::pixel_format::bgr555, std::string("bgr555"))
        (camera::pixel_format::bgra5658_premultiplied, std::string("bgra5658_premultiplied"))
        (camera::pixel_format::ayuv444, std::string("ayuv444"))
        (camera::pixel_format::ayuv444_premultiplied, std::string("ayuv444_premultiplied"))
        (camera::pixel_format::yuv444, std::string("yuv444"))
        (camera::pixel_format::yuv420_p, std::string("yuv420_p"))
        (camera::pixel_format::yv12, std::string("yv12"))
        (camera::pixel_format::uyvy, std::string("uyvy"))
        (camera::pixel_format::yuyv, std::string("yuyv"))
        (camera::pixel_format::nv12, std::string("nv12"))
        (camera::pixel_format::nv21, std::string("nv21"))
        (camera::pixel_format::imc1, std::string("imc1"))
        (camera::pixel_format::imc2, std::string("imc2"))
        (camera::pixel_format::imc3, std::string("imc3"))
        (camera::pixel_format::imc4, std::string("imc4"))
        (camera::pixel_format::y8, std::string("y8"))
        (camera::pixel_format::y16, std::string("y16"))
        (camera::pixel_format::jpeg, std::string("jpeg"))
        (camera::pixel_format::cameraraw, std::string("cameraraw"))
        (camera::pixel_format::adobedng, std::string("adobedng"))
        (camera::pixel_format::rgba32, std::string("rgba32"))
        (camera::pixel_format::user, std::string("user"));

//------------------------------------------------------------------------------

enum camera::pixel_format camera::pixel_format(const std::string& _name)
{
    enum pixel_format format = pixel_format::invalid;

    pixel_format_translator_t::right_const_iterator right_iter = pixel_format_translator.right.find(_name);
    if(right_iter != pixel_format_translator.right.end())
    {
        format = right_iter->second;
    }

    return format;
}

//------------------------------------------------------------------------------

std::string camera::get_pixel_format_name(enum pixel_format _format)
{
    std::string name                                         = "invalid";
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

    m_width                  = other->m_width;
    m_height                 = other->m_height;
    m_intrinsic              = other->m_intrinsic;
    m_distortion_coefficient = other->m_distortion_coefficient;
    m_skew                   = other->m_skew;
    m_is_calibrated          = other->m_is_calibrated;
    m_camera_id              = other->m_camera_id;
    m_max_frame_rate         = other->m_max_frame_rate;
    m_pixel_format           = other->m_pixel_format;
    m_video_file             = other->m_video_file;
    m_stream_url             = other->m_stream_url;
    m_camera_source          = other->m_camera_source;
    m_scale                  = other->m_scale;
    m_calibration_error      = other->m_calibration_error;

    base_class_t::shallow_copy(other);
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

    m_width                  = other->m_width;
    m_height                 = other->m_height;
    m_intrinsic              = other->m_intrinsic;
    m_distortion_coefficient = other->m_distortion_coefficient;
    m_skew                   = other->m_skew;
    m_is_calibrated          = other->m_is_calibrated;
    m_camera_id              = other->m_camera_id;
    m_max_frame_rate         = other->m_max_frame_rate;
    m_pixel_format           = other->m_pixel_format;
    m_video_file             = other->m_video_file;
    m_stream_url             = other->m_stream_url;
    m_camera_source          = other->m_camera_source;
    m_scale                  = other->m_scale;
    m_calibration_error      = other->m_calibration_error;

    base_class_t::deep_copy(other, _cache);
}

// -------------------------------------------------------------------------

void camera::set_distortion_coefficient(double _k1, double _k2, double _p1, double _p2, double _k3)
{
    m_distortion_coefficient[0] = _k1;
    m_distortion_coefficient[1] = _k2;
    m_distortion_coefficient[2] = _p1;
    m_distortion_coefficient[3] = _p2;
    m_distortion_coefficient[4] = _k3;
}

// -------------------------------------------------------------------------

bool camera::operator==(const camera& _other) const noexcept
{
    if(m_width != _other.m_width
       || m_height != _other.m_height
       || !core::is_equal(m_intrinsic, _other.m_intrinsic)
       || !core::is_equal(m_distortion_coefficient, _other.m_distortion_coefficient)
       || !core::is_equal(m_skew, _other.m_skew)
       || m_is_calibrated != _other.m_is_calibrated
       || m_camera_id != _other.m_camera_id
       || !core::is_equal(m_max_frame_rate, _other.m_max_frame_rate)
       || m_pixel_format != _other.m_pixel_format
       || m_video_file != _other.m_video_file
       || m_stream_url != _other.m_stream_url
       || m_camera_source != _other.m_camera_source
       || !core::is_equal(m_scale, _other.m_scale)
       || !core::is_equal(m_calibration_error, _other.m_calibration_error))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool camera::operator!=(const camera& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
