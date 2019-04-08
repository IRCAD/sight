/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwTest/generator/Image.hpp"

#include <fwDataTools/helper/Array.hpp>

#include <fwTools/NumericRoundCast.hxx>
#include <fwTools/Type.hpp>

#include <ctime>

namespace fwTest
{
namespace generator
{

//------------------------------------------------------------------------------

void Image::initRand()
{
    std::srand(::fwTools::numericRoundCast< unsigned int >(std::time(NULL)));
}

//------------------------------------------------------------------------------

void Image::generateImage(::fwData::Image::sptr image,
                          ::fwData::Image::SizeType size,
                          std::vector<double> spacing,
                          std::vector<double> origin,
                          ::fwTools::Type type)
{
    image->setSpacing(spacing);
    image->setOrigin(origin);
    image->setSize(size);
    image->setType(type);

    image->allocate();

    ::fwData::Array::sptr array = image->getDataArray();
    ::fwDataTools::helper::Array helper(array);
    std::fill(helper.begin(), helper.end(), 0);
}

//------------------------------------------------------------------------------

void Image::generateRandomImage(::fwData::Image::sptr image, ::fwTools::Type type)
{
    constexpr int SIZE        = 50;
    constexpr int DOUBLE_SIZE = SIZE * 2;
    image->setType(type);

    ::fwData::Image::SizeType size(3);
    size[0] = rand() % SIZE + 2;
    size[1] = rand() % SIZE + 2;
    size[2] = rand() % SIZE + 2;
    image->setSize(size);

    std::vector< double > spacing(3);
    spacing[0] = (rand() % DOUBLE_SIZE +1) / double(SIZE);
    spacing[1] = (rand() % DOUBLE_SIZE +1) / double(SIZE);
    spacing[2] = (rand() % DOUBLE_SIZE +1) / double(SIZE);
    image->setSpacing(spacing);

    std::vector< double > origin(3);
    origin[0] = (rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[1] = (rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    origin[2] = (rand() % DOUBLE_SIZE - SIZE) / (SIZE / 10.);
    image->setOrigin(origin);

    image->allocate();

    ::fwData::Array::sptr array = image->getDataArray();
    randomizeArray(array);

    image->setWindowWidth( (rand() % DOUBLE_SIZE) / double(SIZE / 10.) + 1);
    image->setWindowCenter((rand() % DOUBLE_SIZE - SIZE) / double(SIZE / 10.));
}

//------------------------------------------------------------------------------

void Image::randomizeArray(::fwData::Array::sptr array)
{
    ::fwDataTools::helper::Array helper(array);
    char* iter = helper.begin< char >();

    for (; iter != helper.end< char >(); ++iter)
    {
        *iter = rand()%256;
    }
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Image::createRandomizedArray(const std::string& type, ::fwData::Array::SizeType sizes)
{
    ::fwData::Array::sptr array = ::fwData::Array::New();

    array->resize( ::fwTools::Type::create(type), sizes, 1, true );

    Image::randomizeArray( array );

    return array;
}

//------------------------------------------------------------------------------

} // namespace generator
} // namespace fwTest
