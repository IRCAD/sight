#ifndef _FWMATH_MATRIXFUNCTIONS_HPP_
#define _FWMATH_MATRIXFUNCTIONS_HPP_

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"

namespace fwMath {

FWMATH_API void multVecMatrix(const fwMatrix4x4 & _matrix, const fwVec3d& _source ,fwVec3d& _dest);

FWMATH_API fwMatrix4x4  getRotationMatrix( const fwVec3d &_vecNorm );
}
#endif /* _FWMATH_MATRIXFUNCTIONS_HPP_ */
