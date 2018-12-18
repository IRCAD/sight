/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <fwMemory/camp/mapper.hpp>

#include <fwCamp/UserObject.hpp>

#include "fwDataCamp/Image.hpp"


fwCampImplementDataMacro((fwData)(Image))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("size", &::fwData::Image::m_size)
    .property("type", &::fwData::Image::m_type)
    .property("spacing", &::fwData::Image::m_spacing)
    .property("origin", &::fwData::Image::m_origin)
    .property("array", &::fwData::Image::m_dataArray)
    .property("nb_components", &::fwData::Image::m_numberOfComponents)
    .property("window_center", &::fwData::Image::m_windowCenter)
    .property("window_width", &::fwData::Image::m_windowWidth)
    ;
}
