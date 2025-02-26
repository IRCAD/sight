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

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>

namespace sight::geometry
{

/**
 * @brief Fast method to invert of a matrix when it is only composed of a rotation and a translation.
 * Use with care, result will be wrong if a scale is present and no prior check is performed.
 *
 * @param[in] _input Input matrix
 * @return Inverted matrix.
 */
template<typename M = glm::dmat4>
requires std::is_same_v<M, glm::mat4>|| std::is_same_v<M, glm::dmat4>
M inverse_translation_rotation(const M& _input)
{
    const auto inverse_translation = -_input[3];
    const glm::mat<3, 3, typename M::value_type> rotation {
        _input[0][0], _input[0][1], _input[0][2],
        _input[1][0], _input[1][1], _input[1][2],
        _input[2][0], _input[2][1], _input[2][2],
    };

    const auto rotated_translation = inverse_translation * rotation;

    const M inverse_transform = {
        _input[0][0], _input[1][0], _input[2][0], 0.,
        _input[0][1], _input[1][1], _input[2][1], 0.,
        _input[0][2], _input[1][2], _input[2][2], 0.,
        rotated_translation[0], rotated_translation[1], rotated_translation[2], 1.
    };

    return inverse_transform;
}

} // namespace sight::geometry
