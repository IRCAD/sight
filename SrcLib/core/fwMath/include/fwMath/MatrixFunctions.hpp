#ifndef _FWMATH_MATRIXFUNCTIONS_HPP_
#define _FWMATH_MATRIXFUNCTIONS_HPP_

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"
namespace fwMath {

FWMATH_API void multVecMatrix(const fwMatrix & _matrix, const fwVec3d& _source ,fwVec3d& _dest);
}
#endif /* _FWMATH_MATRIXFUNCTIONS_HPP_ */
