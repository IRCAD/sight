/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "data/detail/reflection/Image.hpp"

#include <core/memory/camp/mapper.hpp>
#include <core/reflection/UserObject.hpp>

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (Image))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "::sight::data")
    .base< ::sight::data::Object>()
    .property("size", &::sight::data::Image::m_size)
    .property("type", &::sight::data::Image::m_type)
    .property("spacing", &::sight::data::Image::m_spacing)
    .property("origin", &::sight::data::Image::m_origin)
    .property("array", &::sight::data::Image::m_dataArray)
    .property("nb_components", &::sight::data::Image::m_numberOfComponents)
    .property("window_center", &::sight::data::Image::m_windowCenter)
    .property("window_width", &::sight::data::Image::m_windowWidth)
    .property("pixel_format", &::sight::data::Image::m_pixelFormat)
    ;
}

SIGHT_IMPLEMENT_ENUM_REFLECTION((sight) (data) (Image) (PixelFormat))
{
    builder
    .value("UNDEFINED", sight::data::Image::PixelFormat::UNDEFINED)
    .value("GRAY_SCALE", sight::data::Image::PixelFormat::GRAY_SCALE)
    .value("RGB", sight::data::Image::PixelFormat::RGB)
    .value("RGBA", sight::data::Image::PixelFormat::RGBA)
    .value("BGR", sight::data::Image::PixelFormat::BGR)
    .value("BGRA", sight::data::Image::PixelFormat::BGRA)
    ;
}
