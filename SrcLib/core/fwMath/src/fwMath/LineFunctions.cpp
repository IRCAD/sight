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

#include "fwMath/LineFunctions.hpp"
#include "fwMath/VectorFunctions.hpp"
#include "fwMath/PlaneFunctions.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <limits>

namespace fwMath
{

//------------------------------------------------------------------------------

bool getClosestPoints( const fwLine& _ray1, const fwLine& _ray2, fwVec3d& _pointOnThis, fwVec3d& _pointOnfwLine)
{
    const ::glm::dvec3 pos1 = ::glm::make_vec3<double>(_ray1.first.data());
    const ::glm::dvec3 dir1 = ::glm::make_vec3<double>(_ray1.second.data());

    const ::glm::dvec3 pos2 = ::glm::make_vec3<double>(_ray2.first.data());
    const ::glm::dvec3 dir2 = ::glm::make_vec3<double>(_ray2.second.data());

    double dd    = ::glm::dot(dir1, dir2);
    double delta = 1.0 - dd * dd;

    if(delta < std::numeric_limits<double>::epsilon())
    {
        return false;
    }

    double t2 = ( ::glm::dot(dir2, pos1 - pos2) -  ::glm::dot(dir1, pos1-pos2) * dd)/delta;
    double t1 = ( -::glm::dot(dir1, pos1 - pos2) + ::glm::dot(dir2, pos1-pos2) * dd)/delta;

    const ::glm::dvec3 pointOnThis   = pos1 + t1 * dir1;
    const ::glm::dvec3 pointOnfwLine = pos2 + t2 * dir2;

    _pointOnThis[0] = pointOnThis[0];
    _pointOnThis[1] = pointOnThis[1];
    _pointOnThis[2] = pointOnThis[2];

    _pointOnfwLine[0] = pointOnfwLine[0];
    _pointOnfwLine[1] = pointOnfwLine[1];
    _pointOnfwLine[2] = pointOnfwLine[2];

    return true;
}

//------------------------------------------------------------------------------

fwVec3d getClosestPoint( const fwLine& _ray, const fwVec3d& _point)
{
    const ::glm::dvec3 pos   = ::glm::make_vec3<double>(_ray.first.data());
    const ::glm::dvec3 dir   = ::glm::make_vec3<double>(_ray.second.data());
    const ::glm::dvec3 point = ::glm::make_vec3<double>(_point.data());

    double t              = ::glm::dot(point - pos, dir);
    const ::glm::dvec3 pt = (pos + t * dir);

    fwVec3d res;
    res[0] = pt[0];
    res[1] = pt[1];
    res[2] = pt[2];

    return res;
}

//------------------------------------------------------------------------------

bool intersect(const fwLine& _ray, double _radius, const fwVec3d& _point)
{
    fwVec3d point          = getClosestPoint(_ray, _point);
    const ::glm::dvec3 pt1 = ::glm::make_vec3<double>(_point.data());
    const ::glm::dvec3 pt2 = ::glm::make_vec3<double>(point.data());
    ::glm::dvec3 tmp = pt1-pt2;
    double length = ::glm::length(tmp);
    return (length <= _radius);
}

//------------------------------------------------------------------------------

bool intersect(const fwLine& _ray, double _radius, const fwVec3d& _origin, const fwVec3d& _direction, fwVec3d& _point)
{
    fwLine line = std::pair<fwVec3d, fwVec3d>(_origin, _direction);
    fwVec3d pThis;
    if(getClosestPoints(_ray, line, pThis, _point) == false)
    {
        return false;
    }

    const ::glm::dvec3 pt1 = ::glm::make_vec3<double>(_point.data());
    const ::glm::dvec3 pt2 = ::glm::make_vec3<double>(pThis.data());
    ::glm::dvec3 tmp = pt1-pt2;
    double length = ::glm::length(tmp);

    return (length <= _radius);
}

//------------------------------------------------------------------------------

bool intersect( const fwLine& _line, const fwVec3d& _v1,  const fwVec3d& _v2, const fwVec3d& _v3, fwVec3d& _point,
                fwVec3d& _barycentric, bool& _front)
{
    _barycentric = (_v1 + _v2 + _v3)/3.;
    const fwVec3d v01 = _v2 - _v1;
    const fwVec3d v12 = _v3 - _v2;
    const fwVec3d v20 = _v1 - _v3;

    const fwPlane plane = getPlane(_v1, _v2, _v3);

    fwVec3d v;
    v[0] = 0.0F;
    v[1] = 0.0F;
    v[2] = 1.0F;

    const fwVec3d& normal = getNormal(plane);
    _front = ((dot(normal,v )) >= 0.0);

    bool isIntersect = true;
    if(intersect(plane, _line, _point) == false)
    {
        isIntersect = false;
    }
    else if((dot(normal, cross(v01, _point-_v1))) < 0.0)
    {
        isIntersect = false;
    }
    else if((dot(normal, cross(v12, _point-_v2))) < 0.0)
    {
        isIntersect = false;
    }
    else if((dot(normal, cross(v20, _point-_v3))) < 0.0)
    {
        isIntersect = false;
    }

    return isIntersect;
}

} //namespace fwMath
