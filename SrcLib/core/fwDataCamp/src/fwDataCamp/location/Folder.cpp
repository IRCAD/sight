/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/camp/mapper.hpp>
#include <fwData/location/Folder.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(location)(Folder))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("folder", &::fwData::location::Folder::m_folder)
    .property("recursive", &::fwData::location::Folder::m_isRecursive)
    ;
}
