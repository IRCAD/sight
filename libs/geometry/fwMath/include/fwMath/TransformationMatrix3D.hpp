/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "data/config.hpp"

#include <core/base.hpp>

#include <data/Point.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <glm/mat4x4.hpp>

namespace sight::data::tools
{
/**
 * @brief   TransformationMatrix3D performs matrices computations.
 */
class TransformationMatrix3D
{
public:

    /**
     * @brief Invert a matrix.
     * @param[in] _input Input matrix
     * @param[out] _output Inverted matrix.
     */
    DATA_API static bool invert(const data::TransformationMatrix3D::csptr& _input,
                                data::TransformationMatrix3D::sptr& _output);

    /**
     * @brief Multiply two matrices.
     * @param[in] trfA First matrix.
     * @param[in] trfB Second matrix.
     * @param[out] _output Output matrix.
     */
    DATA_API static void multiply(const data::TransformationMatrix3D::csptr& _trfA,
                                  const data::TransformationMatrix3D::csptr& _trfB,
                                  data::TransformationMatrix3D::sptr& _output);

    /**
     * @brief Set the matrix to identity.
     * @param[in,out] _trf Matrix we want to set to identity.
     */
    DATA_API static void identity(data::TransformationMatrix3D::sptr& _trf);

    /**
     * @brief Multiply a point and a matrix.
     * @param[in] _trf Matrix.
     * @param[in] _input Input point.
     * @param[out] _output Output point.
     */
    DATA_API static void multiply(const data::TransformationMatrix3D::csptr& _trf,
                                  const data::Point::csptr& _input, data::Point::sptr& _output);

    /**
     * @brief Convert a data::TransformationMatrix3D into a GLM matrix.
     * @param[in] _trf Input data::TransformationMatrix3D.
     * @return GLM matrix.
     */
    DATA_API static ::glm::dmat4x4 getMatrixFromTF3D(const data::TransformationMatrix3D::csptr& _trf);

    /**
     * @brief Convert a GLM matrix into a data::TransformationMatrix3D.
     * @param[out] _trf Output data::TransformationMatrix3D.
     * @param[in] _input Input GLM matrix.
     */
    DATA_API static void setTF3DFromMatrix(data::TransformationMatrix3D::sptr& _trf,
                                           const ::glm::dmat4x4& _input);

    /**
     * @brief Return whether a data::TransformationMatrix3D is an identity matrix.
     * @param[in] _trf Input data::TransformationMatrix3D.
     * @param[in] _epsilon Precision of the test (default 1e-12)
     * @return boolean value: true if the matrix is identity, false otherwise.
     */
    DATA_API static bool isIdentity(const data::TransformationMatrix3D::csptr& _trf,
                                    const double _epsilon = 1e-12);
};

// ----------------------------------------------------------------------------

inline ::glm::dmat4x4 TransformationMatrix3D::getMatrixFromTF3D(const data::TransformationMatrix3D::csptr& _trf)
{
    // TransformationMatrix3D is stored row-major
    // glm matrices are stored column-major
    const auto& coefs = _trf->getCoefficients();

    ::glm::dmat4x4 mat(coefs[0], coefs[4], coefs[8], coefs[12],
                       coefs[1], coefs[5], coefs[9], coefs[13],
                       coefs[2], coefs[6], coefs[10], coefs[14],
                       coefs[3], coefs[7], coefs[11], coefs[15]);

    return mat;
}

// ----------------------------------------------------------------------------

inline void TransformationMatrix3D::setTF3DFromMatrix(data::TransformationMatrix3D::sptr& _trf,
                                                      const ::glm::dmat4x4& _input)
{
    // TransformationMatrix3D is stored row-major
    // glm matrices are stored column-major
    auto& coefs = _trf->getCoefficients();
    for (size_t i = 0; i < 4; ++i)
    {
        const size_t rowDst          = i * 4;
        const ::glm::length_t rowSrc = static_cast< ::glm::length_t>(i);
        for (size_t j = 0; j < 4; ++j)
        {
            const ::glm::length_t colSrc = static_cast< ::glm::length_t>(j);
            coefs[rowDst + j] = _input[colSrc][rowSrc];
        }
    }
}
} // namespace sight::data::tools
