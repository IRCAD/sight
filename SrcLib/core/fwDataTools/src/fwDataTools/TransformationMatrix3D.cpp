/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/TransformationMatrix3D.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>

namespace fwDataTools
{

//------------------------------------------------------------------------------

bool TransformationMatrix3D::invert(const ::fwData::TransformationMatrix3D::csptr& _input,
                                    ::fwData::TransformationMatrix3D::sptr& _output)
{
    // Normally we should transpose matrices since GLM uses a column-major layout and FW4SPL uses row-major layout
    // However the transposition has a cost and inversion does not care about the layout, so we skip it
    const ::glm::dmat4x4 mat        = ::glm::make_mat4<double>(_input->getCoefficients().data());
    const ::glm::dmat4x4 matInverse = ::glm::inverse(mat);

    auto& coefs = _output->getCoefficients();
    for (size_t i = 0; i < 4; ++i)
    {
        const size_t rowDst          = i * 4;
        const ::glm::length_t rowSrc = static_cast< ::glm::length_t>(i);
        for (size_t j = 0; j < 4; ++j)
        {
            const ::glm::length_t colSrc = static_cast< ::glm::length_t>(j);
            coefs[rowDst+j] = matInverse[rowSrc][colSrc];
        }
    }

    return true;
}

// ----------------------------------------------------------------------------

void TransformationMatrix3D::multiply(const ::fwData::TransformationMatrix3D::csptr& _trfA,
                                      const ::fwData::TransformationMatrix3D::csptr& _trfB,
                                      ::fwData::TransformationMatrix3D::sptr& _output)
{
    // Normally we should transpose matrices since GLM uses a column-major layout and FW4SPL uses row-major layout
    // However the transposition has a cost, so it is faster to not transpose them
    // and perform the inverse multiplication
    const ::glm::dmat4x4 matA = ::glm::make_mat4<double>(_trfA->getCoefficients().data());
    const ::glm::dmat4x4 matB = ::glm::make_mat4<double>(_trfB->getCoefficients().data());

    const ::glm::dmat4x4 matC = matB * matA;

    auto& coefs = _output->getCoefficients();
    for (size_t i = 0; i < 4; ++i)
    {
        const size_t rowDst          = i * 4;
        const ::glm::length_t rowSrc = static_cast< ::glm::length_t>(i);
        for (size_t j = 0; j < 4; ++j)
        {
            const ::glm::length_t colSrc = static_cast< ::glm::length_t>(j);
            coefs[rowDst + j] = matC[rowSrc][colSrc];
        }
    }
}

// ----------------------------------------------------------------------------

void TransformationMatrix3D::identity(::fwData::TransformationMatrix3D::sptr& _trf)
{
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            _trf->setCoefficient(i, j, i == j ? 1 : 0);
        }
    }
}

// ----------------------------------------------------------------------------

void TransformationMatrix3D::multiply(const ::fwData::TransformationMatrix3D::csptr& _trf,
                                      const ::fwData::Point::csptr& _input, ::fwData::Point::sptr& _output)
{
    // Normally we should transpose matrices since GLM uses a column-major layout and FW4SPL uses row-major layout
    // However the transposition has a cost, so it is faster to not transpose them
    // and perform the inverse multiplication
    const ::glm::dmat4x4 mat = ::glm::make_mat4<double>(_trf->getCoefficients().data());

    const auto& inCoord = _input->getCoord();
    ::glm::dvec4 in;
    in[0] = inCoord[0];
    in[1] = inCoord[1];
    in[2] = inCoord[2];
    in[3] = 1;

    ::glm::dvec4 out = in * mat;
    std::array<double, 3> res = {{ out[0], out[1], out[2] }};
    _output->setCoord(res);
}

// ----------------------------------------------------------------------------

} // namespace fwDataTools
