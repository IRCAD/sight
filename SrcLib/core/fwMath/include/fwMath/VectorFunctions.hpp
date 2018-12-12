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

#ifndef __FWMATH_VECTORFUNCTIONS_HPP__
#define __FWMATH_VECTORFUNCTIONS_HPP__

#include <fwCore/base.hpp>

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/config.hpp"

namespace fwMath
{

/**
 *  @brief Normalize a vector
 */
FWMATH_API double normalize(fwVec3d & vec);

/**
 *  @brief Return a normalized vector
 *  @param [in] vec vector to normalize
 *  @return normalized vector
 */
FWMATH_API fwVec3d normalized(const fwVec3d & vec);

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
FWMATH_API double vecLength(const fwVec3d &_vec);

/**
 *  @brief Compute the negate of the vector.
 *  @param [in] _vec
 *  @param [out] _vec
 */
FWMATH_API void negate(fwVec3d &_vec);

} //namespace fwMath

/**
 *  @brief operator *= for fwVec3d
 *  @param [in] vec1 vector to multiply
 *  @param [in] val value to multiply with vector
 *  @return vec1[0]*=vec2[0], vec1[1]*=vec2[1], vec1[2]*=vec2[2]
 */
FWMATH_API fwVec3d& operator*=(fwVec3d& vec1, double val);

/**
 *  @brief operator /= for fwVec3d
 *  @param [in] vec
 *  @param [in] val
 *  @return vec[0]/=val, vec[1]/=val, vec[2]/=val
 */
FWMATH_API fwVec3d& operator/=(fwVec3d& vec, double val );

/**
 *  @brief operator += for fwVec3d
 *  @param [in] vec1
 *  @param [in] vec2
 *  @return  vec1 => vec1[0]+=vec2[0], vec1[1]+=vec2[1], vec1[2]+=vec2[2]
 */
FWMATH_API fwVec3d& operator+=(fwVec3d& vec1, const fwVec3d& vec2);

/**
 *  @brief
 *  @param [in] vec1
 *  @param [in] vec2
 *  @return vec1[0]-=vec2[0], vec1[1]-=vec2[1], vec1[2]-=vec2[2]
 */
FWMATH_API fwVec3d& operator-=(fwVec3d& vec1, const fwVec3d& vec2);

/**
 *  @brief operator * between fwVec3d and value.
 *  @param [in] _vec
 *  @param [in] _val
 *  @return the result of the operation _vec*val
 */
FWMATH_API fwVec3d operator*(const fwVec3d& _vec, double _val);

/**
 *  @brief operator * between value and fwVec3d.
 *  @param [in] _val
 *  @param [in] _vec
 *  @return the result of the operation _vec*val
 */
FWMATH_API fwVec3d operator*(const double _val, const fwVec3d& _vec);

/**
 *  @brief operator / between fwVec3d and value.
 *  @param [in] _vec
 *  @param [in] val
 *  @return the result of the operation _vec/val
 */
FWMATH_API fwVec3d operator/(const fwVec3d& _vec, double val);

/**
 *  @brief operator + between two fwVec3d.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return the result of the operation _vec1 + _vec2
 */
FWMATH_API fwVec3d operator+(const fwVec3d& _vec1, const fwVec3d& _vec2);

/**
 *  @brief operator - between two fwVec3d data.
 *  @param [in] _vec1
 *  @param [in] _vec2
 *  @return the difference between two value.
 */
FWMATH_API fwVec3d operator-(const fwVec3d& _vec1, const fwVec3d& _vec2);

/**
 *  @brief Compare two fwVec3d data.
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


#endif /* __FWMATH_VECTORFUNCTIONS_HPP__ */
