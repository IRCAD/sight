/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "utest_data/generator/image.hpp"

#include <core/tools/random/generator.hpp>
#include <core/type.hpp>

#include <data/helper/medical_image.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <ctime>
#include <random>

namespace sight::utest_data::generator
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

// Because of N4659 29.6.1.1: [rand.req.genl]/1e requires one of short, int, long, long long, unsigned short,
// unsigned int, unsigned long, or unsigned long long, we need T2 template argument. Sorry

template<typename T, typename T2 = T, typename I>
inline static void randomize(I& _iterable, std::uint32_t _seed = 0)
{
    using distribution_t = std::conditional_t<std::is_floating_point_v<T>,
                                              std::uniform_real_distribution<T>,
                                              std::uniform_int_distribution<T2> >;

    std::mt19937 random(_seed);
    distribution_t distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

    for(auto it = _iterable.template begin<T>(), end = _iterable.template end<T>() ; it != end ; ++it)
    {
        *it = static_cast<T>(distribution(random));
    }
}

//------------------------------------------------------------------------------

template<typename I>
inline static void randomize_iterable(I& _iterable, std::uint32_t _seed = 0)
{
    auto lock       = _iterable.dump_lock();
    const auto type = _iterable.type();

    if(type == core::type::NONE || type == core::type::UINT8)
    {
        randomize<std::uint8_t, std::uint16_t>(_iterable, _seed);
    }
    else if(type == core::type::UINT16)
    {
        randomize<std::uint16_t>(_iterable, _seed);
    }
    else if(type == core::type::UINT32)
    {
        randomize<std::uint32_t>(_iterable, _seed);
    }
    else if(type == core::type::UINT64)
    {
        randomize<std::uint64_t>(_iterable, _seed);
    }
    else if(type == core::type::INT8)
    {
        randomize<std::int8_t, std::int16_t>(_iterable, _seed);
    }
    else if(type == core::type::INT16)
    {
        randomize<std::int16_t>(_iterable, _seed);
    }
    else if(type == core::type::INT32)
    {
        randomize<std::int32_t>(_iterable, _seed);
    }
    else if(type == core::type::INT64)
    {
        randomize<std::int64_t>(_iterable, _seed);
    }
    else if(type == core::type::FLOAT)
    {
        randomize<float>(_iterable, _seed);
    }
    else if(type == core::type::DOUBLE)
    {
        randomize<double>(_iterable, _seed);
    }
    else
    {
        SIGHT_THROW("Unknown type ");
    }
}

//------------------------------------------------------------------------------

void image::generate_image(
    data::image::sptr _image,
    const data::image::size_t& _sizes,
    const data::image::spacing_t& _spacing,
    const data::image::origin_t& _origin,
    const data::image::orientation_t& _orientation,
    const core::type& _type,
    const enum data::image::pixel_format_t& _format,
    const std::optional<std::uint32_t>& _seed
)
{
    SIGHT_ASSERT("Image must not be null", _image);

    const auto lock = _image->dump_lock();

    _image->resize(_sizes, _type, _format);
    _image->set_spacing(_spacing);
    _image->set_origin(_origin);
    _image->set_orientation(_orientation);

    if(_seed)
    {
        randomize_iterable(*_image, *_seed);
    }
    else
    {
        std::memset(_image->buffer(), 0, _image->size_in_bytes());
    }

    sight::data::helper::medical_image::check_image_slice_index(_image);
}

//------------------------------------------------------------------------------

void image::generate_random_image(data::image::sptr _image, core::type _type, std::uint32_t _seed)
{
    static constexpr int s_SIZE        = 50;
    static constexpr int s_DOUBLE_SIZE = s_SIZE * 2;

    data::image::size_t size;
    size[0] = static_cast<data::image::size_t::value_type>(safe_rand() % s_SIZE + 2);
    size[1] = static_cast<data::image::size_t::value_type>(safe_rand() % s_SIZE + 2);
    size[2] = static_cast<data::image::size_t::value_type>(safe_rand() % s_SIZE + 2);

    data::image::spacing_t spacing;
    spacing[0] = (safe_rand() % s_DOUBLE_SIZE + 1) / double(s_SIZE);
    spacing[1] = (safe_rand() % s_DOUBLE_SIZE + 1) / double(s_SIZE);
    spacing[2] = (safe_rand() % s_DOUBLE_SIZE + 1) / double(s_SIZE);
    _image->set_spacing(spacing);

    data::image::origin_t origin;
    origin[0] = (safe_rand() % s_DOUBLE_SIZE - s_SIZE) / (s_SIZE / 10.);
    origin[1] = (safe_rand() % s_DOUBLE_SIZE - s_SIZE) / (s_SIZE / 10.);
    origin[2] = (safe_rand() % s_DOUBLE_SIZE - s_SIZE) / (s_SIZE / 10.);
    _image->set_origin(origin);

    auto rotate_x     = glm::rotate(glm::dmat4(1), glm::radians(double(safe_rand() % 360)), glm::dvec3(1, 0, 0));
    auto rotate_x_y   = glm::rotate(rotate_x, glm::radians(double(safe_rand() % 360)), glm::dvec3(0, 1, 0));
    auto rotate_x_y_z = glm::rotate(rotate_x_y, glm::radians(double(safe_rand() % 360)), glm::dvec3(0, 0, 1));

    _image->set_orientation(
        {rotate_x_y_z[0][0], rotate_x_y_z[1][0], rotate_x_y_z[2][0],
         rotate_x_y_z[0][1], rotate_x_y_z[1][1], rotate_x_y_z[2][1],
         rotate_x_y_z[0][2], rotate_x_y_z[1][2], rotate_x_y_z[2][2]
        });

    _image->resize(size, _type, data::image::gray_scale);

    randomize_image(_image, _seed);

    _image->set_window_width({(safe_rand() % s_DOUBLE_SIZE) / double(s_SIZE / 10.) + 1});
    _image->set_window_center({(safe_rand() % s_DOUBLE_SIZE - s_SIZE) / double(s_SIZE / 10.)});

    sight::data::helper::medical_image::check_image_slice_index(_image);
}

//------------------------------------------------------------------------------

void image::randomize_image(data::image::sptr _image, std::uint32_t _seed)
{
    randomize_iterable(*_image, _seed);
}

//------------------------------------------------------------------------------

void image::randomize_array(data::array::sptr _array, std::uint32_t _seed)
{
    randomize_iterable(*_array, _seed);
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::generator
