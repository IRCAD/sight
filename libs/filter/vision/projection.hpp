/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <filter/vision/config.hpp>

#include <cmath>
#include <cstddef>
#include <type_traits>

namespace sight::filter::vision
{

/**
 * @brief Computes the 3D reprojection of a pixel according to a depth map, given the camera calibration data.
 *
 * @tparam T floating point type used for the computation
 * @param[in] px X pixel coordinate.
 * @param[in] py Y pixel coordinate.
 * @param[in] depth Pixel depth according to the depth map.
 * @param[in] cx X Pixel coordinate of the camera optical center.
 * @param[in] cy Y Pixel coordinate of the camera optical center.
 * @param[in] fx X focal length of the camera.
 * @param[in] fy Y focal length of the camera.
 * @param[out] x Output X coordinate.
 * @param[out] y Output Y coordinate.
 * @param[out] z Output Z coordinate.
 */
template<class T>
void project_pixel(
    std::size_t _px,
    std::size_t _py,
    T _depth,
    T _cx,
    T _cy,
    T _fx,
    T _fy,
    T& _x,
    T& _y,
    T& _z
);

/**
 * @brief compute the projection of a point in a 2D image
 *
 * @tparam T floating point type used for the computation
 * @param[in] x x coordinate of the point
 * @param[in] y y coordinate of the point
 * @param[in] z z coordinate of the point
 * @param[in] cx X Pixel coordinate of the camera optical center.
 * @param[in] cy Y Pixel coordinate of the camera optical center.
 * @param[in] fx X focal length of the camera.
 * @param[in] fy Y focal length of the camera.
 * @param[in] width image width
 * @param[in] height image height
 * @param[out] px output x pixel coordinate
 * @param[out] py output y pixel coordinate
 * @return true if the point can be projected on the image
 */
template<class T>
bool project_point(
    T _x,
    T _y,
    T _z,
    T _cx,
    T _cy,
    T _fx,
    T _fy,
    std::size_t _width,
    std::size_t _height,
    std::size_t& _px,
    std::size_t& _py
);

//------------------------------------------------------------------------------

template<class T>
void project_pixel(std::size_t _px, std::size_t _py, T _depth, T _cx, T _cy, T _fx, T _fy, T& _x, T& _y, T& _z)
{
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");
    _x = (static_cast<T>(_px) - _cx) / _fx * _depth;
    _y = (static_cast<T>(_py) - _cy) / _fy * _depth;
    _z = static_cast<T>(_depth);
}

//------------------------------------------------------------------------------

template<class T>
bool project_point(
    T _x,
    T _y,
    T _z,
    T _cx,
    T _cy,
    T _fx,
    T _fy,
    std::size_t _width,
    std::size_t _height,
    std::size_t& _px,
    std::size_t& _py
)
{
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");
    const T a = _x / _z;
    const T b = _y / _z;
    T u       = a * _fx + _cx;
    T v       = b * _fy + _cy;

    if((v < 1.) || (v > static_cast<T>(_height)) || (u < 1.) || (u > static_cast<T>(_width)))
    {
        return false;
    }

    _px = static_cast<std::size_t>(std::lround(u));
    _py = static_cast<std::size_t>(std::lround(v));

    return true;
}

} // namespace sight::filter::vision
