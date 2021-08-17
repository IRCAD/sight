/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Matrix4.hpp>
#include <data/Point.hpp>

#include <glm/mat4x4.hpp>

namespace sight::geometry::data
{

/**
 * @brief Invert a matrix.
 * @param[in] _input Input matrix
 * @param[out] _output Inverted matrix.
 */
GEOMETRY_DATA_API bool invert(
    const sight::data::Matrix4& _input,
    sight::data::Matrix4& _output
);

/**
 * @brief Multiply two matrices.
 * @param[in] trfA First matrix.
 * @param[in] trfB Second matrix.
 * @param[out] _output Output matrix.
 */
GEOMETRY_DATA_API void multiply(
    const sight::data::Matrix4& _trfA,
    const sight::data::Matrix4& _trfB,
    sight::data::Matrix4& _output
);

/**
 * @brief Set the matrix to identity.
 * @param[in,out] _trf Matrix we want to set to identity.
 */
GEOMETRY_DATA_API void identity(sight::data::Matrix4& _trf);

/**
 * @brief Multiply a point and a matrix.
 * @param[in] _trf Matrix.
 * @param[in] _input Input point.
 * @param[out] _output Output point.
 */
GEOMETRY_DATA_API void multiply(
    const sight::data::Matrix4& _trf,
    const sight::data::Point& _input,
    sight::data::Point& _output
);

/**
 * @brief Return whether a data::Matrix4 is an identity matrix.
 * @param[in] _trf Input data::Matrix4.
 * @param[in] _epsilon Precision of the test (default 1e-12)
 * @return boolean value: true if the matrix is identity, false otherwise.
 */
GEOMETRY_DATA_API bool isIdentity(
    const sight::data::Matrix4& _trf,
    const double _epsilon = 1e-12
);

/**
 * @brief Convert a data::Matrix4 into a GLM matrix.
 * @param[in] _trf Input data::Matrix4.
 * @return GLM matrix.
 */
inline glm::dmat4x4 getMatrixFromTF3D(const sight::data::Matrix4& _trf)
{
    // Matrix4 is stored row-major
    // glm matrices are stored column-major
    const auto& coefs = _trf.getCoefficients();

    glm::dmat4x4 mat(coefs[0], coefs[4], coefs[8], coefs[12],
                     coefs[1], coefs[5], coefs[9], coefs[13],
                     coefs[2], coefs[6], coefs[10], coefs[14],
                     coefs[3], coefs[7], coefs[11], coefs[15]);

    return mat;
}

/**
 * @brief Convert a GLM matrix into a data::Matrix4.
 * @param[out] _trf Output data::Matrix4.
 * @param[in] _input Input GLM matrix.
 */
inline void setTF3DFromMatrix(sight::data::Matrix4& _trf, const glm::dmat4x4& _input)
{
    // Matrix4 is stored row-major
    // glm matrices are stored column-major
    auto& coefs = _trf.getCoefficients();
    for(size_t i = 0 ; i < 4 ; ++i)
    {
        const size_t rowDst        = i * 4;
        const glm::length_t rowSrc = static_cast<glm::length_t>(i);
        for(size_t j = 0 ; j < 4 ; ++j)
        {
            const glm::length_t colSrc = static_cast<glm::length_t>(j);
            coefs[rowDst + j] = _input[colSrc][rowSrc];
        }
    }
}

} // namespace sight::geometry::data
