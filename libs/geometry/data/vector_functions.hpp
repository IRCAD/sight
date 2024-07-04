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

#include <sight/geometry/data/config.hpp>

#include "geometry/data/types.hpp"

#include <core/base.hpp>

#include <functional>
#include <optional>

// cspell: ignore orthogonalize

namespace sight::geometry::data
{

/**
 *  @brief Normalize a vector
 */
SIGHT_GEOMETRY_DATA_API double normalize(fw_vec3d& _vec);

/**
 *  @brief Return a normalized vector
 *  @param [in] _vec vector to normalize
 *  @return normalized vector
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d normalized(const fw_vec3d& _vec);

/**
 *  @brief Compute the Dot product between two vectors.
 *  @param [in] _v1
 *  @param [in] _v2
 *  @return the dot product.
 */
SIGHT_GEOMETRY_DATA_API double dot(const fw_vec3d& _v1, const fw_vec3d& _v2);
/**
 *  @brief Compute the Cross product between two vectors.
 *  @param [in] _v1
 *  @param [in] _v2
 *  @return the cross product.
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d cross(const fw_vec3d& _v1, const fw_vec3d& _v2);

/**
 *  @brief Compute the length of the vector
 *  @param [in] _vec
 *  @return the vector's length
 */
SIGHT_GEOMETRY_DATA_API double vec_length(const fw_vec3d& _vec);

/**
 *  @brief Compute the negate of the vector.
 *  @param [in] _vec
 *  @param [out] _vec
 */
SIGHT_GEOMETRY_DATA_API void negate(fw_vec3d& _vec);

/**
 *  @brief Makes the vectors orthogonal.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @param [out] _vec3 (optional)
 *  @return false if already orthogonal, true otherwise.
 */
SIGHT_GEOMETRY_DATA_API bool orthogonalize(
    fw_vec3d& _vec1,
    fw_vec3d& _vec2,
    const std::optional<std::reference_wrapper<fw_vec3d> >& _vec3 = std::nullopt
);

} //namespace sight::geometry::data

/**
 *  @brief operator *= for fw_vec3d
 *  @param [in] _vec1 vector to multiply
 *  @param [in] _val value to multiply with vector
 *  @return vec1[0]*=vec2[0], vec1[1]*=vec2[1], vec1[2]*=vec2[2]
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d& operator*=(fw_vec3d& _vec1, double _val);

/**
 *  @brief operator /= for fw_vec3d
 *  @param [in] _vec
 *  @param [in] _val
 *  @return vec[0]/=val, vec[1]/=val, vec[2]/=val
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d& operator/=(fw_vec3d& _vec, double _val);

/**
 *  @brief operator += for fw_vec3d
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return  vec1 => vec1[0]+=vec2[0], vec1[1]+=vec2[1], vec1[2]+=vec2[2]
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d& operator+=(fw_vec3d& _vec1, const fw_vec3d& _vec2);

/**
 *  @brief
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return vec1[0]-=vec2[0], vec1[1]-=vec2[1], vec1[2]-=vec2[2]
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d& operator-=(fw_vec3d& _vec1, const fw_vec3d& _vec2);

/**
 *  @brief operator * between fw_vec3d and value.
 *  @param [in] _vec
 *  @param [in] _val
 *  @return the result of the operation _vec*val
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d operator*(const fw_vec3d& _vec, double _val);

/**
 *  @brief operator * between value and fw_vec3d.
 *  @param [in] _val
 *  @param [in] _vec
 *  @return the result of the operation _vec*val
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d operator*(double _val, const fw_vec3d& _vec);

/**
 *  @brief operator / between fw_vec3d and value.
 *  @param [in] _vec
 *  @param [in] _val
 *  @return the result of the operation _vec/val
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d operator/(const fw_vec3d& _vec, double _val);

/**
 *  @brief operator + between two fw_vec3d.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return the result of the operation _vec1 + _vec2
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d operator+(const fw_vec3d& _vec1, const fw_vec3d& _vec2);

/**
 *  @brief operator - between two fw_vec3d data.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return the difference between two value.
 */
SIGHT_GEOMETRY_DATA_API fw_vec3d operator-(const fw_vec3d& _vec1, const fw_vec3d& _vec2);

/**
 *  @brief Compare two fw_vec3d data.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return return 1 if equal 0 otherwise.
 */
SIGHT_GEOMETRY_DATA_API bool operator==(const fw_vec3d& _vec1, const fw_vec3d& _vec2);

/**
 *  @brief Compare two fw_vec3d data.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return 1 if different.
 */
SIGHT_GEOMETRY_DATA_API bool operator!=(const fw_vec3d& _vec1, const fw_vec3d& _vec2);
