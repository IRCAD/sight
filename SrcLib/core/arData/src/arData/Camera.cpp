/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/Camera.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

#include <boost/assign.hpp>
#include <boost/bimap/bimap.hpp>

fwDataRegisterMacro(::arData::Camera);

namespace arData
{

const ::fwCom::Signals::SignalKeyType Camera::s_INTRINSIC_CALIBRATED_SIG = "intrinsicCalibrated";
const ::fwCom::Signals::SignalKeyType Camera::s_ID_MODIFIED_SIG          = "idModified";

//------------------------------------------------------------------------------

Camera::Camera( ::fwData::Object::Key key ) :
    m_width(0),
    m_height(0),
    m_skew(0.),
    m_isCalibrated(false),
    m_maxFrameRate(30.f),
    m_pixelFormat(INVALID),
    m_cameraSource(UNKNOWN),
    m_scale(1.)
{
    m_intrinsic.fill(0.);
    m_distortionCoefficient.fill(0.);

    newSignal< IntrinsicCalibratedSignalType >(s_INTRINSIC_CALIBRATED_SIG);
    newSignal< IdModifiedSignalType >(s_ID_MODIFIED_SIG);
}

// -------------------------------------------------------------------------

Camera::~Camera() throw()
{
}

//------------------------------------------------------------------------------

typedef ::boost::bimaps::bimap< Camera::PixelFormat, std::string > PixelFormatTranslatorType;
PixelFormatTranslatorType pixelFormatTranslator =
    ::boost::assign::list_of< PixelFormatTranslatorType::relation >
        (Camera::PixelFormat::INVALID,                std::string("INVALID"))
        (Camera::PixelFormat::ARGB32,                 std::string("ARGB32"))
        (Camera::PixelFormat::ARGB32_PREMULTIPLIED,   std::string("ARGB32_PREMULTIPLIED"))
        (Camera::PixelFormat::RGB32,                  std::string("RGB32"))
        (Camera::PixelFormat::RGB24,                  std::string("RGB24"))
        (Camera::PixelFormat::RGB565,                 std::string("RGB565"))
        (Camera::PixelFormat::RGB555,                 std::string("RGB555"))
        (Camera::PixelFormat::ARGB8565_PREMULTIPLIED, std::string("ARGB8565_PREMULTIPLIED"))
        (Camera::PixelFormat::BGRA32,                 std::string("BGRA32"))
        (Camera::PixelFormat::BGRA32_PREMULTIPLIED,   std::string("BGRA32_PREMULTIPLIED"))
        (Camera::PixelFormat::BGR32,                  std::string("BGR32"))
        (Camera::PixelFormat::BGR24,                  std::string("BGR24"))
        (Camera::PixelFormat::BGR565,                 std::string("BGR565"))
        (Camera::PixelFormat::BGR555,                 std::string("BGR555"))
        (Camera::PixelFormat::BGRA5658_PREMULTIPLIED, std::string("BGRA5658_PREMULTIPLIED"))
        (Camera::PixelFormat::AYUV444,                std::string("AYUV444"))
        (Camera::PixelFormat::AYUV444_PREMULTIPLIED,  std::string("AYUV444_PREMULTIPLIED"))
        (Camera::PixelFormat::YUV444,                 std::string("YUV444"))
        (Camera::PixelFormat::YUV420P,                std::string("YUV420P"))
        (Camera::PixelFormat::YV12,                   std::string("YV12"))
        (Camera::PixelFormat::UYVY,                   std::string("UYVY"))
        (Camera::PixelFormat::YUYV,                   std::string("YUYV"))
        (Camera::PixelFormat::NV12,                   std::string("NV12"))
        (Camera::PixelFormat::NV21,                   std::string("NV21"))
        (Camera::PixelFormat::IMC1,                   std::string("IMC1"))
        (Camera::PixelFormat::IMC2,                   std::string("IMC2"))
        (Camera::PixelFormat::IMC3,                   std::string("IMC3"))
        (Camera::PixelFormat::IMC4,                   std::string("IMC4"))
        (Camera::PixelFormat::Y8,                     std::string("Y8"))
        (Camera::PixelFormat::Y16,                    std::string("Y16"))
        (Camera::PixelFormat::JPEG,                   std::string("JPEG"))
        (Camera::PixelFormat::CAMERARAW,              std::string("CAMERARAW"))
        (Camera::PixelFormat::ADOBEDNG,               std::string("ADOBEDNG"))
        (Camera::PixelFormat::RGBA32,                 std::string("RGBA32"))
        (Camera::PixelFormat::USER,                   std::string("USER"));

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

void Camera::shallowCopy( const ::fwData::Object::csptr& _source )
{
    Camera::csptr other = Camera::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( _source );

    m_width                 = other->m_width;
    m_height                = other->m_height;
    m_intrinsic             = other->m_intrinsic;
    m_distortionCoefficient = other->m_distortionCoefficient;
    m_skew                  = other->m_skew;
    m_isCalibrated          = other->m_isCalibrated;
    m_description           = other->m_description;
    m_cameraID              = other->m_cameraID;
    m_maxFrameRate          = other->m_maxFrameRate;
    m_pixelFormat           = other->m_pixelFormat;
    m_videoFile             = other->m_videoFile;
    m_streamUrl             = other->m_streamUrl;
    m_cameraSource          = other->m_cameraSource;
    m_scale                 = other->m_scale;
}

//------------------------------------------------------------------------------

void Camera::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    Camera::csptr other = Camera::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    m_intrinsic[0] = other->m_intrinsic[0];
    m_intrinsic[1] = other->m_intrinsic[1];
    m_intrinsic[2] = other->m_intrinsic[2];
    m_intrinsic[3] = other->m_intrinsic[3];

    m_distortionCoefficient[0] = other->m_distortionCoefficient[0];
    m_distortionCoefficient[1] = other->m_distortionCoefficient[1];
    m_distortionCoefficient[2] = other->m_distortionCoefficient[2];
    m_distortionCoefficient[3] = other->m_distortionCoefficient[3];
    m_distortionCoefficient[4] = other->m_distortionCoefficient[4];

    m_skew         = other->m_skew;
    m_width        = other->m_width;
    m_height       = other->m_height;
    m_isCalibrated = other->m_isCalibrated;
    m_description  = other->m_description;
    m_cameraID     = other->m_cameraID;
    m_maxFrameRate = other->m_maxFrameRate;
    m_pixelFormat  = other->m_pixelFormat;
    m_videoFile    = other->m_videoFile;
    m_streamUrl    = other->m_streamUrl;
    m_cameraSource = other->m_cameraSource;
    m_scale        = other->m_scale;
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

} // namespace arData

