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
#include "geometry/data/types.hpp"

#include <core/base.hpp>

namespace sight::geometry::data
{

/**
 * @brief Multiply a matrix by a vector
 *  @param [in]  _matrix
 *  @param [in]  _source
 *  @param [out] _dest
 *
 */
GEOMETRY_DATA_API void mult_vec_matrix(const fw_matrix4x4& _matrix, const fw_vec3d& _source, fw_vec3d& _dest);

/**
 * @brief Compute a matrix rotation
 * @deprecated The function implementation seems very dodgy, prefer glm::yawPitchRoll() or glm::orientate4()
 *  @param [in]  _vec_norm
 *  @return rotation matrix
 *
 */
GEOMETRY_DATA_API fw_matrix4x4 get_rotation_matrix(const fw_vec3d& _vec_norm);

/**
 * @brief Computes the inverse of a matrix
 *  @param [in]  _matrix
 *  @return inverted matrix
 *
 */
GEOMETRY_DATA_API fw_matrix4x4 get_inverse(const fw_matrix4x4& _matrix);

} // namespace sight::geometry::data

/**
 * @brief Multiplies a matrix by a matrix
 *  @param [in]  _matrix1
 *  @param [in]  _matrix2
 *  @return matrix
 *
 */
GEOMETRY_DATA_API fw_matrix4x4 operator*(const fw_matrix4x4& _matrix1, const fw_matrix4x4& _matrix2);
