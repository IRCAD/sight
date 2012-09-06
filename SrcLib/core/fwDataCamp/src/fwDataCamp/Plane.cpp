/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Plane.hpp"

fwCampImplementDataMacro((fwData)(Plane))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("plane", &::fwData::Plane::m_plane)
        .property("points", &::fwData::Plane::m_vPoints)
        .property("intersection", &::fwData::Plane::m_isIntersection)
        ;
}
