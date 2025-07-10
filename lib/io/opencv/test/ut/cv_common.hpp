/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#pragma once

#include <core/spy_log.hpp>

#include <opencv2/core.hpp>

#include <random>
#include <type_traits>

namespace sight::io::opencv::ut
{

//------------------------------------------------------------------------------

template<typename T>
struct getCvFormat;

#define DECLARE_CV_FORMAT(_TYPE, _CVTYPE1, _CVTYPE2, _CVTYPE3, _CVTYPE4) \
        template<> \
        struct getCvFormat<_TYPE> \
        { \
            static const std::array<std::int32_t, 4> type; \
        };

DECLARE_CV_FORMAT(std::uint8_t, CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4)
DECLARE_CV_FORMAT(std::int8_t, CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4)
DECLARE_CV_FORMAT(std::uint16_t, CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4)
DECLARE_CV_FORMAT(std::int16_t, CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4)
DECLARE_CV_FORMAT(std::int32_t, CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4)
DECLARE_CV_FORMAT(float, CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4)
DECLARE_CV_FORMAT(double, CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4)

//------------------------------------------------------------------------------

template<typename T>
cv::Mat gen_cv_image(
    const std::vector<T>& _image_buffer,
    std::size_t _w,
    std::size_t _h,
    std::size_t _d,
    std::uint8_t _num_channels
)
{
    SIGHT_ASSERT("Width should be at least 1", _w >= 1);

    std::vector<int> cv_size;
    if(_d > 0)
    {
        cv_size.push_back(static_cast<int>(_d));
    }

    if(_h > 0)
    {
        cv_size.push_back(static_cast<int>(_h));
    }
    else
    {
        cv_size.push_back(static_cast<int>(1));
    }

    cv_size.push_back(static_cast<int>(_w));

    const auto cv_type = getCvFormat<T>::type[_num_channels - 1];
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    cv::Mat cv_image = cv::Mat(cv_size, cv_type, static_cast<void*>(const_cast<T*>(_image_buffer.data())));

    return cv_image;
}

//------------------------------------------------------------------------------

template<std::integral T>
static const std::vector<T> gen_image_buffer(std::size_t _w, std::size_t _h, std::size_t _d, std::uint8_t _num_channels)
{
    const std::size_t image_size = _w * (_h == 0 ? 1 : _h) * (_d == 0 ? 1 : _d) * _num_channels;
    std::vector<T> buffer;
    buffer.resize(image_size);

    std::random_device rd;
    std::mt19937 engine(rd());

    std::uniform_int_distribution<> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

    for(std::size_t i = 0 ; i < image_size ; ++i)
    {
        auto value    = dist(engine);
        T bound_value = static_cast<T>(value);
        buffer[i] = bound_value;
    }

    return buffer;
}

//------------------------------------------------------------------------------

template<std::floating_point T>
static const std::vector<T> gen_image_buffer(std::size_t _w, std::size_t _h, std::size_t _d, std::uint8_t _num_channels)
{
    const std::size_t image_size = _w * (_h == 0 ? 1 : _h) * (_d == 0 ? 1 : _d) * _num_channels;
    std::vector<T> buffer;
    buffer.resize(image_size);

    std::random_device rd;
    std::mt19937 engine(rd());

    std::uniform_real_distribution<> dist(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());

    for(std::size_t i = 0 ; i < image_size ; ++i)
    {
        auto value    = dist(engine);
        T bound_value = static_cast<T>(value);
        buffer[i] = bound_value;
    }

    return buffer;
}

//------------------------------------------------------------------------------

} // namespace sight::io::opencv::ut
