/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <arData/Camera.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((arData)(Camera))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "arData")
    .base< ::fwData::Object>()
    .property("intrinsic",&::arData::Camera::m_intrinsic)
    .property("is_calibrated",&::arData::Camera::m_isCalibrated)
    .property("skew",&::arData::Camera::m_skew)
    .property("distortion_coefficient",&::arData::Camera::m_distortionCoefficient)
    .property("description",&::arData::Camera::m_description)
    .property("camera_id",&::arData::Camera::m_cameraID)
    ;
}
