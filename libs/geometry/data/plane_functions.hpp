/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include "geometry/data/config.hpp"
#include "geometry/data/line_functions.hpp"
#include "geometry/data/matrix_functions.hpp"
#include "geometry/data/types.hpp"
#include "geometry/data/vector_functions.hpp"

#include <core/base.hpp>

namespace sight::geometry::data
{

static const double EPSILON = 0.001;

/**
 * @brief
 */
GEOMETRY_DATA_API fw_plane get_plane(const fw_vec3d& _point1, const fw_vec3d& _point2, const fw_vec3d& _point3);
/**
 * @brief compute a plane from a normal and a point which must be in the plane.
 * @param [in] _point a point of the plan/
 * @param [in] _normal the normal of the new plane.
 * @return the new plane.
 */
GEOMETRY_DATA_API fw_plane get_plane(const fw_vec3d& _normal, const fw_vec3d& _point);

/**
 * @brief Initialize a plane _plane with three points (_point1, _point2, _point3).
 * It computes the plane's normal and the distance from the origin.
 * @param [out] _plane the new plane.
 * @param [in] _point1 a point of the plan.
 * @param [in] _point2 a point of the plan.
 * @param [in] _point3 a point of the plan.
 */
GEOMETRY_DATA_API void set_values(
    fw_plane& _plane,
    const fw_vec3d& _point1,
    const fw_vec3d& _point2,
    const fw_vec3d& _point3
);

/**
 * @brief Return the normal of the given plane _plane.
 * @param [in] _plane
 * @return the normalize normal of the plane.
 */
GEOMETRY_DATA_API fw_vec3d get_normal(const fw_plane& _plane);
/**
 * @brief Set the normal of the given plane _plane.
 *  @param [in] _plane
 *  @param [in] _normal
 */
GEOMETRY_DATA_API void set_normal(fw_plane& _plane, const fw_vec3d& _normal);
/**
 * @brief Get the distance from origin for the given plan (_plane).
 *  @param [in] _plane
 *  @return the distance of origin of the plane.
 */
GEOMETRY_DATA_API double get_distance(const fw_plane& _plane);
/**
 * @brief Set the distance from origin (_distance) for the given plan (_plane).
 *  @param [in]  _distance
 *  @param [in] _plane
 *  @return [out] _plane
 */
GEOMETRY_DATA_API void set_distance(fw_plane& _plane, double _distance);
/**
 *  @brief Give the intersection between a plane and a line. The result is returned in a point (_point)
 *  @param [in]  _fw_plane
 *  @param [in]  _line
 *  @param [out] _point intersection point.
 *  @return true if an intersection is found.
 */
GEOMETRY_DATA_API bool intersect(const fw_plane& _fw_plane, const fw_line& _line, fw_vec3d& _point);

/**
 *  @brief Compute if a point is in a half plane.
 *  @param [in]  _plane
 *  @param [in]  _point
 *  @return true if point is in a half plane.
 */
GEOMETRY_DATA_API bool is_in_half_space(const fw_plane& _plane, const fw_vec3d& _point);

/**
 *  @brief Apply a transformation to a plane. The transformation is defined by a matrix 4x4.
 *  @param [in]  _plane
 *  @param [in]  _matrix
 *  @param [out] _plane
 */
GEOMETRY_DATA_API void transform(fw_plane& _plane, const fw_matrix4x4& _matrix);

/**
 *  @brief Add an offset at the distance of origin which define the plane (_plane).
 *  @param [in]  _plane
 *  @param [in]  _offset
 *  @param [out] _plane
 */
GEOMETRY_DATA_API void offset(fw_plane& _plane, double _offset);

} // namespace sight::geometry::data

/**
 *  @brief Compares if two planes are equal
 *  @param [in]  _plane1
 *  @param [in]  _plane2
 *  @return true if the planes are equal and false if they are not.
 */
GEOMETRY_DATA_API bool operator==(fw_plane& _plane1, fw_plane& _plane2);
