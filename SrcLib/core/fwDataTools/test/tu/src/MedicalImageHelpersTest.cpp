/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MedicalImageHelpersTest.hpp"

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Array.hpp>

#include <fwData/Array.hpp>
#include <fwData/Image.hpp>

#include <fwTest/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::MedicalImageHelpersTest );

namespace fwDataTools
{
namespace ut
{

// ------------------------------------------------------------------------------

void MedicalImageHelpersTest::setUp()
{
    // Set up context before running a test.
}

// ------------------------------------------------------------------------------

void MedicalImageHelpersTest::tearDown()
{
    // Clean up after the test run.
}

// ------------------------------------------------------------------------------

void MedicalImageHelpersTest::getMinMaxTest()
{

    {
        // Test on 3D image of type 'int16'
        typedef std::int16_t Type;

        const Type MIN   = 45;
        const Type MAX   = 345;
        const Type RANGE = MAX - MIN;

        ::fwData::Image::sptr image = ::fwData::Image::New();

        ::fwData::Image::SizeType size(3);
        size[0] = 125;
        size[1] = 110;
        size[2] = 45;
        ::fwData::Image::SpacingType spacing(3, 1);
        ::fwData::Image::OriginType origin(3, 0);
        ::fwTest::generator::Image::generateImage(image, size, spacing, origin, ::fwTools::Type::create<Type>());

        ::fwData::Array::sptr array = image->getDataArray();
        ::fwDataTools::helper::Array helper(array);

        Type* buffer    = helper.begin<Type>();
        Type* bufferEnd = helper.end<Type>();

        for (; buffer != bufferEnd; ++buffer)
        {
            *buffer = MIN + rand()%RANGE;
        }

        Type resMin, resMax;

        buffer = helper.begin<Type>();

        buffer[156] = MIN;
        buffer[245] = MAX;

        ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }

    {
        // Test on 3D image of type 'float'
        typedef float Type;
        const Type MIN  = -12.3;
        const Type MAX  = 18.2;
        const int RANGE = static_cast<int>(MAX - MIN);

        ::fwData::Image::sptr image = ::fwData::Image::New();

        ::fwData::Image::SizeType size(3);
        size[0] = 42;
        size[1] = 34;
        size[2] = 75;
        ::fwData::Image::SpacingType spacing(3, 1);
        ::fwData::Image::OriginType origin(3, 0);
        ::fwTest::generator::Image::generateImage(image, size, spacing, origin, ::fwTools::Type::create<Type>());

        ::fwData::Array::sptr array = image->getDataArray();
        ::fwDataTools::helper::Array helper(array);

        Type* buffer    = helper.begin<Type>();
        Type* bufferEnd = helper.end<Type>();

        for (; buffer != bufferEnd; ++buffer)
        {
            *buffer = MIN + rand()%RANGE;
        }

        Type resMin, resMax;

        buffer = helper.begin<Type>();

        buffer[16]  = MIN;
        buffer[286] = MAX;

        ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("min values are not equal", MIN, resMin, 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("max values are not equal", MAX, resMax, 0.00001);
    }

    {
        // test of 2D image of type 'uint8'

        typedef std::uint8_t Type;

        const Type MIN   = 3;
        const Type MAX   = 245;
        const Type RANGE = MAX - MIN;

        ::fwData::Image::sptr image = ::fwData::Image::New();

        ::fwData::Image::SizeType size(2);
        size[0] = 156;
        size[1] = 208;
        ::fwData::Image::SpacingType spacing(2, 1);
        ::fwData::Image::OriginType origin(2, 0);
        ::fwTest::generator::Image::generateImage(image, size, spacing, origin, ::fwTools::Type::create<Type>());

        ::fwData::Array::sptr array = image->getDataArray();
        ::fwDataTools::helper::Array helper(array);

        Type* buffer    = helper.begin<Type>();
        Type* bufferEnd = helper.end<Type>();

        for (; buffer != bufferEnd; ++buffer)
        {
            *buffer = MIN + rand()%RANGE;
        }

        Type resMin, resMax;

        buffer = helper.begin<Type>();

        buffer[5]    = MIN;
        buffer[2155] = MAX;

        ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }
}

// ------------------------------------------------------------------------------

void MedicalImageHelpersTest::getPixelBufferTest()
{
    {
        uint8_t pGray[1] = {84};
        uint8_t pRGB[3]  = {42, 24, 21};
        getPixelBufferTestHelper<uint8_t, 1>(pGray);
        getPixelBufferTestHelper<uint8_t, 3>(pRGB);
    }
    {
        uint32_t pGray[1] = {0xDEADBEEF};
        uint32_t pRGB[3]  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        getPixelBufferTestHelper<uint32_t, 1>(pGray);
        getPixelBufferTestHelper<uint32_t, 3>(pRGB);
    }
    {
        float pGray[1] = {5423.2f};
        float pRGB[3]  = {42.0f, 1487.4f, 0.1445f};
        getPixelBufferTestHelper<float, 1>(pGray);
        getPixelBufferTestHelper<float, 3>(pRGB);
    }
    {
        double pGray[1] = {541.254981};
        double pRGB[3]  = {841.567, 6476.874, 0.187487};
        getPixelBufferTestHelper<double, 1>(pGray);
        getPixelBufferTestHelper<double, 3>(pRGB);
    }
}

// ------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDataTools
