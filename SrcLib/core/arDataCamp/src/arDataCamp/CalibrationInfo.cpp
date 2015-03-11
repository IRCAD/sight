/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include <arData/CalibrationInfo.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((arData)(CalibrationInfo))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "arData")
        .base< ::fwData::Object >()
        .property("pointlist_container", &::arData::CalibrationInfo::m_pointListContainer)
        .property("image_container", &::arData::CalibrationInfo::m_imageContainer)
        ;
}
