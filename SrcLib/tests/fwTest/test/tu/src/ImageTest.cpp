/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageTest.hpp"

#include <fwTest/generator/Image.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTools/Type.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTest::ut::ImageTest );

namespace fwTest
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

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

void ImageTest::generator3DTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    const std::uint8_t dim = 3;
    ::fwData::Image::SizeType size(dim);
    size[0] = 10;
    size[1] = 15;
    size[2] = 23;
    ::fwData::Image::SpacingType spacing(dim);
    spacing[0] = 0.85;
    spacing[1] = 2.6;
    spacing[2] = 1.87;
    ::fwData::Image::OriginType origin(dim);
    origin[0]            = -45.6;
    origin[1]            = 25.97;
    origin[2]            = -53.9;
    ::fwTools::Type type = ::fwTools::Type::create< std::int32_t >();

    ::fwTest::generator::Image::generateImage(image, size, spacing, origin, type);

    CPPUNIT_ASSERT(image->getSize() == size);
    CPPUNIT_ASSERT(image->getSpacing() == spacing);
    CPPUNIT_ASSERT(image->getOrigin() == origin);
    CPPUNIT_ASSERT_EQUAL(type.string(), image->getType().string());
}

//------------------------------------------------------------------------------

void ImageTest::generator2DTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    const std::uint8_t dim = 2;
    ::fwData::Image::SizeType size(dim);
    size[0] = 10;
    size[1] = 15;
    std::vector<double> spacing(dim);
    spacing[0] = 0.85;
    spacing[1] = 2.6;
    std::vector<double> origin(dim);
    origin[0]            = -45.6;
    origin[1]            = 25.97;
    ::fwTools::Type type = ::fwTools::Type::create< std::int16_t >();

    ::fwTest::generator::Image::generateImage(image, size, spacing, origin, type);
    CPPUNIT_ASSERT(image->getSize() == size);
    CPPUNIT_ASSERT(image->getSpacing() == spacing);
    CPPUNIT_ASSERT(image->getOrigin() == origin);
    CPPUNIT_ASSERT_EQUAL(type.string(), image->getType().string());
}

//------------------------------------------------------------------------------

void ImageTest::deepCopyTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwData::Image::sptr image2;
    ::fwTools::Type type = ::fwTools::Type::create< std::int32_t >();
    ::fwTest::generator::Image::generateRandomImage(image, type);

    image2 = ::fwData::Object::copy(image);
    compare(image, image2);
}

//------------------------------------------------------------------------------

void ImageTest::stressTest()
{
    ::fwTools::Type type = ::fwTools::Type::create< std::int8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::int16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::int32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::int64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< float >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< double >();
    this->stressTestWithType(type, 10);
}

//------------------------------------------------------------------------------

void ImageTest::stressTestWithType(::fwTools::Type type, int nbTest)
{
    for (int nb = 0; nb < nbTest; ++nb)
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(image, type);

        ::fwData::Image::sptr image2;
        image2 = ::fwData::Object::copy(image);
        compare(image, image2);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwTest
