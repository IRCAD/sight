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

#include "geometry/data/plane_functions.hpp"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

constexpr double EPSILON = 0.00000001;

namespace sight::geometry::data
{

//------------------------------------------------------------------------------

fwPlane get_plane(const fwVec3d& _point1, const fwVec3d& _point2, const fwVec3d& _point3)
{
    fwPlane plane;
    set_values(plane, _point1, _point2, _point3);

    return plane;
}

//------------------------------------------------------------------------------

void set_values(fwPlane& _plane, const fwVec3d& _point1, const fwVec3d& _point2, const fwVec3d& _point3)
{
    glm::dvec3 p1(_point1[0], _point1[1], _point1[2]);
    glm::dvec3 p2(_point2[0], _point2[1], _point2[2]);
    glm::dvec3 p3(_point3[0], _point3[1], _point3[2]);

    glm::dvec3 normal = glm::cross(p2 - p1, p3 - p1);
    if(glm::length(normal) <= 0.0)
    {
        normal[0] = 0.0;
        normal[1] = 0.0;
        normal[2] = 1.0;
    }

    normal = glm::normalize(normal);
    double distance = glm::dot(normal, p1);

    _plane[0] = normal[0];
    _plane[1] = normal[1];
    _plane[2] = normal[2];
    _plane[3] = distance;
}

//------------------------------------------------------------------------------

fwVec3d get_normal(const fwPlane& _plane)
{
    return {{_plane[0], _plane[1], _plane[2]}};
}

//------------------------------------------------------------------------------

void set_normal(fwPlane& _plane, const fwVec3d& _normal)
{
    glm::dvec3 vec_normal(_normal[0], _normal[1], _normal[2]);
    vec_normal = glm::normalize(vec_normal);

    _plane[0] = vec_normal[0];
    _plane[1] = vec_normal[1];
    _plane[2] = vec_normal[2];
}

//------------------------------------------------------------------------------

double get_distance(const fwPlane& _plane)
{
    return _plane[3];
}

//------------------------------------------------------------------------------

void set_distance(fwPlane& _plane, const double _distance)
{
    _plane[3] = _distance;
}

//------------------------------------------------------------------------------

bool intersect(const fwPlane& _plane, const fwLine& _line, fwVec3d& _point)
{
    glm::dvec3 normal(_plane[0], _plane[1], _plane[2]);
    normal = glm::normalize(normal);
    glm::dvec3 line_direction(_line.second[0] - _line.first[0],
                              _line.second[1] - _line.first[1],
                              _line.second[2] - _line.first[2]);
    line_direction = glm::normalize(line_direction);
    glm::dvec3 line_origin(_line.first[0], _line.first[1], _line.first[2]);

    double intersection_distance = 0.;
    double d                     = glm::dot(line_direction, normal);

    if(std::abs(d) < EPSILON)
    {
        return false;
    }

    intersection_distance = (_plane[3] - glm::dot(normal, line_origin)) / d;

    line_origin += line_direction * intersection_distance;
    _point[0]    = line_origin[0];
    _point[1]    = line_origin[1];
    _point[2]    = line_origin[2];

    return true;
}

//------------------------------------------------------------------------------

bool is_in_half_space(const fwPlane& _plane, const fwVec3d& _point)
{
    glm::dvec3 point_glm(_point[0], _point[1], _point[2]);
    glm::dvec3 normal(_plane[0], _plane[1], _plane[2]);
    glm::normalize(normal);
    glm::dvec3 pos = normal * _plane[3];
    return glm::dot(normal, point_glm - pos) >= 0.0;
}

//------------------------------------------------------------------------------

void transform(fwPlane& _plane, const fwMatrix4x4& _matrix)
{
    glm::dvec3 normal(_plane[0], _plane[1], _plane[2]);
    glm::dvec3 beg(normal * _plane[3]);
    glm::dvec3 end(beg + normal);
    glm::dvec4 beg4(beg, 1.0);
    glm::dvec4 end4(end, 1.0);
    glm::dmat4x4 mat(_matrix[0][0], _matrix[1][0], _matrix[2][0], _matrix[3][0],
                     _matrix[0][1], _matrix[1][1], _matrix[2][1], _matrix[3][1],
                     _matrix[0][2], _matrix[1][2], _matrix[2][2], _matrix[3][2],
                     _matrix[0][3], _matrix[1][3], _matrix[2][3], _matrix[3][3]);

    beg4 = mat * beg4;
    end4 = mat * end4;

    end[0] = end4[0];
    end[1] = end4[1];
    end[2] = end4[2];

    beg[0] = beg4[0];
    beg[1] = beg4[1];
    beg[2] = beg4[2];

    normal = end - beg;
    normal = glm::normalize(normal);

    _plane[0] = normal[0];
    _plane[1] = normal[1];
    _plane[2] = normal[2];
    _plane[3] = glm::dot(normal, beg);
}

//------------------------------------------------------------------------------

void offset(fwPlane& _plane, double _offset)
{
    double distance = get_distance(_plane);
    distance += _offset;
    set_distance(_plane, distance);
}

//------------------------------------------------------------------------------

fwPlane get_plane(const fwVec3d& _normal, const fwVec3d& _point)
{
    glm::dvec3 point_glm(_point[0], _point[1], _point[2]);
    glm::dvec3 normal_glm(_normal[0], _normal[1], _normal[2]);
    normal_glm = glm::normalize(normal_glm);
    fwPlane plane;
    plane[0] = normal_glm[0];
    plane[1] = normal_glm[1];
    plane[2] = normal_glm[2];
    plane[3] = glm::dot(normal_glm, point_glm);
    return plane;
}

} // namespace sight::geometry::data

//------------------------------------------------------------------------------

bool operator==(fwPlane& _plane1, fwPlane& _plane2)
{
    glm::dvec4 pl1(_plane1[0], _plane1[1], _plane1[2], _plane1[3]);
    glm::dvec4 pl2(_plane2[0], _plane2[1], _plane2[2], _plane2[3]);

    double dx = pl1[0] - pl2[0];
    double dy = pl1[1] - pl2[1];
    double dz = pl1[2] - pl2[2];
    double dd = pl1[3] - pl2[3];

    return std::abs(dx) < EPSILON
           && std::abs(dy) < EPSILON
           && std::abs(dz) < EPSILON
           && std::abs(dd) < EPSILON;
}

//------------------------------------------------------------------------------
