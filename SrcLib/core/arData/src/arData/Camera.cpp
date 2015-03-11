/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "arData/Camera.hpp"

fwDataRegisterMacro(::arData::Camera);

namespace arData
{

const ::fwCom::Signals::SignalKeyType Camera::s_INTRINSIC_CALIBRATED_SIG = "intrinsicCalibrated";
const ::fwCom::Signals::SignalKeyType Camera::s_ID_MODIFIED_SIG          = "idModified";

//------------------------------------------------------------------------------

Camera::Camera( ::fwData::Object::Key key ) :
    m_attrSkew(0.0),
    m_attrIsCalibrated(false)
{
    m_sigIntrinsicCalibrated = IntrinsicCalibratedSignalType::New();
    m_sigIdModified          = IdModifiedSignalType::New();

    ::fwCom::HasSignals::m_signals(s_INTRINSIC_CALIBRATED_SIG, m_sigIntrinsicCalibrated)
        (s_ID_MODIFIED_SIG, m_sigIdModified);

#ifdef COM_LOG
    ::fwCom::HasSignals::m_signals.setID();
#endif
}

// -------------------------------------------------------------------------

Camera::~Camera() throw()
{
}

//------------------------------------------------------------------------------

void Camera::shallowCopy( const ::fwData::Object::csptr& _source )
{
    Camera::csptr other = Camera::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( _source );

    m_intrinsic                 = other->m_intrinsic;
    m_attrDistortionCoefficient = other->m_attrDistortionCoefficient;
    m_attrSkew                  = other->m_attrSkew;
    m_attrIsCalibrated          = other->m_attrIsCalibrated;
    m_description               = other->m_description;
    m_cameraID                  = other->m_cameraID;
}

//------------------------------------------------------------------------------

void Camera::cachedDeepCopy(const Object::csptr &source, DeepCopyCacheType &cache)
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
    m_intrinsic[4] = other->m_intrinsic[4];
    m_intrinsic[5] = other->m_intrinsic[5];

    m_attrDistortionCoefficient[0] = other->m_attrDistortionCoefficient[0];
    m_attrDistortionCoefficient[1] = other->m_attrDistortionCoefficient[1];
    m_attrDistortionCoefficient[2] = other->m_attrDistortionCoefficient[2];
    m_attrDistortionCoefficient[3] = other->m_attrDistortionCoefficient[3];
    m_attrDistortionCoefficient[4] = other->m_attrDistortionCoefficient[4];
    m_attrSkew                     = other->m_attrSkew;

    m_attrIsCalibrated = other->m_attrIsCalibrated;
    m_description      = other->m_description;
    m_cameraID         = other->m_cameraID;
}

// -------------------------------------------------------------------------

void Camera::setDistortionCoefficient(double k1, double k2,
                                      double p1, double p2, double k3)
{
    m_attrDistortionCoefficient[0] = k1;
    m_attrDistortionCoefficient[1] = k2;
    m_attrDistortionCoefficient[2] = p1;
    m_attrDistortionCoefficient[3] = p2;
    m_attrDistortionCoefficient[4] = k3;
}

// -------------------------------------------------------------------------

} // namespace arData

