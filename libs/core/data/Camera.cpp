/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "data/Camera.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

#include <boost/assign.hpp>
#include <boost/bimap/bimap.hpp>

SIGHT_REGISTER_DATA(sight::data::Camera);

namespace sight::data
{

const core::com::Signals::SignalKeyType Camera::s_INTRINSIC_CALIBRATED_SIG = "intrinsicCalibrated";
const core::com::Signals::SignalKeyType Camera::s_ID_MODIFIED_SIG          = "idModified";

//------------------------------------------------------------------------------

Camera::Camera()
{
    m_intrinsic.fill(0.);
    m_distortionCoefficient.fill(0.);

    newSignal<IntrinsicCalibratedSignalType>(s_INTRINSIC_CALIBRATED_SIG);
    newSignal<IdModifiedSignalType>(s_ID_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

Camera::Camera(data::Object::Key /*unused*/)
{
    m_intrinsic.fill(0.);
    m_distortionCoefficient.fill(0.);

    newSignal<IntrinsicCalibratedSignalType>(s_INTRINSIC_CALIBRATED_SIG);
    newSignal<IdModifiedSignalType>(s_ID_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

using PixelFormatTranslatorType = boost::bimaps::bimap<Camera::PixelFormat, std::string>;
PixelFormatTranslatorType pixelFormatTranslator =
    boost::assign::list_of<PixelFormatTranslatorType::relation>
        (Camera::PixelFormat::INVALID, std::string("INVALID"))
        (Camera::PixelFormat::ARGB32, std::string("ARGB32"))
        (Camera::PixelFormat::ARGB32_PREMULTIPLIED, std::string("ARGB32_PREMULTIPLIED"))
        (Camera::PixelFormat::RGB32, std::string("RGB32"))
        (Camera::PixelFormat::RGB24, std::string("RGB24"))
        (Camera::PixelFormat::RGB565, std::string("RGB565"))
        (Camera::PixelFormat::RGB555, std::string("RGB555"))
        (Camera::PixelFormat::ARGB8565_PREMULTIPLIED, std::string("ARGB8565_PREMULTIPLIED"))
        (Camera::PixelFormat::BGRA32, std::string("BGRA32"))
        (Camera::PixelFormat::BGRA32_PREMULTIPLIED, std::string("BGRA32_PREMULTIPLIED"))
        (Camera::PixelFormat::BGR32, std::string("BGR32"))
        (Camera::PixelFormat::BGR24, std::string("BGR24"))
        (Camera::PixelFormat::BGR565, std::string("BGR565"))
        (Camera::PixelFormat::BGR555, std::string("BGR555"))
        (Camera::PixelFormat::BGRA5658_PREMULTIPLIED, std::string("BGRA5658_PREMULTIPLIED"))
        (Camera::PixelFormat::AYUV444, std::string("AYUV444"))
        (Camera::PixelFormat::AYUV444_PREMULTIPLIED, std::string("AYUV444_PREMULTIPLIED"))
        (Camera::PixelFormat::YUV444, std::string("YUV444"))
        (Camera::PixelFormat::YUV420P, std::string("YUV420P"))
        (Camera::PixelFormat::YV12, std::string("YV12"))
        (Camera::PixelFormat::UYVY, std::string("UYVY"))
        (Camera::PixelFormat::YUYV, std::string("YUYV"))
        (Camera::PixelFormat::NV12, std::string("NV12"))
        (Camera::PixelFormat::NV21, std::string("NV21"))
        (Camera::PixelFormat::IMC1, std::string("IMC1"))
        (Camera::PixelFormat::IMC2, std::string("IMC2"))
        (Camera::PixelFormat::IMC3, std::string("IMC3"))
        (Camera::PixelFormat::IMC4, std::string("IMC4"))
        (Camera::PixelFormat::Y8, std::string("Y8"))
        (Camera::PixelFormat::Y16, std::string("Y16"))
        (Camera::PixelFormat::JPEG, std::string("JPEG"))
        (Camera::PixelFormat::CAMERARAW, std::string("CAMERARAW"))
        (Camera::PixelFormat::ADOBEDNG, std::string("ADOBEDNG"))
        (Camera::PixelFormat::RGBA32, std::string("RGBA32"))
        (Camera::PixelFormat::USER, std::string("USER"));

//------------------------------------------------------------------------------

Camera::PixelFormat Camera::getPixelFormat(const std::string& name)
{
    PixelFormat format = PixelFormat::INVALID;

    PixelFormatTranslatorType::right_const_iterator rightIter = pixelFormatTranslator.right.find(name);
    if(rightIter != pixelFormatTranslator.right.end())
    {
        format = rightIter->second;
    }

    return format;
}

//------------------------------------------------------------------------------

std::string Camera::getPixelFormatName(PixelFormat format)
{
    std::string name                                        = "INVALID";
    PixelFormatTranslatorType::left_const_iterator leftIter = pixelFormatTranslator.left.find(format);
    if(leftIter != pixelFormatTranslator.left.end())
    {
        name = leftIter->second;
    }

    return name;
}

//------------------------------------------------------------------------------

void Camera::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
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

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void Camera::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
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

    BaseClass::deepCopy(other, cache);
}

// -------------------------------------------------------------------------

void Camera::setDistortionCoefficient(double k1, double k2, double p1, double p2, double k3)
{
    m_distortionCoefficient[0] = k1;
    m_distortionCoefficient[1] = k2;
    m_distortionCoefficient[2] = p1;
    m_distortionCoefficient[3] = p2;
    m_distortionCoefficient[4] = k3;
}

// -------------------------------------------------------------------------

bool Camera::operator==(const Camera& other) const noexcept
{
    if(m_width != other.m_width
       || m_height != other.m_height
       || !core::tools::is_equal(m_intrinsic, other.m_intrinsic)
       || !core::tools::is_equal(m_distortionCoefficient, other.m_distortionCoefficient)
       || !core::tools::is_equal(m_skew, other.m_skew)
       || m_isCalibrated != other.m_isCalibrated
       || m_cameraID != other.m_cameraID
       || !core::tools::is_equal(m_maxFrameRate, other.m_maxFrameRate)
       || m_pixelFormat != other.m_pixelFormat
       || m_videoFile != other.m_videoFile
       || m_streamUrl != other.m_streamUrl
       || m_cameraSource != other.m_cameraSource
       || !core::tools::is_equal(m_scale, other.m_scale))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Camera::operator!=(const Camera& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
