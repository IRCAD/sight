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

template <class P, size_t N>
void getPixelBufferTestHelper(P pixelValue[3])
{
    static_assert(N != 0, "Cannot test 0-dimensional pixel types");
    constexpr size_t IMG_DIMENSIONS = 100;

    // Create a new image
    auto image = ::fwData::Image::New();
    ::fwData::Image::SizeType size(3);
    std::fill_n(size.begin(), 3, IMG_DIMENSIONS);
    image->allocate(size, ::fwTools::Type::create<P>(), N);
    image->setSpacing(::fwData::Image::SpacingType(3, 1));
    image->setOrigin(::fwData::Image::OriginType(3, 0));

    // Zero the buffer
    auto array           = image->getDataArray();
    const auto szArray   = image->getAllocatedSizeInBytes();
    void* const arrayPtr = array->getBufferObject()->getBuffer();
    std::fill_n(static_cast<uint8_t*>(arrayPtr), szArray, 0);

    // Pick some random coordinates and store the given pixel there
    size_t coords[3];
    std::generate_n(coords, 3, [&] () { return rand() % IMG_DIMENSIONS; });
    P* pixelPtr = static_cast<P*>(arrayPtr) +
                  ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * N);
    std::copy(pixelValue, pixelValue + N, pixelPtr);

    // Test that the helper returned pixel value is correct
    ::fwDataTools::helper::ImageGetter constHelper(image);
    ::fwDataTools::helper::Image helper(image);
    P* pixelHelperPtr1 = static_cast<P*>(constHelper.getPixelBuffer(coords[0], coords[1], coords[2]));
    P* pixelHelperPtr2 = static_cast<P*>(helper.getPixelBuffer(coords[0], coords[1], coords[2]));
    if(std::is_floating_point<P>::value)
    {
        for(auto i = 0; i != N; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr1[i], pixelValue[i],
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr2[i], pixelValue[i],
                                                 0.00001);
        }
    }
    else
    {
        for(auto i = 0; i != N; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr1[i], pixelValue[i]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr2[i], pixelValue[i]);
        }
    }
}

//------------------------------------------------------------------------------

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

template <class P, size_t N>
void setPixelBufferTestHelper(P pixelValue[3])
{
    static_assert(N != 0, "Cannot test 0-dimensional pixel types");
    constexpr size_t IMG_DIMENSIONS = 100;

    // Create a new image
    auto image = ::fwData::Image::New();
    ::fwData::Image::SizeType size(3);
    std::fill_n(size.begin(), 3, IMG_DIMENSIONS);
    image->allocate(size, ::fwTools::Type::create<P>(), N);
    image->setSpacing(::fwData::Image::SpacingType(3, 1));
    image->setOrigin(::fwData::Image::OriginType(3, 0));

    // Zero the buffer
    auto array           = image->getDataArray();
    const auto szArray   = image->getAllocatedSizeInBytes();
    void* const arrayPtr = array->getBufferObject()->getBuffer();
    std::fill_n(static_cast<uint8_t*>(arrayPtr), szArray, 0);

    // Pick some random coordinates and use setPixelBuffer to store the given pixel there
    size_t coords[3];
    std::generate_n(coords, 3, [&] () { return rand() % IMG_DIMENSIONS; });
    size_t pixelIndex = (coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]);
    ::fwDataTools::helper::Image helper(image);
    helper.setPixelBuffer(pixelIndex, reinterpret_cast<uint8_t*>(pixelValue));

    // Test that the helper returned pixel value is correct
    ::fwDataTools::helper::ImageGetter constHelper(image);
    P* pixelHelperPtr = static_cast<P*>(helper.getPixelBuffer(coords[0], coords[1], coords[2]));
    if(std::is_floating_point<P>::value)
    {
        for(auto i = 0; i != N; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr[i], pixelValue[i],
                                                 0.00001);
        }
    }
    else
    {
        for(auto i = 0; i != N; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr[i], pixelValue[i]);
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::setPixelBufferTest()
{
    {
        uint8_t pGray[1] = {84};
        uint8_t pRGB[3]  = {42, 24, 21};
        setPixelBufferTestHelper<uint8_t, 1>(pGray);
        setPixelBufferTestHelper<uint8_t, 3>(pRGB);
    }
    {
        uint32_t pGray[1] = {0xDEADBEEF};
        uint32_t pRGB[3]  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        setPixelBufferTestHelper<uint32_t, 1>(pGray);
        setPixelBufferTestHelper<uint32_t, 3>(pRGB);
    }
    {
        float pGray[1] = {5423.2f};
        float pRGB[3]  = {42.0f, 1487.4f, 0.1445f};
        setPixelBufferTestHelper<float, 1>(pGray);
        setPixelBufferTestHelper<float, 3>(pRGB);
    }
    {
        double pGray[1] = {541.254981};
        double pRGB[3]  = {841.567, 6476.874, 0.187487};
        setPixelBufferTestHelper<double, 1>(pGray);
        setPixelBufferTestHelper<double, 3>(pRGB);
    }
}

} // namespace ut
} // namespace fwDataTools
