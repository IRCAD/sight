/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include <core/tools/Type.hpp>

#include <ctime>

namespace sight::utestData
{

namespace generator
{

//------------------------------------------------------------------------------

void Image::initRand()
{
    std::srand(core::tools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

void Image::generateImage(
    data::Image::sptr image,
    data::Image::SizeType size,
    std::vector<double> spacing,
    std::vector<double> origin,
    core::tools::Type type
)
{
    image->setSpacing(spacing);
    image->setOrigin(origin);
    image->setSize(size);
    image->setType(type);

    image->resize();

    auto lock = image->lock();
    std::fill(image->begin(), image->end(), 0);
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
    image->setSpacing2(spacing);
    image->setOrigin2(origin);

    auto lock = image->lock();
    std::fill(image->begin(), image->end(), 0);
}

//------------------------------------------------------------------------------

void Image::generateRandomImage(data::Image::sptr image, core::tools::Type type)
{
    constexpr int SIZE        = 50;
    constexpr int DOUBLE_SIZE = SIZE * 2;
    image->setType(type);

    data::Image::Size size;
    size[0] = static_cast<data::Image::Size::value_type>(rand() % SIZE + 2);
    size[1] = static_cast<data::Image::Size::value_type>(rand() % SIZE + 2);
    size[2] = static_cast<data::Image::Size::value_type>(rand() % SIZE + 2);
    image->setSize2(size);

    data::Image::Spacing spacing;
    spacing[0] = (rand() % DOUBLE_SIZE + 1) / double(SIZE);
    spacing[1] = (rand() % DOUBLE_SIZE + 1) / double(SIZE);
    spacing[2] = (rand() % DOUBLE_SIZE + 1) / double(SIZE);
    image->setSpacing2(spacing);

    data::Image::Origin origin;
    origin[0] = (rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[1] = (rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[2] = (rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    image->setOrigin2(origin);
    image->setPixelFormat(data::Image::GRAY_SCALE);

    image->resize();

    randomizeImage(image);

    image->setWindowWidth((rand() % DOUBLE_SIZE) / double(SIZE / 10.) + 1);
    image->setWindowCenter((rand() % DOUBLE_SIZE - SIZE) / double(SIZE / 10.));
}

//------------------------------------------------------------------------------

void Image::randomizeImage(data::Image::sptr image)
{
    auto lock          = image->lock();
    auto iter          = image->begin();
    const auto iterEnd = image->end();

    for( ; iter != iterEnd ; ++iter)
    {
        *iter = static_cast<char>(rand() % 256);
    }
}

//------------------------------------------------------------------------------

void Image::randomizeArray(data::Array::sptr array)
{
    const auto dumpLock = array->lock();
    auto iter           = array->begin();
    const auto end      = array->end();

    for( ; iter != end ; ++iter)
    {
        *iter = static_cast<char>(rand() % 256);
    }
}

//------------------------------------------------------------------------------

data::Array::sptr Image::createRandomizedArray(const std::string& type, data::Array::SizeType sizes)
{
    data::Array::sptr array = data::Array::New();

    array->resize(sizes, core::tools::Type::create(type), true);

    Image::randomizeArray(array);

    return array;
}

//------------------------------------------------------------------------------

} // namespace generator

} // namespace sight::utestData
