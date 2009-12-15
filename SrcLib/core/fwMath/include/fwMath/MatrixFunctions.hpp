/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMATH_MATRIXFUNCTIONS_HPP_
#define _FWMATH_MATRIXFUNCTIONS_HPP_

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"

namespace fwMath {

/**
 * @brief Multiply amatrix by a vector
 *  @param [in]  _matrix
 *  @param [in]  _source
 *  @param [out] _dest
 *
 */
FWMATH_API void multVecMatrix(const fwMatrix4x4 & _matrix, const fwVec3d& _source ,fwVec3d& _dest);
/**
 * @brief Compute a matrix rotation
 *  @param [in]  _vecNorm
 *  @return rotation matrix
 *
 */

FWMATH_API fwMatrix4x4  getRotationMatrix( const fwVec3d &_vecNorm );
}
#endif /* _FWMATH_MATRIXFUNCTIONS_HPP_ */
