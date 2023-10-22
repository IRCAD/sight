/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#pragma once

#include <core/base.hpp>

#include <array>
#include <cmath>

namespace sight::geometry::data
{

class Point
{
public:

    float x {0.};
    float y {0.};
    float z {0.};

    Point()
    = default;

    Point(const std::array<float, 3>& _p) :
        x(_p[0]),
        y(_p[1]),
        z(_p[2])
    {
    }

    Point(float _x, float _y, float _z) :
        x(_x),
        y(_y),
        z(_z)
    {
    }

    //------------------------------------------------------------------------------

    bool operator<(const Point& _pt) const
    {
        return this->x < _pt.x
               || (this->x == _pt.x && this->y < _pt.y)
               || (this->x == _pt.x && this->y == _pt.y && this->z < _pt.z);
    }
};

template<typename T>
class Vector
{
public:

    T x;
    T y;
    T z;

    Vector() :
        x(0.),
        y(0.),
        z(0.)
    {
    }

    Vector(const Vector& _v) :
        x(_v.x),
        y(_v.y),
        z(_v.z)
    {
    }

    Vector(const Point& _p1, const Point& _p2) :
        x(_p2.x - _p1.x),
        y(_p2.y - _p1.y),
        z(_p2.z - _p1.z)
    {
    }

    Vector(T _x, T _y, T _z) :
        x(_x),
        y(_y),
        z(_z)
    {
    }

    //------------------------------------------------------------------------------

    bool operator<(const Vector& _v) const
    {
        return x < _v.x
               || (x == _v.x && y < _v.y)
               || (x == _v.x && y == _v.y && z < _v.z);
    }

    //------------------------------------------------------------------------------

    Vector& operator=(const Vector& _v) = default;

    //------------------------------------------------------------------------------

    void operator-=(const Vector& _v)
    {
        x -= _v.x;
        y -= _v.y;
        z -= _v.z;
    }

    //------------------------------------------------------------------------------

    void operator+=(const Vector& _v)
    {
        x += _v.x;
        y += _v.y;
        z += _v.z;
    }

    //------------------------------------------------------------------------------

    void operator*=(const float _val)
    {
        x = _val * x;
        y = _val * y;
        z = _val * z;
    }

    //------------------------------------------------------------------------------

    void operator/=(const float _val)
    {
        x = x / _val;
        y = y / _val;
        z = z / _val;
    }

    //------------------------------------------------------------------------------

    T norm2()
    {
        return x * x + y * y + z * z;
    }

    //------------------------------------------------------------------------------

    T norm()
    {
        return std::sqrt(this->norm2());
    }

    //------------------------------------------------------------------------------

    void normalize()
    {
        T norm = this->norm();
        *this /= (norm == 0) ? 1 : norm;
    }

    //------------------------------------------------------------------------------

    Vector normalized()
    {
        Vector v(*this);
        v.normalize();
        return v;
    }

    //------------------------------------------------------------------------------

    T dot(Vector& _v)
    {
        return x * _v.x + y * _v.y + z * _v.z;
    }

    //------------------------------------------------------------------------------

    void crossWith(const Vector& _v)
    {
        Vector v;
        v.x = y * _v.z - z * _v.y;
        v.y = z * _v.x - x * _v.z;
        v.z = x * _v.y - y * _v.x;
        x   = v.x;
        y   = v.y;
        z   = v.z;
    }

    //------------------------------------------------------------------------------

    Vector cross(const Vector& _v)
    {
        Vector res(*this);
        res.crossWith(_v);
        return res;
    }
};

} // namespace sight::geometry::data
