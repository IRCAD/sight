/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <array>
#include <vector>

/**
 * @brief Definition of a type for a vector of tree double.
 */
using fw_vec3d = std::array<double, 3>;

/**
 * @brief Definition of a type for a line defined by a position and a direction
 */
using fw_line = std::pair<fw_vec3d, fw_vec3d>;

/**
 * @brief Definition of type for a plane defined by a normal and a distance
 */
using fw_plane = std::array<double, 4>;

/**
 * @brief Definition of a type for a matrix 4*4
 */
using fw_matrix4x4 = std::array<std::array<double, 4>, 4>;

/**
 * @brief Definition of a type for vertex positions
 */
using fw_vertex_position = std::vector<std::vector<float> >;

/**
 * @brief Definition of a type for vertex index
 */
using fw_vertex_index = std::vector<std::vector<int> >;
