/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Type.hpp>

#include <fwDataTools/Image.hpp>

#include "ImageTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::ImageTest );

namespace fwDataTools
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::testGetterSetter3D()
{
    ::fwData::Image::NewSptr image;
    const ::boost::uint8_t dim = 3;
    ::fwData::Image::SizeType size(dim);
    size[0] = 10;
    size[1] = 15;
    size[2] = 23;
    std::vector<double> spacing(dim);
    spacing[0] = 0.85;
    spacing[1] = 2.6;
    spacing[2] = 1.87;
    std::vector<double> origin(dim);
    origin[0] = -45.6;
    origin[1] = 25.97;
    origin[2] = -53.9;
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int32_t >();

    ::fwDataTools::Image::generateImage(image, size, spacing, origin, type);
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, size, spacing, origin, type));
}

//------------------------------------------------------------------------------

void ImageTest::testGetterSetter2D()
{
    ::fwData::Image::NewSptr image;
    const ::boost::uint8_t dim = 2;
    ::fwData::Image::SizeType size(dim);
    size[0] = 10;
    size[1] = 15;
    std::vector<double> spacing(dim);
    spacing[0] = 0.85;
    spacing[1] = 2.6;
    std::vector<double> origin(dim);
    origin[0] = -45.6;
    origin[1] = 25.97;
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int16_t >();

    ::fwDataTools::Image::generateImage(image, size, spacing, origin, type);
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, size, spacing, origin, type));
}

//------------------------------------------------------------------------------

void ImageTest::testDeepCopy()
{
    ::fwData::Image::NewSptr image;
    ::fwData::Image::NewSptr image2;
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int32_t >();
    ::fwDataTools::Image::generateRandomImage(image, type);

    image2->deepCopy(image);
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2));
}

//------------------------------------------------------------------------------

void ImageTest::stressTest()
{
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::int16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::int32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::int64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< float >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< double >();
    this->stressTestWithType(type, 10);
}

//------------------------------------------------------------------------------

void ImageTest::stressTestWithType(::fwTools::Type type, int nbTest)
{
    for (int nb=0 ; nb < nbTest ; ++nb)
    {
        ::fwData::Image::NewSptr image;
        ::fwData::Image::NewSptr image2;
        ::fwDataTools::Image::generateRandomImage(image, type);

        image2->deepCopy(image);
        CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2));
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
