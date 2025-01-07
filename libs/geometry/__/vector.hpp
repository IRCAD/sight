/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#pragma once

#include <sight/geometry/__/config.hpp>

#include <glm/vec3.hpp>

#include <functional>
#include <optional>

// cspell: ignore orthogonalize

namespace sight::geometry
{

SIGHT_GEOMETRY_API bool orthogonalize(
    glm::dvec3& _vec1,
    glm::dvec3& _vec2,
    const std::optional<std::reference_wrapper<glm::dvec3> >& _vec3 = std::nullopt
);

} // namespace sight::geometry
