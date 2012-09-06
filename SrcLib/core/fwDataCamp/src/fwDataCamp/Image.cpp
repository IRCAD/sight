/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/camp/mapper.hpp>

#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Image.hpp"


fwCampImplementDataMacro((fwData)(Image))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("size", &::fwData::Image::m_size)
        .property("type", &::fwData::Image::m_type)
        .property("spacing", &::fwData::Image::m_spacing)
        .property("origin", &::fwData::Image::m_origin)
        .property("array", &::fwData::Image::m_dataArray)
        ;
}
