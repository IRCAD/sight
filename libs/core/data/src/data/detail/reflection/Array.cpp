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

#include "data/detail/reflection/Array.hpp"

#include <core/memory/BufferObject.hpp>
#include <core/memory/camp/mapper.hpp>
#include <core/reflection/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((sight)(data)(Array))
{
    builder.base< ::sight::data::Object>()
    .tag("object_version", "1")
    .tag("lib_name", "::sight::data")
    .property("strides", &::sight::data::Array::m_strides)
    .property("size", &::sight::data::Array::m_size)
    .property("type", &::sight::data::Array::m_type)
    .property("nb_of_components", &::sight::data::Array::m_nbOfComponents)
    .property("buffer", &::sight::data::Array::m_bufferObject)
    .property("isOwner", &::sight::data::Array::m_isBufferOwner)
    ;
}
