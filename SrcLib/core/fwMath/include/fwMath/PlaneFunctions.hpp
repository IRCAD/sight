#ifndef _FWMATH_PLANEFUNCTIONS_HPP_
#define _FWMATH_PLANEFUNCTIONS_HPP_

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/VectorFunctions.hpp"
#include "fwMath/LineFunctions.hpp"
#include "fwMath/export.hpp"
#include "fwMath/MatrixFunctions.hpp"

namespace fwMath {

FWMATH_API fwPlane getPlane(const fwVec3d & _point1, const fwVec3d & _point2, const fwVec3d & _point3);

FWMATH_API fwVec3d getNormal(const fwPlane& _plane);
FWMATH_API void  setNormal(fwPlane& _plane, const fwVec3d& _normal);
FWMATH_API double getDistance(const fwPlane& _plane);
FWMATH_API void  setDistance(fwPlane& _plane, double _distance);
/**
 * Give the intersection between a plane and a line. The result is returned in a point (_point)
 *  @param [in]  _fwPlane
 *  @param [in]  _line
 *  @param [out] _point intersection point.
 *  @return true if a intersection is found.
 */
FWMATH_API bool intersect( const fwPlane& _fwPlane, const fwLine & _line, fwVec3d & _point);

FWMATH_API bool isInHalfSpace(const fwPlane& _plane, const fwVec3d& _point);

FWMATH_API void transform(fwPlane& _plane, const fwMatrix& _matrix);

}
#endif /* _FWMATH_PLANEFUNCTIONS_HPP_ */
