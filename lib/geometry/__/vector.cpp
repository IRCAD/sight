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

#include "geometry/__/vector.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

namespace sight::geometry
{

//------------------------------------------------------------------------------

bool orthogonalize(
    glm::dvec3& _vec1,
    glm::dvec3& _vec2,
    const std::optional<std::reference_wrapper<glm::dvec3> >& _vec3
)
{
    const glm::dvec3 glm_u = _vec1;
    const glm::dvec3 glm_v = _vec2;

    // Make them Orthogonal
    const auto glm_w     = glm::normalize(glm::cross(glm_u, glm_v));
    const auto new_glm_u = glm::normalize(glm::cross(glm_v, glm_w));
    const auto new_glm_v = glm::cross(glm_w, new_glm_u);

    if(_vec3)
    {
        _vec3->get() = {glm_w.x, glm_w.y, glm_w.z};
    }

    if(glm::all(glm::epsilonEqual(glm_u, new_glm_u, 1e-3)) && glm::all(glm::epsilonEqual(glm_v, new_glm_v, 1e-3)))
    {
        return false;
    }

    _vec1 = {new_glm_u.x, new_glm_u.y, new_glm_u.z};
    _vec2 = {new_glm_v.x, new_glm_v.y, new_glm_v.z};

    return true;
}

} // namespace sight::geometry
