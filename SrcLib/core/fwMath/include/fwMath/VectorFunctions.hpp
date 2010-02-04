/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMATH_VECTORFUNCTIONS_HPP_
#define _FWMATH_VECTORFUNCTIONS_HPP_

#include <cmath>

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"

namespace fwMath {

/**
 *  @brief Normalize a vector
 *  @param [in]
 *  @param [in]
 *  @return
 */
FWMATH_API double normalize(fwVec3d & vec);

/**
 *  @brief Retrun a normalized vector
 *  @param [in] vector to normalize
 *  @return normalized vector
 */
FWMATH_API fwVec3d normalized(fwVec3d & vec);

/**
 *  @brief Compute the Dot product between two vectors.
 *  @param [in] v1
 *  @param [in] v2
 *  @return the dot product.
 */
FWMATH_API double dot(const fwVec3d& v1, const fwVec3d& v2);
/**
 *  @brief Compute the Cross product between two vectors.
 *  @param [in] v1
 *  @param [in] v2
 *  @return the cross product.
 */
FWMATH_API fwVec3d cross(const fwVec3d& v1, const fwVec3d& v2);

/**
 *  @brief Compute the length of the vector
 *  @param [in] _vec
 *  @return the vector's length
 */
FWMATH_API double vecLength(fwVec3d &_vec);

/**
 *  @brief Compute of the vector.
 *  @param [in] _vec
 */
//FWMATH_API void setPosition(fwVec3d &_vec);

/**
 *  @brief Compute the negate of the vector.
 *  @param [in] _vec
 *  @param [out] _vec
 */
FWMATH_API void negate(fwVec3d &_vec);

}

/**
 *  @brief operator *= for fwVec3d
 *  @param [in]
 *  @param [in]
 *  @return vec1[0]*=vec2[0], vec1[1]*=vec2[1], vec1[2]*=vec2[2]
 */
FWMATH_API fwVec3d& operator*=(fwVec3d& vec1, double val);

/**
 *  @brief operator /= for fwVec3d
 *  @param [in] vec
 *  @param [in] val
 *  @return vec[0]/=val, vec[1]/=val, vec[2]/=val
 */
FWMATH_API fwVec3d& operator/=(fwVec3d& vec,  double val );
/**
 *  @brief operator += for fwVec3d
 *  @param [in] vec1
 *  @param [in] vec2
 *  @return  vec1 => vec1[0]+=vec2[0], vec1[1]+=vec2[1], vec1[2]+=vec2[2]
 */
FWMATH_API fwVec3d& operator+=(fwVec3d& vec1, fwVec3d& vec2);
/**
 *  @brief
 *  @param [in] vec1
 *  @param [in] vec2
 *  @return vec1[0]-=vec2[0], vec1[1]-=vec2[1], vec1[2]-=vec2[2]
 */
FWMATH_API fwVec3d& operator-=(fwVec3d& vec1, fwVec3d& vec2);
/**
 *  @brief operator * between fwVec3d and value.
 *  @param [in] _vec
 *  @param [in] _val
 *  @return the result of the operation _vec*val
 */
FWMATH_API fwVec3d operator*(const fwVec3d& _vec, const double _val);
/**
 *  @brief operator * between value and fwVec3d.
 *  @param [in] _val
 *  @param [in] _vec
 *  @return the resut of the operation _vec*val
 */
FWMATH_API fwVec3d operator*(const double _val, const fwVec3d& _vec);
/**
 *  @brief operator / between fwVec3d and value.
 *  @param [in] _vec
 *  @param [in] val
 *  @return the resut of the operation _vec/val
 */
FWMATH_API fwVec3d operator/(const fwVec3d& _vec, double val);
/**
 *  @brief operator + between tawo fwVec3d.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return the result of the operation _vec1 + _vec2
 */
FWMATH_API fwVec3d operator+(const fwVec3d& _vec1, const fwVec3d& _vec2);
/**
 *  @brief operator - between two fwVec3d datas.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return the difference between two value.
 */
FWMATH_API fwVec3d operator-(const fwVec3d& _vec1, const fwVec3d& _vec2);
/**
 *  @brief Compare two fwVec3d datas.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return return 1 if equal 0 otherwise.
 */
FWMATH_API int operator==(const fwVec3d& _vec1, const fwVec3d& _vec2);
/**
 *  @brief Compare two fwVec3d data.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return 1 if different.
 */
FWMATH_API int operator!=(const fwVec3d& _vec1, const fwVec3d& _vec2);

#endif /* _FWMATH_VECTORFUNCTIONS_HPP_ */
