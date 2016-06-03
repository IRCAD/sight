/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_TRANSFORMATIONMATRIX3D_HPP__
#define __FWDATATOOLS_TRANSFORMATIONMATRIX3D_HPP__

#include <fwCore/base.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Point.hpp>

#include "fwDataTools/config.hpp"

#include <glm/mat4x4.hpp>

namespace fwDataTools
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
    FWDATATOOLS_API static bool invert(const ::fwData::TransformationMatrix3D::csptr& _input,
                                       ::fwData::TransformationMatrix3D::sptr& _output);

    /**
     * @brief Multiply two matrices.
     * @param[in] trfA First matrix.
     * @param[in] trfB Second matrix.
     * @param[out] _output Output matrix.
     */
    FWDATATOOLS_API static void multiply(const ::fwData::TransformationMatrix3D::csptr& _trfA,
                                         const::fwData::TransformationMatrix3D::csptr& _trfB,
                                         ::fwData::TransformationMatrix3D::sptr& _output);

    /**
     * @brief Set the matrix to identity.
     * @param[in,out] _trf Matrix we want to set to identity.
     */
    FWDATATOOLS_API static void identity(::fwData::TransformationMatrix3D::sptr& _trf);

    /**
     * @brief Multiply a point and a matrix.
     * @param[in] _trf Matrix.
     * @param[in] _input Input point.
     * @param[out] _output Output point.
     */
    FWDATATOOLS_API static void multiply(const ::fwData::TransformationMatrix3D::csptr& _trf,
                                         const ::fwData::Point::csptr& _input, ::fwData::Point::sptr& _output);

    /**
     * @brief Multiply a point and a matrix.
     * @param[in] _trf input ::fwData::TransformationMatrix3D.
     * @return GLM matrix.
     */
    FWDATATOOLS_API static ::glm::dmat4x4 getMatrixFromTF3D(const ::fwData::TransformationMatrix3D::csptr& _trf);

    /**
     * @brief Multiply a point and a matrix.
     * @param[in] _trf output ::fwData::TransformationMatrix3D.
     * @param[out] _input Input GLM matrix.
     */
    FWDATATOOLS_API static void setTF3DFromMatrix(::fwData::TransformationMatrix3D::sptr& _trf,
                                                  const ::glm::dmat4x4& _input);
};

// ----------------------------------------------------------------------------

inline ::glm::dmat4x4 TransformationMatrix3D::getMatrixFromTF3D(const ::fwData::TransformationMatrix3D::csptr& _trf)
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

inline void TransformationMatrix3D::setTF3DFromMatrix(::fwData::TransformationMatrix3D::sptr& _trf,
                                                      const ::glm::dmat4x4& _input)
{
    // TransformationMatrix3D is stored row-major
    // glm matrices are stored column-major
    auto& coefs = _trf->getRefCoefficients();
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
} // namespace fwDataTools

#endif // __FWDATATOOLS_TRANSFORMATIONMATRIX3D_HPP__
