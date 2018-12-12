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

#include "fwMath/PlaneFunctions.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>


#define EPSILON 0.00000001

namespace fwMath
{

//------------------------------------------------------------------------------

fwPlane getPlane(const fwVec3d & point1, const fwVec3d & point2, const fwVec3d & point3)
{
    fwPlane plane;
    setValues(plane, point1, point2, point3);

    return plane;
}

//------------------------------------------------------------------------------

void setValues(fwPlane& plane, const fwVec3d & point1, const fwVec3d & point2, const fwVec3d & point3)
{
    ::glm::dvec3 p1(point1[0], point1[1], point1[2]);
    ::glm::dvec3 p2(point2[0], point2[1], point2[2]);
    ::glm::dvec3 p3(point3[0], point3[1], point3[2]);

    ::glm::dvec3 normal = ::glm::cross(p2 - p1, p3 - p1);
    if(::glm::length(normal) <= 0.0)
    {
        normal[0] = 0.0;
        normal[1] = 0.0;
        normal[2] = 1.0;
    }
    normal = ::glm::normalize(normal);
    double distance = ::glm::dot(normal, p1);

    plane[0] = normal[0];
    plane[1] = normal[1];
    plane[2] = normal[2];
    plane[3] = distance;
}

//------------------------------------------------------------------------------

fwVec3d getNormal(const fwPlane& plane)
{
    return {{plane[0], plane[1], plane[2]}};
}

//------------------------------------------------------------------------------

void setNormal(fwPlane& plane, const fwVec3d& normal)
{
    ::glm::dvec3 vecNormal(normal[0], normal[1], normal[2]);
    vecNormal = ::glm::normalize(vecNormal);

    plane[0] = vecNormal[0];
    plane[1] = vecNormal[1];
    plane[2] = vecNormal[2];
}

//------------------------------------------------------------------------------

double getDistance(const fwPlane& plane)
{
    return plane[3];
}

//------------------------------------------------------------------------------

void  setDistance(fwPlane& plane, const double distance)
{
    plane[3] = distance;
}

//------------------------------------------------------------------------------

bool intersect( const fwPlane& plane, const fwLine & line,  fwVec3d & point)
{
    ::glm::dvec3 normal(plane[0], plane[1], plane[2]);
    normal = ::glm::normalize(normal);
    ::glm::dvec3 lineDirection(line.second[0] - line.first[0],
                               line.second[1] - line.first[1],
                               line.second[2] - line.first[2]);
    lineDirection = ::glm::normalize(lineDirection);
    ::glm::dvec3 lineOrigin(line.first[0], line.first[1], line.first[2]);

    double intersectionDistance = 0.;
    double d                    = ::glm::dot(lineDirection, normal);

    if(std::abs(d) < EPSILON)
    {
        return false;
    }

    intersectionDistance = (plane[3] - ::glm::dot(normal, lineOrigin)) / d;

    lineOrigin += lineDirection * intersectionDistance;
    point[0]    = lineOrigin[0];
    point[1]    = lineOrigin[1];
    point[2]    = lineOrigin[2];

    return true;
}

//------------------------------------------------------------------------------

bool isInHalfSpace(const fwPlane& plane, const fwVec3d& point)
{
    ::glm::dvec3 pointGlm(point[0], point[1], point[2]);
    ::glm::dvec3 normal(plane[0], plane[1], plane[2]);
    ::glm::normalize(normal);
    ::glm::dvec3 pos = normal * plane[3];
    return (::glm::dot(normal, pointGlm-pos) >= 0.0);
}

//------------------------------------------------------------------------------

void transform(fwPlane& plane, const fwMatrix4x4& matrix)
{
    ::glm::dvec3 normal(plane[0], plane[1], plane[2]);
    ::glm::dvec3 beg(normal * plane[3]);
    ::glm::dvec3 end(beg + normal);
    ::glm::dvec4 beg4(beg, 1.0);
    ::glm::dvec4 end4(end, 1.0);
    ::glm::dmat4x4 mat(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
                       matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
                       matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
                       matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);

    beg4 = mat * beg4;
    end4 = mat * end4;

    end[0] = end4[0];
    end[1] = end4[1];
    end[2] = end4[2];

    beg[0] = beg4[0];
    beg[1] = beg4[1];
    beg[2] = beg4[2];

    normal = end - beg;
    normal = ::glm::normalize(normal);

    plane[0] = normal[0];
    plane[1] = normal[1];
    plane[2] = normal[2];
    plane[3] = ::glm::dot(normal, beg);
}

//------------------------------------------------------------------------------

void offset(fwPlane& plane, double offset)
{
    double distance = getDistance(plane);
    distance += offset;
    setDistance(plane, distance);
}

//------------------------------------------------------------------------------

fwPlane getPlane(const fwVec3d&  normal,const fwVec3d& point)
{
    ::glm::dvec3 pointGlm(point[0], point[1], point[2]);
    ::glm::dvec3 normalGlm(normal[0], normal[1], normal[2]);
    normalGlm = ::glm::normalize(normalGlm);
    fwPlane plane;
    plane[0] = normalGlm[0];
    plane[1] = normalGlm[1];
    plane[2] = normalGlm[2];
    plane[3] = ::glm::dot(normalGlm, pointGlm);
    return plane;
}

} // namespace fwMath

//------------------------------------------------------------------------------

bool operator==(fwPlane& plane1, fwPlane& plane2)
{
    ::glm::dvec4 pl1(plane1[0], plane1[1], plane1[2], plane1[3]);
    ::glm::dvec4 pl2(plane2[0], plane2[1], plane2[2], plane2[3]);

    double dx = pl1[0] - pl2[0];
    double dy = pl1[1] - pl2[1];
    double dz = pl1[2] - pl2[2];
    double dd = pl1[3] - pl2[3];

    return ( std::abs(dx) < EPSILON &&
             std::abs(dy) < EPSILON &&
             std::abs(dz) < EPSILON &&
             std::abs(dd) < EPSILON );
}

//------------------------------------------------------------------------------
