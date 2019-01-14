/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include <depthMapOp/config.hpp>

#include <cmath>
#include <cstddef>
#include <type_traits>

namespace depthMapOp
{

/// Contains helpers to project/reproject 3D/2D points on images
class DEPTHMAPOP_CLASS_API Projection
{
public:

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
    template <class T>
    static void projectPixel(size_t px, size_t py, T depth,
                             T cx, T cy, T fx, T fy,
                             T& x, T& y, T& z);

/**
 * @brief compute the projection of a point in a 2D image
 *
 * @tparam T floating point type used for the computation
 * @param[in] x x coordinate of the point
 * @param[in] y y coordinate of the point
 * @param[in] z zcoordinate of the point
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
    template <class T>
    static bool projectPoint(T x, T y, T z, T cx, T cy, T fx, T fy,
                             size_t width, size_t height,
                             size_t& px, size_t& py);

};

//------------------------------------------------------------------------------

template <class T>
void Projection::projectPixel(size_t px, size_t py, T depth,
                              T cx, T cy, T fx, T fy,
                              T& x, T& y, T& z)
{
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");
    x = (static_cast<T>(px) - cx) / fx * depth;
    y = (static_cast<T>(py) - cy) / fy * depth;
    z = static_cast<T>(depth);
}

//------------------------------------------------------------------------------

template <class T>
bool Projection::projectPoint(T x, T y, T z, T cx, T cy, T fx, T fy,
                              size_t width, size_t height,
                              size_t& px, size_t& py)
{
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");
    const T a = x / z;
    const T b = y / z;
    T u       = a * fx + cx;
    T v       = b * fy + cy;

    if ((v < 1.) || (v > height) || (u < 1.) || (u > width))
    {
        return false;
    }

    px = static_cast<size_t>(std::lround(u));
    py = static_cast<size_t>(std::lround(v));

    return true;
}

} // namespace depthMapOp
