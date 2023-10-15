/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <data/matrix4.hpp>
#include <data/point.hpp>

#include <glm/mat4x4.hpp>

namespace sight::geometry::data
{

/**
 * @brief Invert a matrix.
 * @param[in] _input Input matrix
 * @param[out] _output Inverted matrix.
 */
GEOMETRY_DATA_API bool invert(
    const sight::data::matrix4& _input,
    sight::data::matrix4& _output
);

/**
 * @brief Multiply two matrices.
 * @param[in] trfA First matrix.
 * @param[in] trfB Second matrix.
 * @param[out] _output Output matrix.
 */
GEOMETRY_DATA_API void multiply(
    const sight::data::matrix4& _trfA,
    const sight::data::matrix4& _trfB,
    sight::data::matrix4& _output
);

/**
 * @brief Set the matrix to identity.
 * @param[in,out] _trf Matrix we want to set to identity.
 */
GEOMETRY_DATA_API void identity(sight::data::matrix4& _trf);

/**
 * @brief Multiply a point and a matrix.
 * @param[in] _trf Matrix.
 * @param[in] _input Input point.
 * @param[out] _output Output point.
 */
GEOMETRY_DATA_API void multiply(
    const sight::data::matrix4& _trf,
    const sight::data::point& _input,
    sight::data::point& _output
);

/**
 * @brief Return whether a data::matrix4 is an identity matrix.
 * @param[in] _trf Input data::matrix4.
 * @param[in] _epsilon Precision of the test (default 1e-12)
 * @return boolean value: true if the matrix is identity, false otherwise.
 */
GEOMETRY_DATA_API bool isIdentity(
    const sight::data::matrix4& _trf,
    double _epsilon = 1e-12
);

/**
 * @brief Convert a data::matrix4 into a GLM matrix.
 * @param[in] _trf Input data::matrix4.
 * @return GLM matrix.
 */
inline glm::dmat4x4 getMatrixFromTF3D(const sight::data::matrix4& _trf)
{
    // Matrix4 is stored row-major
    // glm matrices are stored column-major
    glm::dmat4x4 mat(_trf[0], _trf[4], _trf[8], _trf[12],
                     _trf[1], _trf[5], _trf[9], _trf[13],
                     _trf[2], _trf[6], _trf[10], _trf[14],
                     _trf[3], _trf[7], _trf[11], _trf[15]);

    return mat;
}

/**
 * @brief Convert a GLM matrix into a data::matrix4.
 * @param[out] _trf Output data::matrix4.
 * @param[in] _input Input GLM matrix.
 */
inline void setTF3DFromMatrix(sight::data::matrix4& _trf, const glm::dmat4x4& _input)
{
    // Matrix4 is stored row-major
    // glm matrices are stored column-major
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        const std::size_t rowDst = i * 4;
        const auto rowSrc        = static_cast<glm::length_t>(i);
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto colSrc = static_cast<glm::length_t>(j);
            _trf[rowDst + j] = _input[colSrc][rowSrc];
        }
    }
}

} // namespace sight::geometry::data
