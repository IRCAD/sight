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

#include "data/Plane.hpp"

#include <core/reflection/UserObject.hpp>

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (Plane))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "sight::data")
    .base<sight::data::Object>()
    .property("points", &sight::data::Plane::m_vPoints)
    .property("intersection", &sight::data::Plane::m_isIntersection)
    ;
}
