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

#include "geometry/__/line.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <optional>

namespace sight::geometry
{

/**
 * @brief Definition of type for a plane defined by a normal and a distance
 */
using plane_t = glm::dvec4;

/**
 * @brief
 */
SIGHT_GEOMETRY_API plane_t get_plane(const glm::dvec3& _point1, const glm::dvec3& _point2, const glm::dvec3& _point3);
/**
 * @brief compute a plane from a normal and a point which must be in the plane.
 * @param [in] _point a point of the plan/
 * @param [in] _normal the normal of the new plane.
 * @return the new plane.
 */
SIGHT_GEOMETRY_API plane_t get_plane(const glm::dvec3& _normal, const glm::dvec3& _point);

/**
 * @brief Initialize a plane _plane with three points (_point1, _point2, _point3).
 * It computes the plane's normal and the distance from the origin.
 * @param [out] _plane the new plane.
 * @param [in] _point1 a point of the plan.
 * @param [in] _point2 a point of the plan.
 * @param [in] _point3 a point of the plan.
 */
SIGHT_GEOMETRY_API void set_plane(
    plane_t& _plane,
    const glm::dvec3& _point1,
    const glm::dvec3& _point2,
    const glm::dvec3& _point3
);

/**
 * @brief Return the normal of the given plane _plane.
 * @param [in] _plane
 * @return the normalize normal of the plane.
 */
SIGHT_GEOMETRY_API glm::dvec3 get_normal(const plane_t& _plane);
/**
 * @brief Set the normal of the given plane _plane.
 *  @param [in] _plane
 *  @param [in] _normal
 */
SIGHT_GEOMETRY_API void set_normal(plane_t& _plane, const glm::dvec3& _normal);
/**
 * @brief Get the distance from origin for the given plan (_plane).
 *  @param [in] _plane
 *  @return the distance of origin of the plane.
 */
SIGHT_GEOMETRY_API double get_distance(const plane_t& _plane);
/**
 * @brief Set the distance from origin (_distance) for the given plan (_plane).
 *  @param [in]  _distance
 *  @param [in] _plane
 *  @return [out] _plane
 */
SIGHT_GEOMETRY_API void set_distance(plane_t& _plane, double _distance);

/**
 *  @brief Give the intersection between a plane and a line. The result is returned in a point (_point)
 *  @param [in]  _plane
 *  @param [in]  _line
 *  @return intersection if found, nullopt otherwise.
 */
SIGHT_GEOMETRY_API std::optional<glm::dvec3> intersect(const plane_t& _plane, const line_t& _line);

/**
 *  @brief Give the intersection between a plane and a ray. The result is returned in a point (_point)
 *  @param [in]  _plane
 *  @param [in]  ray
 *  @return intersection if found, nullopt otherwise
 */
SIGHT_GEOMETRY_API std::optional<glm::dvec3> intersect_ray(const plane_t& _plane, const ray_t& _ray);

/**
 *  @brief Compute if a point is in a half plane.
 *  @param [in]  _plane
 *  @param [in]  _point
 *  @return true if point is in a half plane.
 */
SIGHT_GEOMETRY_API bool is_in_half_space(const plane_t& _plane, const glm::dvec3& _point);

/**
 *  @brief Apply a transformation to a plane. The transformation is defined by a matrix 4x4.
 *  @param [in]  _plane
 *  @param [in]  _matrix
 *  @param [out] _plane
 */
SIGHT_GEOMETRY_API void transform(plane_t& _plane, const glm::dmat4& _matrix);

/**
 *  @brief Add an offset at the distance of origin which define the plane (_plane).
 *  @param [in]  _plane
 *  @param [in]  _offset
 *  @param [out] _plane
 */
SIGHT_GEOMETRY_API void offset(plane_t& _plane, double _offset);

} // namespace sight::geometry

/**
 *  @brief Compares if two planes are equal
 *  @param [in]  _plane1
 *  @param [in]  _plane2
 *  @return true if the planes are equal and false if they are not.
 */
SIGHT_GEOMETRY_API bool operator==(sight::geometry::plane_t& _plane1, sight::geometry::plane_t& _plane2);
