/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Video.hpp"
#include "fwData/Camera.hpp"

fwDataRegisterMacro(::fwData::Camera);

namespace fwData
{
    Camera::Camera()
        : m_attrSkew(0.0), m_dataAvailable(false)
    {
        SLM_TRACE_FUNC();

        m_extrinsicMatrix = ::fwData::TransformationMatrix3D::New();
        m_intrinsicMatrix = ::fwData::TransformationMatrix3D::New();

        uint32HL def = { 0, 0 };
        m_cameraGUID = std::make_pair(false, def);
    }

    // -------------------------------------------------------------------------

    Camera::~Camera() throw()
    { SLM_TRACE_FUNC(); }

    // -------------------------------------------------------------------------

    void Camera::setCameraGUID(uint32HL cameraGUID)
    {
        m_cameraGUID.first = true;
        m_cameraGUID.second = cameraGUID;
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
} // namespace fwData
