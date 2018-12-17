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

#include "fwMath/VectorFunctions.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fwMath
{

//------------------------------------------------------------------------------

double normalize(fwVec3d& vec)
{
    const ::glm::dvec3 vecGlm        = ::glm::make_vec3<double>(vec.data());
    const double length              = vecLength(vec);
    const ::glm::dvec3 normalizedVec = ::glm::normalize(vecGlm);
    vec[0] = normalizedVec[0];
    vec[1] = normalizedVec[1];
    vec[2] = normalizedVec[2];
    return length;
}

//------------------------------------------------------------------------------

fwVec3d normalized(const fwVec3d& vec)
{
    fwVec3d normalized = vec;
    normalize(normalized);
    return normalized;
}

//------------------------------------------------------------------------------

double dot(const fwVec3d& v1, const fwVec3d& v2)
{
    // Using GLM here is slower, especially in debug, so keep it simple
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

//------------------------------------------------------------------------------

fwVec3d cross(const fwVec3d& v1, const fwVec3d& v2)
{
    const ::glm::dvec3 vecGlm1 = ::glm::make_vec3<double>(v1.data());
    const ::glm::dvec3 vecGlm2 = ::glm::make_vec3<double>(v2.data());

    ::glm::dvec3 res = ::glm::cross(vecGlm1, vecGlm2);

    return {{res[0], res[1], res[2]}};
}

//------------------------------------------------------------------------------

double vecLength(const fwVec3d &vec)
{
    ::glm::dvec3 vecGLM (vec[0], vec[1], vec[2]);
    return ::glm::length(vecGLM);
}

//------------------------------------------------------------------------------

void negate(fwVec3d &vec)
{
    vec[0] = -vec[0];
    vec[1] = -vec[1];
    vec[2] = -vec[2];
}
} //namespace fwMath

//------------------------------------------------------------------------------

fwVec3d& operator*=(fwVec3d& vec1, double val)
{
    ::glm::dvec3 vecGlm1(vec1[0], vec1[1], vec1[2]);
    vecGlm1 *= val;
    vec1     = {{vecGlm1[0], vecGlm1[1], vecGlm1[2]}};

    return vec1;
}

//------------------------------------------------------------------------------

fwVec3d& operator/=(fwVec3d& vec, double val )
{
    FW_RAISE_IF("Division by 0 not possible.", val == 0.0);

    ::glm::dvec3 vecGlm1 = ::glm::make_vec3<double>(vec.data());
    vecGlm1             /= val;
    vec                  = {{vecGlm1[0], vecGlm1[1], vecGlm1[2]}};

    return vec;
}

//------------------------------------------------------------------------------

fwVec3d& operator+=(fwVec3d& vec1, const fwVec3d& vec2)
{
    ::glm::dvec3 vecGlm1 = ::glm::make_vec3<double>(vec1.data());
    ::glm::dvec3 vecGlm2 = ::glm::make_vec3<double>(vec2.data());
    vecGlm1             += vecGlm2;
    vec1                 = {{vecGlm1[0], vecGlm1[1], vecGlm1[2]}};

    return vec1;
}

//------------------------------------------------------------------------------

fwVec3d& operator-=(fwVec3d& vec1, const fwVec3d& vec2)
{
    ::glm::dvec3 vecGlm1 = ::glm::make_vec3<double>(vec1.data());
    ::glm::dvec3 vecGlm2 = ::glm::make_vec3<double>(vec2.data());
    vecGlm1             -= vecGlm2;
    vec1                 = {{vecGlm1[0], vecGlm1[1], vecGlm1[2]}};

    return vec1;
}

//------------------------------------------------------------------------------

fwVec3d operator*(const fwVec3d& vec, double val)
{
    ::glm::dvec3 vecGlm1(vec[0], vec[1], vec[2]);
    ::glm::dvec3 vecGlm = vecGlm1 * val;

    return {{vecGlm[0], vecGlm[1], vecGlm[2]}};
}

//------------------------------------------------------------------------------

fwVec3d operator*(const double val, const fwVec3d& vec)
{
    return vec * val;
}

//------------------------------------------------------------------------------

fwVec3d operator/(const fwVec3d& vec, double val)
{
    FW_RAISE_IF("Division by 0 not possible.", val == 0.0);

    ::glm::dvec3 vecGlm1(vec[0], vec[1], vec[2]);
    ::glm::dvec3 vecGlm = vecGlm1 / val;

    return {{vecGlm[0], vecGlm[1], vecGlm[2]}};
}

//------------------------------------------------------------------------------

fwVec3d operator+(const fwVec3d& vec1, const fwVec3d& vec2)
{
    const ::glm::dvec3 vecGlm1 = ::glm::make_vec3<double>(vec1.data());
    const ::glm::dvec3 vecGlm2 = ::glm::make_vec3<double>(vec2.data());
    ::glm::dvec3 vecGlm = vecGlm1 + vecGlm2;

    return {{vecGlm[0], vecGlm[1], vecGlm[2]}};
}

//------------------------------------------------------------------------------

fwVec3d operator-(const fwVec3d& vec1, const fwVec3d& vec2)
{
    const ::glm::dvec3 vecGlm1 = ::glm::make_vec3<double>(vec1.data());
    const ::glm::dvec3 vecGlm2 = ::glm::make_vec3<double>(vec2.data());
    ::glm::dvec3 vecGlm = vecGlm1 - vecGlm2;

    return {{vecGlm[0], vecGlm[1], vecGlm[2]}};
}

//------------------------------------------------------------------------------

int operator==(const fwVec3d& vec1, const fwVec3d& vec2)
{
    const ::glm::dvec3 vecGlm1 = ::glm::make_vec3<double>(vec1.data());
    const ::glm::dvec3 vecGlm2 = ::glm::make_vec3<double>(vec2.data());
    ::glm::bvec3 res = ::glm::equal(vecGlm1, vecGlm2);

    return res[0] && res[1] && res[2];
}

//------------------------------------------------------------------------------

int operator!=(const fwVec3d& vec1, const fwVec3d& vec2)
{
    return !(vec1 == vec2);
}

