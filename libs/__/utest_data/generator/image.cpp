/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
    using Distribution = std::conditional_t<std::is_floating_point_v<T>,
                                            std::uniform_real_distribution<T>,
                                            std::uniform_int_distribution<T2> >;

    std::mt19937 random(_seed);
    Distribution distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

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
    const auto type = _iterable.getType();

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

void image::generateImage(
    data::image::sptr _image,
    const data::image::Size& _sizes,
    const data::image::Spacing& _spacing,
    const data::image::Origin& _origin,
    const core::type& _type,
    const data::image::PixelFormat& _format,
    const std::optional<std::uint32_t>& _seed
)
{
    SIGHT_ASSERT("Image must not be null", _image);

    const auto lock = _image->dump_lock();

    _image->resize(_sizes, _type, _format);
    _image->setSpacing(_spacing);
    _image->setOrigin(_origin);

    if(_seed)
    {
        randomize_iterable(*_image, *_seed);
    }
    else
    {
        std::memset(_image->buffer(), 0, _image->getSizeInBytes());
    }

    sight::data::helper::medical_image::check_image_slice_index(_image);
}

//------------------------------------------------------------------------------

void image::generateRandomImage(data::image::sptr _image, core::type _type, std::uint32_t _seed)
{
    static constexpr int SIZE        = 50;
    static constexpr int DOUBLE_SIZE = SIZE * 2;

    data::image::Size size;
    size[0] = static_cast<data::image::Size::value_type>(safe_rand() % SIZE + 2);
    size[1] = static_cast<data::image::Size::value_type>(safe_rand() % SIZE + 2);
    size[2] = static_cast<data::image::Size::value_type>(safe_rand() % SIZE + 2);

    data::image::Spacing spacing;
    spacing[0] = (safe_rand() % DOUBLE_SIZE + 1) / double(SIZE);
    spacing[1] = (safe_rand() % DOUBLE_SIZE + 1) / double(SIZE);
    spacing[2] = (safe_rand() % DOUBLE_SIZE + 1) / double(SIZE);
    _image->setSpacing(spacing);

    data::image::Origin origin;
    origin[0] = (safe_rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[1] = (safe_rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[2] = (safe_rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    _image->setOrigin(origin);

    _image->resize(size, _type, data::image::GRAY_SCALE);

    randomizeImage(_image, _seed);

    _image->setWindowWidth({(safe_rand() % DOUBLE_SIZE) / double(SIZE / 10.) + 1});
    _image->setWindowCenter({(safe_rand() % DOUBLE_SIZE - SIZE) / double(SIZE / 10.)});

    sight::data::helper::medical_image::check_image_slice_index(_image);
}

//------------------------------------------------------------------------------

void image::randomizeImage(data::image::sptr _image, std::uint32_t _seed)
{
    randomize_iterable(*_image, _seed);
}

//------------------------------------------------------------------------------

void image::randomizeArray(data::array::sptr _array, std::uint32_t _seed)
{
    randomize_iterable(*_array, _seed);
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::generator
