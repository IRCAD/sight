/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMATH_LINEFUNCTIONS_HPP_
#define _FWMATH_LINEFUNCTIONS_HPP_

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"

namespace fwMath {

//------------------------------------------------------------------------------

/**
 * @brief Compute the closest points between two lines.
 * Return FALSE if the lines are parallel, TRUE otherwise.
 * @verbatim
  p1 + t1 * d1
  p2 + t2 * d2
  (p2 - p1 + t2 * d2 - t1 * d1) * d1 = 0
  (p2 - p1 + t2 * d2 - t1 * d1) * d2 = 0
  t2 * (d2.d1) - t1 = (p1 - p2).d1
  t2 - t1 * (d1.d2) = (p1 - p2).d2

  delta = 1 - (d1.d2)**2

  t2 = [ d2.(p1-p2) - d1.(p1-p2) * (d1.d2)]/delta
  t1 = [-d1.(p1-p2) + d2.(p1-p2) * (d1.d2)]/delta
 @endverbatim
 */
FWMATH_API bool getClosestPoints( const fwLine& _line1, const fwLine& _line2, fwVec3d& _pointOnThis, fwVec3d& _pointOnfwLine);

/**
 * @brief
 */
FWMATH_API fwVec3d getClosestPoint( const fwLine& _line, fwVec3d& _point);

/**
 * @brief
 */
FWMATH_API bool intersect(const fwLine& _line, double _radius, fwVec3d _point) ;

/**
 * @brief
 */
FWMATH_API bool intersect(const fwLine& _line, double _radius,fwVec3d _vec0, fwVec3d _vec1, fwVec3d _point);

/**
 * @brief
 */
FWMATH_API bool intersect( const fwLine& _line, const fwVec3d &_v1,  const fwVec3d &_v2, const fwVec3d &_v3, fwVec3d &_point, fwVec3d &_barycentric, bool& _front);

}

#endif /* _FWMATH_LINEFUNCTIONS_HPP_ */
