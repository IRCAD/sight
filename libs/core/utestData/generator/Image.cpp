/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "utestData/generator/Image.hpp"

#include <core/tools/NumericRoundCast.hxx>
#include <core/tools/random/Generator.hpp>
#include <core/tools/Type.hpp>

#include <ctime>
#include <random>

namespace sight::utestData
{

namespace generator
{

using core::tools::random::safeRand;

//------------------------------------------------------------------------------

// Because of N4659 29.6.1.1: [rand.req.genl]/1e requires one of short, int, long, long long, unsigned short,
// unsigned int, unsigned long, or unsigned long long, we need T2 template argument. Sorry

template<typename T, typename T2 = T, typename I>
inline static void randomize(I& iterable, std::uint32_t seed = 0)
{
    using Distribution = typename std::conditional<std::is_floating_point<T>::value,
                                                   std::uniform_real_distribution<T>,
                                                   std::uniform_int_distribution<T2> >::type;

    std::mt19937 random(seed);
    Distribution distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

    for(auto it = iterable.template begin<T>(), end = iterable.template end<T>() ; it != end ; ++it)
    {
        *it = static_cast<T>(distribution(random));
    }
}

//------------------------------------------------------------------------------

template<typename I>
inline static void randomizeIterable(I& iterable, std::uint32_t seed = 0)
{
    auto lock       = iterable.dump_lock();
    const auto type = iterable.getType();

    if(type == core::tools::Type::s_UNSPECIFIED_TYPE || type == core::tools::Type::s_UINT8)
    {
        randomize<std::uint8_t, std::uint16_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_UINT16)
    {
        randomize<std::uint16_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_UINT32)
    {
        randomize<std::uint32_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_UINT64)
    {
        randomize<std::uint64_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_INT8)
    {
        randomize<std::int8_t, std::int16_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_INT16)
    {
        randomize<std::int16_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_INT32)
    {
        randomize<std::int32_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_INT64)
    {
        randomize<std::int64_t>(iterable, seed);
    }
    else if(type == core::tools::Type::s_FLOAT)
    {
        randomize<float>(iterable, seed);
    }
    else if(type == core::tools::Type::s_DOUBLE)
    {
        randomize<double>(iterable, seed);
    }
    else
    {
        SIGHT_THROW("Unknowntype: " << type);
    }
}

//------------------------------------------------------------------------------

void Image::generateImage(
    data::Image::sptr image,
    data::Image::Size size,
    data::Image::Spacing spacing,
    data::Image::Origin origin,
    core::tools::Type type,
    data::Image::PixelFormat format
)
{
    image->resize(size, type, format);
    image->setSpacing(spacing);
    image->setOrigin(origin);

    auto lock = image->dump_lock();
    std::fill(image->begin(), image->end(), 0);
}

//------------------------------------------------------------------------------

void Image::generateRandomImage(data::Image::sptr image, core::tools::Type type, std::uint32_t seed)
{
    constexpr int SIZE        = 50;
    constexpr int DOUBLE_SIZE = SIZE * 2;

    data::Image::Size size;
    size[0] = static_cast<data::Image::Size::value_type>(safeRand() % SIZE + 2);
    size[1] = static_cast<data::Image::Size::value_type>(safeRand() % SIZE + 2);
    size[2] = static_cast<data::Image::Size::value_type>(safeRand() % SIZE + 2);

    data::Image::Spacing spacing;
    spacing[0] = (safeRand() % DOUBLE_SIZE + 1) / double(SIZE);
    spacing[1] = (safeRand() % DOUBLE_SIZE + 1) / double(SIZE);
    spacing[2] = (safeRand() % DOUBLE_SIZE + 1) / double(SIZE);
    image->setSpacing(spacing);

    data::Image::Origin origin;
    origin[0] = (safeRand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[1] = (safeRand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[2] = (safeRand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    image->setOrigin(origin);

    image->resize(size, type, data::Image::GRAY_SCALE);

    randomizeImage(image, seed);

    image->setWindowWidth((safeRand() % DOUBLE_SIZE) / double(SIZE / 10.) + 1);
    image->setWindowCenter((safeRand() % DOUBLE_SIZE - SIZE) / double(SIZE / 10.));
}

//------------------------------------------------------------------------------

void Image::randomizeImage(data::Image::sptr image, std::uint32_t seed)
{
    randomizeIterable(*image, seed);
}

//------------------------------------------------------------------------------

void Image::randomizeArray(data::Array::sptr array, std::uint32_t seed)
{
    randomizeIterable(*array, seed);
}

//------------------------------------------------------------------------------

data::Array::sptr Image::createRandomizedArray(const std::string& type, data::Array::SizeType sizes, std::uint32_t seed)
{
    data::Array::sptr array = data::Array::New();

    array->resize(sizes, core::tools::Type::create(type), true);

    Image::randomizeArray(array, seed);

    return array;
}

//------------------------------------------------------------------------------

} // namespace generator

} // namespace sight::utestData
