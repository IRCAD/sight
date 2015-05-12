/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/TransformationMatrix3D.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(TransformationMatrix3D))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("coefficient", &::fwData::TransformationMatrix3D::m_vCoefficients)
    ;
}
