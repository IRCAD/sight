/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/Tag.hpp"

fwCampImplementDataMacro((fwData)(Tag))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("point_list", &::fwData::Tag::m_pointList)
    .property("type", &::fwData::Tag::m_sType)
    .property("size", &::fwData::Tag::m_size)
    ;
}
