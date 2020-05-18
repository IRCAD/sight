/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWMATH_PLANEFUNCTIONS_HPP__
#define __FWMATH_PLANEFUNCTIONS_HPP__


#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/VectorFunctions.hpp"
#include "fwMath/LineFunctions.hpp"
#include "fwMath/config.hpp"
#include "fwMath/MatrixFunctions.hpp"

#include <fwCore/base.hpp>

namespace fwMath
{

static const double EPSILON = 0.001;

/**
 * @brief
 */
FWMATH_API fwPlane getPlane(const fwVec3d & _point1, const fwVec3d & _point2, const fwVec3d & _point3);
/**
 * @brief compute a plane from a normal and a point which must be in the plane.
 * @param [in] _point a point of the plan/
 * @param [in] _normal the normal of the new plane.
 * @return the new plane.
 */
FWMATH_API fwPlane getPlane(const fwVec3d&  _normal,const fwVec3d& _point);

/**
 * @brief Initialize a plane _plane with three points (_point1, _point2, _point3).
 * It computes the plane's normal and the distance from the origin.
 * @param [out] _plane the new plane.
 * @param [in] _point1 a point of the plan.
 * @param [in] _point2 a point of the plan.
 * @param [in] _point3 a point of the plan.
 */
FWMATH_API void setValues(fwPlane& _plane, const fwVec3d & _point1, const fwVec3d & _point2, const fwVec3d & _point3);

/**
 * @brief Return the normal of the given plane _plane.
 * @param [in] _plane
 * @return the normalize normal of the plane.
 */
FWMATH_API fwVec3d getNormal(const fwPlane& _plane);
/**
 * @brief Set the normal of the given plane _plane.
 *  @param [in] _plane
 *  @param [in] _normal
 */
FWMATH_API void  setNormal(fwPlane& _plane, const fwVec3d& _normal);
/**
 * @brief Get the distance from origin for the given plan (_plane).
 *  @param [in] _plane
 *  @return the distance of origin of the plane.
 */
FWMATH_API double getDistance(const fwPlane& _plane);
/**
 * @brief Set the distance from origin (_distance) for the given plan (_plane).
 *  @param [in]  _distance
 *  @param [in] _plane
 *  @return [out] _plane
 */
FWMATH_API void  setDistance(fwPlane& _plane, const double _distance);
/**
 *  @brief Give the intersection between a plane and a line. The result is returned in a point (_point)
 *  @param [in]  _fwPlane
 *  @param [in]  _line
 *  @param [out] _point intersection point.
 *  @return true if an intersection is found.
 */
FWMATH_API bool intersect( const fwPlane& _fwPlane, const fwLine & _line, fwVec3d& _point);

/**
 *  @brief Compute if a point is in a half plane.
 *  @param [in]  _plane
 *  @param [in]  _point
 *  @return true if point is in a half plane.
 */
FWMATH_API bool isInHalfSpace(const fwPlane& _plane, const fwVec3d& _point);

/**
 *  @brief Apply a transformation to a plane. The transformation is defined by a matrix 4x4.
 *  @param [in]  _plane
 *  @param [in]  _matrix
 *  @param [out] _plane
 */
FWMATH_API void transform(fwPlane& _plane, const fwMatrix4x4& _matrix);

/**
 *  @brief Add an offset at the distance of origin which define the plane (_plane).
 *  @param [in]  _plane
 *  @param [in]  _offset
 *  @param [out] _plane
 */
FWMATH_API void offset(fwPlane& _plane, double _offset);

} // namespace fwMath

/**
 *  @brief Compares if two planes are equal
 *  @param [in]  plane1
 *  @param [in]  plane2
 *  @return true if the planes are equal and false if they are not.
 */
FWMATH_API bool operator==(fwPlane& plane1, fwPlane& plane2);

#endif /* __FWMATH_PLANEFUNCTIONS_HPP__ */
