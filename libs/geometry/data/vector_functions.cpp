/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "geometry/data/vector_functions.hpp"

#include <core/exceptionmacros.hpp>
#include <core/macros.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sight::geometry::data
{

//------------------------------------------------------------------------------

double normalize(fwVec3d& _vec)
{
    const glm::dvec3 vec_glm        = glm::make_vec3<double>(_vec.data());
    const double length             = vec_length(_vec);
    const glm::dvec3 normalized_vec = glm::normalize(vec_glm);
    _vec[0] = normalized_vec[0];
    _vec[1] = normalized_vec[1];
    _vec[2] = normalized_vec[2];
    return length;
}

//------------------------------------------------------------------------------

fwVec3d normalized(const fwVec3d& _vec)
{
    fwVec3d normalized = _vec;
    normalize(normalized);
    return normalized;
}

//------------------------------------------------------------------------------

double dot(const fwVec3d& _v1, const fwVec3d& _v2)
{
    // Using GLM here is slower, especially in debug, so keep it simple
    return _v1[0] * _v2[0] + _v1[1] * _v2[1] + _v1[2] * _v2[2];
}

//------------------------------------------------------------------------------

fwVec3d cross(const fwVec3d& _v1, const fwVec3d& _v2)
{
    const glm::dvec3 vec_glm1 = glm::make_vec3<double>(_v1.data());
    const glm::dvec3 vec_glm2 = glm::make_vec3<double>(_v2.data());

    glm::dvec3 res = glm::cross(vec_glm1, vec_glm2);

    return {{res[0], res[1], res[2]}};
}

//------------------------------------------------------------------------------

double vec_length(const fwVec3d& _vec)
{
    glm::dvec3 vec_glm(_vec[0], _vec[1], _vec[2]);
    return glm::length(vec_glm);
}

//------------------------------------------------------------------------------

void negate(fwVec3d& _vec)
{
    _vec[0] = -_vec[0];
    _vec[1] = -_vec[1];
    _vec[2] = -_vec[2];
}

} //namespace sight::geometry::data

//------------------------------------------------------------------------------

fwVec3d& operator*=(fwVec3d& _vec1, double _val)
{
    glm::dvec3 vec_glm1(_vec1[0], _vec1[1], _vec1[2]);
    vec_glm1 *= _val;
    _vec1     = {{vec_glm1[0], vec_glm1[1], vec_glm1[2]}};

    return _vec1;
}

//------------------------------------------------------------------------------

fwVec3d& operator/=(fwVec3d& _vec, double _val)
{
    SIGHT_THROW_IF("Division by 0 not possible.", _val == 0.0);

    glm::dvec3 vec_glm1 = glm::make_vec3<double>(_vec.data());
    vec_glm1 /= _val;
    _vec      = {{vec_glm1[0], vec_glm1[1], vec_glm1[2]}};

    return _vec;
}

//------------------------------------------------------------------------------

fwVec3d& operator+=(fwVec3d& _vec1, const fwVec3d& _vec2)
{
    glm::dvec3 vec_glm1 = glm::make_vec3<double>(_vec1.data());
    glm::dvec3 vec_glm2 = glm::make_vec3<double>(_vec2.data());
    vec_glm1 += vec_glm2;
    _vec1     = {{vec_glm1[0], vec_glm1[1], vec_glm1[2]}};

    return _vec1;
}

//------------------------------------------------------------------------------

fwVec3d& operator-=(fwVec3d& _vec1, const fwVec3d& _vec2)
{
    glm::dvec3 vec_glm1 = glm::make_vec3<double>(_vec1.data());
    glm::dvec3 vec_glm2 = glm::make_vec3<double>(_vec2.data());
    vec_glm1 -= vec_glm2;
    _vec1     = {{vec_glm1[0], vec_glm1[1], vec_glm1[2]}};

    return _vec1;
}

//------------------------------------------------------------------------------

fwVec3d operator*(const fwVec3d& _vec, double _val)
{
    glm::dvec3 vec_glm1(_vec[0], _vec[1], _vec[2]);
    glm::dvec3 vec_glm = vec_glm1 * _val;

    return {{vec_glm[0], vec_glm[1], vec_glm[2]}};
}

//------------------------------------------------------------------------------

fwVec3d operator*(const double _val, const fwVec3d& _vec)
{
    return _vec * _val;
}

//------------------------------------------------------------------------------

fwVec3d operator/(const fwVec3d& _vec, double _val)
{
    SIGHT_THROW_IF("Division by 0 not possible.", _val == 0.0);

    glm::dvec3 vec_glm1(_vec[0], _vec[1], _vec[2]);
    glm::dvec3 vec_glm = vec_glm1 / _val;

    return {{vec_glm[0], vec_glm[1], vec_glm[2]}};
}

//------------------------------------------------------------------------------

fwVec3d operator+(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
    const glm::dvec3 vec_glm1 = glm::make_vec3<double>(_vec1.data());
    const glm::dvec3 vec_glm2 = glm::make_vec3<double>(_vec2.data());
    glm::dvec3 vec_glm        = vec_glm1 + vec_glm2;

    return {{vec_glm[0], vec_glm[1], vec_glm[2]}};
}

//------------------------------------------------------------------------------

fwVec3d operator-(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
    const glm::dvec3 vec_glm1 = glm::make_vec3<double>(_vec1.data());
    const glm::dvec3 vec_glm2 = glm::make_vec3<double>(_vec2.data());
    glm::dvec3 vec_glm        = vec_glm1 - vec_glm2;

    return {{vec_glm[0], vec_glm[1], vec_glm[2]}};
}

//------------------------------------------------------------------------------

bool operator==(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
    const glm::dvec3 vec_glm1 = glm::make_vec3<double>(_vec1.data());
    const glm::dvec3 vec_glm2 = glm::make_vec3<double>(_vec2.data());
    glm::bvec3 res            = glm::equal(vec_glm1, vec_glm2);

    return res[0] && res[1] && res[2];
}

//------------------------------------------------------------------------------

bool operator!=(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
    return !(_vec1 == _vec2);
}
