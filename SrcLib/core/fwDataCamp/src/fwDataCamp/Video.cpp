/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/Video.hpp"

fwCampImplementDataMacro((fwData)(Video))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object >()
    .property("data_available",&fwData::Video::m_dataAvailable)
    .property("last_modified",&fwData::Video::m_lastModified)
    .property("camera",&fwData::Video::m_camera)
    ;
}
