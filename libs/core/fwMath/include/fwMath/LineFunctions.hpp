/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWMATH_LINEFUNCTIONS_HPP__
#define __FWMATH_LINEFUNCTIONS_HPP__

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/config.hpp"

#include <fwCore/base.hpp>

namespace fwMath
{

//------------------------------------------------------------------------------

/**
 * @brief Compute the closest points between two rays.
 *  @param [in]  _ray1 ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _ray2 ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [out] _pointOnThis intersection point.
 *  @param [out] _pointOnfwLine barycenter of the triangle defined by the three points of the place.
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
FWMATH_API bool getClosestPoints( const fwLine& _ray1, const fwLine& _ray2,
                                  fwVec3d& _pointOnThis, fwVec3d& _pointOnfwLine);

/**
 * @brief Compute the projection of a point in a given direction.
 *  @param [in]  _ray ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _point point to be projected
 *  @return closest point of the line if an intersection is found.
 */
FWMATH_API fwVec3d getClosestPoint( const fwLine& _ray, const fwVec3d& _point);

/**
 * @brief Compute the projection of a point in a given direction and test if this intersection is inside a given radius.
 *  @param [in]  _ray ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _radius maximum distance of the point
 *  @param [in]  _point point to be projected
 *  @return closest point of the line if an intersection is found.
 */
FWMATH_API bool intersect(const fwLine& _ray, double _radius, const fwVec3d& _point);

/**
 * @brief Compute the closest points between two rays and test these points lie inside a sphere of a given radius.
 *  @param [in]  _line ray (origin,direction). Direction vector is assumed be normalized.
 *  @param [in]  _radius maximum distance of the point
 *  @param [in]  _origin origin of the second ray
 *  @param [in]  _direction direction of the second ray
 *  @param [in]  _point point to be projected
 *  @return closest point of the line if an intersection is found.
 */
FWMATH_API bool intersect(const fwLine& _line, double _radius, const fwVec3d& _origin, const fwVec3d& _direction,
                          fwVec3d& _point);

/**
 * @brief Give the intersection between a plane and a line. The result is returned in a point (_point).
 * @deprecated This function was added for a specific purpose and will be removed in a future release.
 *  @param [in]  _line input line (2 positions)
 *  @param [in]  _v1 first point of the plane
 *  @param [in]  _v2 second point of the plane
 *  @param [in]  _v3 third point of the plane
 *  @param [out] _point intersection point.
 *  @param [out] _barycentric barycenter of the triangle defined by the three points of the plane.
 *  @param [out] _front true if the dot product of the plane normal and ths positive Z axis (0,0,1) is positive.
 *  @return true if an intersection is found.
 */
FWMATH_API bool intersect( const fwLine& _line, const fwVec3d& _v1,  const fwVec3d& _v2, const fwVec3d& _v3,
                           fwVec3d& _point, fwVec3d& _barycentric, bool& _front);

}

#endif /* __FWMATH_LINEFUNCTIONS_HPP__ */
