#ifndef _FWMATH_VECTORFUNCTIONS_HPP_
#define _FWMATH_VECTORFUNCTIONS_HPP_

#include <math.h>

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"

namespace fwMath {

FWMATH_API double normalize(fwVec3d & vec);

FWMATH_API double dot(const fwVec3d& v1, const fwVec3d& v2);
FWMATH_API fwVec3d cross(const fwVec3d& v1, const fwVec3d& v2);

FWMATH_API double vecLength(fwVec3d &_vec);

FWMATH_API void setPosition(fwVec3d &_vec);

FWMATH_API void negate(fwVec3d &_vec);

}

FWMATH_API fwVec3d& operator*=(fwVec3d& vec1, double val);
FWMATH_API fwVec3d& operator/=(fwVec3d& vec,  double val );
FWMATH_API fwVec3d& operator+=(fwVec3d& vec1, fwVec3d& vec2);
FWMATH_API fwVec3d& operator-=(fwVec3d& vec1, fwVec3d& vec2);
FWMATH_API fwVec3d operator*(const fwVec3d& _point, const double _val);
FWMATH_API fwVec3d operator*(const double _val, const fwVec3d& _point);
FWMATH_API fwVec3d operator/(const fwVec3d& _point, double);
FWMATH_API fwVec3d operator+(const fwVec3d& _point1, const fwVec3d& _point2);
FWMATH_API fwVec3d operator-(const fwVec3d& _point1, const fwVec3d& _point2);
FWMATH_API int operator==(const fwVec3d& _point1, const fwVec3d& _point2);
FWMATH_API int operator!=(const fwVec3d& _point1, const fwVec3d& _point2);

#endif /* _FWMATH_VECTORFUNCTIONS_HPP_ */
