/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_MATRIXFUNCTIONS_HPP__
#define __FWMATH_MATRIXFUNCTIONS_HPP__

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/config.hpp"

namespace fwMath
{

/**
 * @brief Multiply a matrix by a vector
 *  @param [in]  _matrix
 *  @param [in]  _source
 *  @param [out] _dest
 *
 */
FWMATH_API void multVecMatrix(const fwMatrix4x4 & _matrix, const fwVec3d& _source,fwVec3d& _dest);

/**
 * @brief Compute a matrix rotation
 * @deprecated The function implementation seems very dodgy, prefer ::glm::yawPitchRoll() or ::glm::orientate4()
 *  @param [in]  _vecNorm
 *  @return rotation matrix
 *
 */
FWMATH_API fwMatrix4x4  getRotationMatrix( const fwVec3d &_vecNorm );

/**
 * @brief Computes the inverse of a matrix
 *  @param [in]  matrix
 *  @return inverted matrix
 *
 */
FWMATH_API fwMatrix4x4  getInverse( const fwMatrix4x4 & matrix );

} // namespace fwMath

/**
 * @brief Multiplies a matrix by a matrix
 *  @param [in]  matrix1
 *  @param [in]  matrix2
 *  @return matrix
 *
 */
FWMATH_API fwMatrix4x4 operator*( const fwMatrix4x4 & matrix1, const fwMatrix4x4 & matrix2 );

#endif /* __FWMATH_MATRIXFUNCTIONS_HPP__ */
