/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMATH_PLANEFUNCTIONS_HPP_
#define _FWMATH_PLANEFUNCTIONS_HPP_

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/VectorFunctions.hpp"
#include "fwMath/LineFunctions.hpp"
#include "fwMath/export.hpp"
#include "fwMath/MatrixFunctions.hpp"

namespace fwMath {

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
 * @param [in] _point1 a point of the plan.
 * @param [in] _point2 a point of the plan.
 * @param [in] _point3 a point of the plan.
 * @param [out] the new plane.
 */
FWMATH_API void setValues(fwPlane& _plane, const fwVec3d & _point1, const fwVec3d & _point2, const fwVec3d & _point3);

/**
 * @brief Return the normal of the given plane _plane.
 * @param [in] _plane.
 * @return the normalize normal of the plane.
 */
FWMATH_API fwVec3d getNormal(const fwPlane& _plane);
/**
 * @brief Set the normal of the given plane _plane.
 *  @param [in] _normal.
 *  @param [in] _plane.
 */
FWMATH_API void  setNormal(fwPlane& _plane, const fwVec3d& _normal);
/**
 * @brief Get the distance from origin for the given plan (_plane).
 *  @param [in] _plane.
 *  @return the distance of origin of the plane.
 */
FWMATH_API double getDistance(const fwPlane& _plane);
/**
 * @brief Set the distance from origin (_distance) for the given plan (_plane).
 *  @param [in]  _distance.
 *  @param [in] _plane.
 *  @return [out] _plane.
 */
FWMATH_API void  setDistance(fwPlane& _plane, const double _distance);
/**
 *  @brief Give the intersection between a plane and a line. The result is returned in a point (_point)
 *  @param [in]  _fwPlane.
 *  @param [in]  _line.
 *  @param [out] _point intersection point.
 *  @return true if an intersection is found.
 */
FWMATH_API bool intersect( const fwPlane& _fwPlane, const fwLine & _line, fwVec3d & _point);

/**
 *  @brief Compute if a point is in a half plane.
 *  @param [in]  _plane.
 *  @param [in]  _point.
 *  @return true if point is in a half plane.
 */
FWMATH_API bool isInHalfSpace(const fwPlane& _plane, const fwVec3d& _point);

/**
 *  @brief Apply a transformation to a plane. The transformation is defined by a matrix 4x4.
 *  @param [in]  _plane
 *  @param [in]  _matrixs
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

}
#endif /* _FWMATH_PLANEFUNCTIONS_HPP_ */
