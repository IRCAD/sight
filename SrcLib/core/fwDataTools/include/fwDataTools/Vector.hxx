/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_VECTOR_HPP_
#define _FWDATATOOLS_VECTOR_HPP_

#include <fwCore/base.hpp>


namespace fwDataTools
{

class Point
{
public:

    float x;
    float y;
    float z;

    Point() : x(0.), y(0.), z(0.)
    {}
    Point(const float p[3]) : x(p[0]), y(p[1]), z(p[2])
    {}

    Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
    {}

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
    {}

    Vector(const Vector& v) : x(v.x), y(v.y), z(v.z)
    {}
    Vector(const Point& p1, const Point& p2) : x(p2.x-p1.x), y(p2.y-p1.y), z(p2.z-p1.z)
    {}
    Vector(T _x, T _y, T _z) : x(_x), y(_y), z(_z)
    {}


    bool operator<(const Vector& v) const
    {
        return (   x < v.x
                ||(x == v.x && y < v.y)
                ||(x == v.x && y == v.y && z < v.z) );
    }

    void operator=(const Vector& v)
    { x = v.x; y = v.y; z = v.z; }

    void operator-=(const Vector& v)
    { x -= v.x; y -= v.y; z -= v.z; }

    void operator+=(const Vector& v)
    { x += v.x; y += v.y; z += v.z; }

    void operator*=(const float val)
    { x = val*x; y = val*y; z = val*z; }

    void operator/=(const float val)
    { x = x/val; y = y/val; z = z/val; }

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

    T dot(Vector &v)
    {
        return x*v.x + y*v.y + z*v.z;
    }

    void crossWith(const Vector &v)
    {
        T _x = y * v.z - z * v.y;
        T _y = z * v.x - x * v.z;
        T _z = x * v.y - y * v.x;
        x = _x;
        y = _y;
        z = _z;
    }

    Vector cross(const Vector &v)
    {
        Vector res(*this);
        res.crossWith(v);
        return res;
    }
};

} // namespace fwDataTools

#endif // _FWDATATOOLS_VECTOR_HPP_

