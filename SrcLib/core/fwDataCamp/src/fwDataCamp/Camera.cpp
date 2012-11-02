/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/Camera.hpp"

fwCampImplementDataMacro((fwData)(Camera))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "2")
        .tag("lib_name", "fwData")
        .property("extrinsic_matric",&::fwData::Camera::m_extrinsicMatrix)
        .property("intrinsic_matrix",&::fwData::Camera::m_intrinsicMatrix)
        .property("data_available",&::fwData::Camera::m_dataAvailable)
        .property("skew",&::fwData::Camera::m_attrSkew)
        .property("distortion_coefficient",&::fwData::Camera::m_attrDistortionCoefficient)
        ;
}

