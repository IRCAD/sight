/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <fwData/Landmarks.hpp>

fwCampImplementDataMacro((fwData)(Landmarks))
{
    builder.base< ::fwData::Object>()
    .tag("object_version", "1")
    .tag("lib_name", "fwData");
    // property 'landmarks' is not introspected because it contains a struct that is not managed by camp
    //.property("landmarks", &::fwData::Landmarks::m_landmarks);
}

