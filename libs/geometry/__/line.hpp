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

#include <utility>

namespace sight::geometry
{

/**
 * @brief Definition of a type for a line defined by two positions
 */
using line_t = std::pair<glm::dvec3, glm::dvec3>;

/**
 * @brief Definition of a type for a ray defined by a position and a direction
 */
using ray_t = std::pair<glm::dvec3, glm::dvec3>;

//------------------------------------------------------------------------------

/**
 * @brief Compute the closest points between two rays.
 *  @param [in]  _ray1 ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _ray2 ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [out] _point_on_ray1 closest point on the first ray.
 *  @param [out] _point_on_ray2 closest point on the second ray.
 * Return FALSE if the lines are parallel, TRUE otherwise.
 * @verbatim
   p1 + t1 * d1
   p2 + t2 * d2
   (p2 - p1 + t2 * d2 - t1 * d1) * d1 = 0
   (p2 - p1 + t2 * d2 - t1 * d1) * d2 = 0
   t2 * (d2.d1) - t1 = (p1 - p2).d1
   t2 - t1 * (d1.d2) = (p1 - p2).d2

   delta = 1 - (d1.d2)**2

   t2 = [ d2.(p1-p2) - d1.(p1-p2) * (d1.d2)]/delta
   t1 = [-d1.(p1-p2) + d2.(p1-p2) * (d1.d2)]/delta
   @endverbatim
 */
SIGHT_GEOMETRY_API bool get_closest_points(
    const ray_t& _ray1,
    const ray_t& _ray2,
    glm::dvec3& _point_on_ray1,
    glm::dvec3& _point_on_ray2
);

/**
 * @brief Compute the projection of a point in a given direction.
 *  @param [in]  _ray ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _point point to be projected
 *  @return closest point of the line if an intersection is found.
 */
SIGHT_GEOMETRY_API glm::dvec3 get_closest_point(const ray_t& _ray, const glm::dvec3& _point);

/**
 * @brief Compute the projection of a point in a given direction and test if this intersection is inside a given radius.
 *  @param [in]  _ray ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _radius maximum distance of the point
 *  @param [in]  _point point to be projected
 *  @return closest point of the line if an intersection is found.
 */
SIGHT_GEOMETRY_API bool intersect(const ray_t& _ray, double _radius, const glm::dvec3& _point);

/**
 * @brief Compute the closest points between two rays and test these points lie inside a sphere of a given radius.
 *  @param [in]  _line ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _radius maximum distance of the point
 *  @param [in]  _origin origin of the second ray
 *  @param [in]  _direction direction of the second ray
 *  @param [in]  _point point to be projected
 *  @return closest point of the line if an intersection is found.
 */
SIGHT_GEOMETRY_API bool intersect(
    const ray_t& _line,
    double _radius,
    const glm::dvec3& _origin,
    const glm::dvec3& _direction,
    glm::dvec3& _point
);

/**
 * @brief Give the intersection between a plane and a line. The result is returned in a point (_point).
 * @deprecated This function was added for a specific purpose and will be removed in a future release.
 *  @param [in]  _line input line (2 positions)
 *  @param [in]  _v1 first point of the plane
 *  @param [in]  _v2 second point of the plane
 *  @param [in]  _v3 third point of the plane
 *  @param [out] _point intersection point.
 *  @param [out] _barycentric barycenter of the triangle defined by the three points of the plane.
 *  @param [out] _front true if the dot product of the plane normal and ths positive Z axis (0,0,1) is positive.
 *  @return true if an intersection is found.
 */
SIGHT_GEOMETRY_API bool intersect(
    const line_t& _line,
    const glm::dvec3& _v1,
    const glm::dvec3& _v2,
    const glm::dvec3& _v3,
    glm::dvec3& _point,
    glm::dvec3& _barycentric,
    bool& _front
);

} // namespace sight::geometry
