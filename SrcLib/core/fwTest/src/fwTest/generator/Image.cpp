/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    image->setType(type);

    ::fwData::Image::SizeType size(3);
    size[0] = rand()%100 + 2;
    size[1] = rand()%100 + 2;
    size[2] = rand()%100 + 2;
    image->setSize(size);

    std::vector< double > spacing(3);
    spacing[0] = (rand()%200 +1) / 100.;
    spacing[1] = (rand()%200 +1) / 100.;
    spacing[2] = (rand()%200 +1) / 100.;
    image->setSpacing(spacing);

    std::vector< double > origin(3);
    origin[0] = (rand()%200 - 100) / 10.;
    origin[1] = (rand()%200 - 100) / 10.;
    origin[2] = (rand()%200 - 100) / 10.;
    image->setOrigin(origin);

    image->allocate();

    ::fwData::Array::sptr array = image->getDataArray();
    randomizeArray(array);

    image->setWindowWidth( (rand()%200) / 10. + 1);
    image->setWindowCenter((rand()%200 - 100) / 10.);
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
