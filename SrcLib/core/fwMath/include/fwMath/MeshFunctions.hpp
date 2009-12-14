/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMATH_MESHFUNCTIONS_HPP_
#define _FWMATH_MESHFUNCTIONS_HPP_

#include <math.h>
#include <vector>

#include <fwCore/base.hpp>
#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"


namespace fwMath
{

/**
 * @brief
 */
FWMATH_API bool IsInclosedVolume(const fwVertexPosition _vertex, const fwVertexIndex _vertexIndex, const fwVec3d &_P );

/**
 * @brief Compute the intersection between triangle(define by threes vertex vert1, vert2, vert3) and the Oz parallel line and cross by the point P.
 * The function return true if intersection is found (false otherwise).
 * t is the oriented distance of P to the intersection point.
 * u and v is the coordinate of the intersection point in the (O, u, v) triangle coordinate system with :
 * O = vert1. u =  vert2 - O. v =  vert3 - O.
 * "Fast, Minimum Storage Ray/Triangle Intersection", Tomas Möller, Ben Trumbore.
 *
 */
FWMATH_API bool intersect_triangle(fwVec3d _orig, fwVec3d _dir, fwVec3d _vert0, fwVec3d _vert1, fwVec3d _vert2, double &_t, double &_u, double &_v);
}

#endif /* _FWMATH_MESHFUNCTIONS_HPP_ */
