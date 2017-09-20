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

template <class P>
void getPixelBufferTestHelper(const P& pixelValue)
{
    constexpr size_t IMG_DIMENSIONS = 100;
    constexpr size_t N_COMPONENTS   = std::tuple_size<P>::value;
    using SubPixel = typename P::value_type;

    static_assert(N_COMPONENTS != 0, "Cannot test 0-dimensional pixel types");

    // Create a new image
    auto image = ::fwData::Image::New();
    ::fwData::Image::SizeType size(3);
    std::fill_n(size.begin(), 3, IMG_DIMENSIONS);
    image->allocate(size, ::fwTools::Type::create<SubPixel>(), N_COMPONENTS);
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
    auto imageBufferPtr = image->getDataArray()->getBufferObject()->getBuffer();
    SubPixel* pixelPtr  = static_cast<SubPixel*>(imageBufferPtr) +
                          ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * N_COMPONENTS);
    std::copy(pixelValue.begin(), pixelValue.end(), pixelPtr);

    // Test that the helper returned pixel value is correct
    ::fwDataTools::helper::ImageGetter constHelper(image);
    ::fwDataTools::helper::Image helper(image);
    SubPixel* pixelHelperPtr1 = static_cast<SubPixel*>(constHelper.getPixelBuffer(coords[0], coords[1], coords[2]));
    SubPixel* pixelHelperPtr2 = static_cast<SubPixel*>(helper.getPixelBuffer(coords[0], coords[1], coords[2]));
    if(std::is_floating_point<SubPixel>::value)
    {
        for(auto i = 0; i != N_COMPONENTS; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr1[i], pixelValue[i],
                                                 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr2[i], pixelValue[i],
                                                 0.00001);
        }
    }
    else
    {
        for(auto i = 0; i != N_COMPONENTS; ++i)
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
        std::array<uint8_t, 1> pGray = {84};
        std::array<uint8_t, 3> pRGB  = {42, 24, 21};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
    {
        std::array<uint32_t, 1> pGray = {0xDEADBEEF};
        std::array<uint32_t, 3> pRGB  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
    {
        std::array<float, 1> pGray = {5423.2f};
        std::array<float, 3> pRGB  = {42.0f, 1487.4f, 0.1445f};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
    {
        std::array<double, 1> pGray = {541.254981};
        std::array<double, 3> pRGB  = {841.567, 6476.874, 0.187487};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
}

// ------------------------------------------------------------------------------

template <class P>
void setPixelBufferTestHelper(P& pixelValue)
{
    constexpr size_t IMG_DIMENSIONS = 100;
    constexpr size_t N_COMPONENTS   = std::tuple_size<P>::value;
    using SubPixel = typename P::value_type;

    static_assert(N_COMPONENTS != 0, "Cannot test 0-dimensional pixel types");

    // Create a new image
    auto image = ::fwData::Image::New();
    ::fwData::Image::SizeType size(3);
    std::fill_n(size.begin(), 3, IMG_DIMENSIONS);
    image->allocate(size, ::fwTools::Type::create<SubPixel>(), N_COMPONENTS);
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
    helper.setPixelBuffer(pixelIndex, reinterpret_cast<uint8_t*>(pixelValue.data()));

    // Test that the helper returned pixel value is correct
    ::fwDataTools::helper::ImageGetter constHelper(image);
    SubPixel* pixelHelperPtr = static_cast<SubPixel*>(helper.getPixelBuffer(coords[0], coords[1], coords[2]));
    if(std::is_floating_point<SubPixel>::value)
    {
        for(auto i = 0; i != N_COMPONENTS; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr[i], pixelValue[i],
                                                 0.00001);
        }
    }
    else
    {
        for(auto i = 0; i != N_COMPONENTS; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr[i], pixelValue[i]);
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::setPixelBufferTest()
{
    {
        std::array<uint8_t, 1> pGray = {84};
        std::array<uint8_t, 3> pRGB  = {42, 24, 21};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
    {
        std::array<uint32_t, 1> pGray = {0xDEADBEEF};
        std::array<uint32_t, 3> pRGB  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
    {
        std::array<float, 1> pGray = {5423.2f};
        std::array<float, 3> pRGB  = {42.0f, 1487.4f, 0.1445f};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
    {
        std::array<double, 1> pGray = {541.254981};
        std::array<double, 3> pRGB  = {841.567, 6476.874, 0.187487};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
}

} // namespace ut
} // namespace fwDataTools
