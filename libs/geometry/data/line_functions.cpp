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

#include "geometry/data/line_functions.hpp"

#include "geometry/data/plane_functions.hpp"
#include "geometry/data/vector_functions.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <limits>

namespace sight::geometry::data
{

//------------------------------------------------------------------------------

bool get_closest_points(
    const fw_line& _ray1,
    const fw_line& _ray2,
    fw_vec3d& _point_on_this,
    fw_vec3d& _point_onfw_line
)
{
    const glm::dvec3 pos1 = glm::make_vec3<double>(_ray1.first.data());
    const glm::dvec3 dir1 = glm::make_vec3<double>(_ray1.second.data());

    const glm::dvec3 pos2 = glm::make_vec3<double>(_ray2.first.data());
    const glm::dvec3 dir2 = glm::make_vec3<double>(_ray2.second.data());

    double dd    = glm::dot(dir1, dir2);
    double delta = 1.0 - dd * dd;

    if(delta < std::numeric_limits<double>::epsilon())
    {
        return false;
    }

    double t2 = (glm::dot(dir2, pos1 - pos2) - glm::dot(dir1, pos1 - pos2) * dd) / delta;
    double t1 = (-glm::dot(dir1, pos1 - pos2) + glm::dot(dir2, pos1 - pos2) * dd) / delta;

    const glm::dvec3 point_on_this   = pos1 + t1 * dir1;
    const glm::dvec3 point_onfw_line = pos2 + t2 * dir2;

    _point_on_this[0] = point_on_this[0];
    _point_on_this[1] = point_on_this[1];
    _point_on_this[2] = point_on_this[2];

    _point_onfw_line[0] = point_onfw_line[0];
    _point_onfw_line[1] = point_onfw_line[1];
    _point_onfw_line[2] = point_onfw_line[2];

    return true;
}

//------------------------------------------------------------------------------

fw_vec3d get_closest_point(const fw_line& _ray, const fw_vec3d& _point)
{
    const glm::dvec3 pos   = glm::make_vec3<double>(_ray.first.data());
    const glm::dvec3 dir   = glm::make_vec3<double>(_ray.second.data());
    const glm::dvec3 point = glm::make_vec3<double>(_point.data());

    double t            = glm::dot(point - pos, dir);
    const glm::dvec3 pt = (pos + t * dir);

    fw_vec3d res;
    res[0] = pt[0];
    res[1] = pt[1];
    res[2] = pt[2];

    return res;
}

//------------------------------------------------------------------------------

bool intersect(const fw_line& _ray, double _radius, const fw_vec3d& _point)
{
    fw_vec3d point       = get_closest_point(_ray, _point);
    const glm::dvec3 pt1 = glm::make_vec3<double>(_point.data());
    const glm::dvec3 pt2 = glm::make_vec3<double>(point.data());
    glm::dvec3 tmp       = pt1 - pt2;
    double length        = glm::length(tmp);
    return length <= _radius;
}

//------------------------------------------------------------------------------

bool intersect(
    const fw_line& _ray,
    double _radius,
    const fw_vec3d& _origin,
    const fw_vec3d& _direction,
    fw_vec3d& _point
)
{
    fw_line line = std::pair<fw_vec3d, fw_vec3d>(_origin, _direction);
    fw_vec3d p_this;
    if(!get_closest_points(_ray, line, p_this, _point))
    {
        return false;
    }

    const glm::dvec3 pt1 = glm::make_vec3<double>(_point.data());
    const glm::dvec3 pt2 = glm::make_vec3<double>(p_this.data());
    glm::dvec3 tmp       = pt1 - pt2;
    double length        = glm::length(tmp);

    return length <= _radius;
}

//------------------------------------------------------------------------------

bool intersect(
    const fw_line& _line,
    const fw_vec3d& _v1,
    const fw_vec3d& _v2,
    const fw_vec3d& _v3,
    fw_vec3d& _point,
    fw_vec3d& _barycentric,
    bool& _front
)
{
    _barycentric = (_v1 + _v2 + _v3) / 3.;
    const fw_vec3d v01 = _v2 - _v1;
    const fw_vec3d v12 = _v3 - _v2;
    const fw_vec3d v20 = _v1 - _v3;

    const fw_plane plane = get_plane(_v1, _v2, _v3);

    fw_vec3d v;
    v[0] = 0.0F;
    v[1] = 0.0F;
    v[2] = 1.0F;

    const fw_vec3d& normal = get_normal(plane);
    _front = ((dot(normal, v)) >= 0.0);

    return !(!intersect(plane, _line, _point) || ((dot(normal, cross(v01, _point - _v1))) < 0.0)
             || ((dot(normal, cross(v12, _point - _v2))) < 0.0) || ((dot(normal, cross(v20, _point - _v3))) < 0.0));
}

} //namespace sight::geometry::data
