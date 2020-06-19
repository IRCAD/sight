/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#ifndef __FWDATATOOLS_VECTOR_HXX__
#define __FWDATATOOLS_VECTOR_HXX__

#include <fwCore/base.hpp>

#include <cmath>

namespace fwDataTools
{

class Point
{
public:

    float x;
    float y;
    float z;

    Point() : x(0.), y(0.), z(0.)
    {
    }
    Point(const float p[3]) : x(p[0]), y(p[1]), z(p[2])
    {
    }

    Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
    {
    }

    bool operator<(const Point& pt) const
    {
        return (   this->x < pt.x
                   ||(this->x == pt.x && this->y < pt.y)
                   ||(this->x == pt.x && this->y == pt.y && this->z < pt.z) );
    }
};

template <typename T>
class Vector
{
public:

    T x;
    T y;
    T z;

    Vector() : x(0.), y(0.), z(0.)
    {
    }

    Vector(const Vector& v) : x(v.x), y(v.y), z(v.z)
    {
    }
    Vector(const Point& p1, const Point& p2) : x(p2.x-p1.x), y(p2.y-p1.y), z(p2.z-p1.z)
    {
    }
    Vector(T _x, T _y, T _z) : x(_x), y(_y), z(_z)
    {
    }


    bool operator<(const Vector& v) const
    {
        return (   x < v.x
                   ||(x == v.x && y < v.y)
                   ||(x == v.x && y == v.y && z < v.z) );
    }

    void operator=(const Vector& v)
    {
        x = v.x; y = v.y; z = v.z;
    }

    void operator-=(const Vector& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
    }

    void operator+=(const Vector& v)
    {
        x += v.x; y += v.y; z += v.z;
    }

    void operator*=(const float val)
    {
        x = val*x; y = val*y; z = val*z;
    }

    void operator/=(const float val)
    {
        x = x/val; y = y/val; z = z/val;
    }

    T norm2()
    {
        return x*x + y*y + z*z;
    }

    T norm()
    {
        return std::sqrt(this->norm2());
    }

    void normalize()
    {
        T norm = this->norm();
        *this /= (norm == 0) ? 1 : norm;
    }

    Vector normalized()
    {
        Vector v(*this);
        v.normalize();
        return v;
    }

    T dot(Vector& v)
    {
        return x*v.x + y*v.y + z*v.z;
    }

    void crossWith(const Vector& v)
    {
        T _x = y * v.z - z * v.y;
        T _y = z * v.x - x * v.z;
        T _z = x * v.y - y * v.x;
        x = _x;
        y = _y;
        z = _z;
    }

    Vector cross(const Vector& v)
    {
        Vector res(*this);
        res.crossWith(v);
        return res;
    }
};

} // namespace fwDataTools

#endif // __FWDATATOOLS_VECTOR_HXX__

