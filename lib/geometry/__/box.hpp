/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <glm/geometric.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

namespace sight::geometry
{

/**
 * @brief An oriented bounding box defined by a center, per-axis half-extents, and an orientation matrix.
 *
 * orientation columns are the box axes in world space; extent holds the half-length along each axis
 * (always positive).
 */
struct oriented_box_t
{
    glm::dvec3 center;
    glm::dvec3 extent; ///< distance from the center to the edge in each direction, always positive
    glm::dmat3 orientation;

    /// Returns true if all extents are positive and orientation is orthonormal
    [[nodiscard]] bool is_valid() const
    {
        constexpr double eps = 1e-6;
        const glm::dvec3 x   = orientation[0];
        const glm::dvec3 y   = orientation[1];
        const glm::dvec3 z   = orientation[2];
        const double len_x   = glm::length(x);
        const double len_y   = glm::length(y);
        const double len_z   = glm::length(z);
        return extent.x > 0.0 && extent.y > 0.0 && extent.z > 0.0
               && std::abs(len_x - 1.0) < eps* std::max(1.0, len_x)
               && std::abs(len_y - 1.0) < eps* std::max(1.0, len_y)
               && std::abs(len_z - 1.0) < eps* std::max(1.0, len_z)
               && std::abs(glm::dot(x, y)) < eps
               && std::abs(glm::dot(x, z)) < eps
               && std::abs(glm::dot(y, z)) < eps;
    }
};

/**
 * @brief Test whether two oriented bounding boxes intersect.
 *
 * Uses the separating axis theorem with all 15 candidate axes (3 face normals from each box plus
 * 9 edge-cross-product axes). Both boxes must have orthonormal orientation matrices.
 *
 * @param _a first oriented box
 * @param _b second oriented box
 *
 * @return true if the boxes overlap, false if a separating axis was found.
 */
SIGHT_GEOMETRY_API bool intersect(const oriented_box_t& _a, const oriented_box_t& _b);

} // namespace sight::geometry
