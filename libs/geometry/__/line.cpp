/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <geometry/__/line.hpp>
#include <geometry/__/plane.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <limits>

namespace sight::geometry
{

//------------------------------------------------------------------------------

bool get_closest_points(
    const ray_t& _ray1,
    const ray_t& _ray2,
    glm::dvec3& _point_on_ray1,
    glm::dvec3& _point_on_ray2
)
{
    const glm::dvec3 pos1 = _ray1.first;
    const glm::dvec3 dir1 = _ray1.second;

    const glm::dvec3 pos2 = _ray2.first;
    const glm::dvec3 dir2 = _ray2.second;

    double dd    = glm::dot(dir1, dir2);
    double delta = 1.0 - dd * dd;

    if(delta < std::numeric_limits<double>::epsilon())
    {
        return false;
    }

    double t2 = (glm::dot(dir2, pos1 - pos2) - glm::dot(dir1, pos1 - pos2) * dd) / delta;
    double t1 = (-glm::dot(dir1, pos1 - pos2) + glm::dot(dir2, pos1 - pos2) * dd) / delta;

    const glm::dvec3 point_on_ray1 = pos1 + t1 * dir1;
    const glm::dvec3 point_on_ray2 = pos2 + t2 * dir2;

    _point_on_ray1[0] = point_on_ray1[0];
    _point_on_ray1[1] = point_on_ray1[1];
    _point_on_ray1[2] = point_on_ray1[2];

    _point_on_ray2[0] = point_on_ray2[0];
    _point_on_ray2[1] = point_on_ray2[1];
    _point_on_ray2[2] = point_on_ray2[2];

    return true;
}

//------------------------------------------------------------------------------

glm::dvec3 get_closest_point(const ray_t& _ray, const glm::dvec3& _point)
{
    const glm::dvec3 pos   = _ray.first;
    const glm::dvec3 dir   = _ray.second;
    const glm::dvec3 point = _point;

    double t            = glm::dot(point - pos, dir);
    const glm::dvec3 pt = (pos + t * dir);

    glm::dvec3 res;
    res[0] = pt[0];
    res[1] = pt[1];
    res[2] = pt[2];

    return res;
}

//------------------------------------------------------------------------------

bool intersect(const ray_t& _ray, double _radius, const glm::dvec3& _point)
{
    glm::dvec3 point     = get_closest_point(_ray, _point);
    const glm::dvec3 pt1 = _point;
    const glm::dvec3 pt2 = point;
    glm::dvec3 tmp       = pt1 - pt2;
    double length        = glm::length(tmp);
    return length <= _radius;
}

//------------------------------------------------------------------------------

bool intersect(
    const ray_t& _ray,
    double _radius,
    const glm::dvec3& _origin,
    const glm::dvec3& _direction,
    glm::dvec3& _point
)
{
    ray_t line = std::pair<glm::dvec3, glm::dvec3>(_origin, _direction);
    glm::dvec3 p_this;
    if(!get_closest_points(_ray, line, p_this, _point))
    {
        return false;
    }

    const glm::dvec3 pt1 = _point;
    const glm::dvec3 pt2 = p_this;
    glm::dvec3 tmp       = pt1 - pt2;
    double length        = glm::length(tmp);

    return length <= _radius;
}

//------------------------------------------------------------------------------

bool intersect(
    const line_t& _line,
    const glm::dvec3& _v1,
    const glm::dvec3& _v2,
    const glm::dvec3& _v3,
    glm::dvec3& _point,
    glm::dvec3& _barycentric,
    bool& _front
)
{
    _barycentric = (_v1 + _v2 + _v3) / 3.;
    const glm::dvec3 v01 = _v2 - _v1;
    const glm::dvec3 v12 = _v3 - _v2;
    const glm::dvec3 v20 = _v1 - _v3;

    const plane_t plane = get_plane(_v1, _v2, _v3);

    glm::dvec3 v;
    v[0] = 0.0F;
    v[1] = 0.0F;
    v[2] = 1.0F;

    const glm::dvec3& normal = get_normal(plane);
    _front = ((dot(normal, v)) >= 0.0);

    return !(!intersect(plane, _line, _point) || ((dot(normal, cross(v01, _point - _v1))) < 0.0)
             || ((dot(normal, cross(v12, _point - _v2))) < 0.0) || ((dot(normal, cross(v20, _point - _v3))) < 0.0));
}

} //namespace sight::geometry
