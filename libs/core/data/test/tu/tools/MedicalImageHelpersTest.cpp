/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "MedicalImageHelpersTest.hpp"

#include <data/Array.hpp>
#include <data/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>

#include <utestData/generator/Image.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::data::tools::ut::MedicalImageHelpersTest );

namespace sight::data::tools
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

        data::Image::sptr image = data::Image::New();

        const data::Image::Size size       = {125, 110, 45};
        const data::Image::Spacing spacing = { 1., 1., 1.};
        const data::Image::Origin origin   = {0., 0., 0.};
        utestData::generator::Image::generateImage(image, size, spacing, origin, core::tools::Type::create<Type>(),
                                                   data::Image::PixelFormat::GRAY_SCALE);

        const auto dumpLock = image->lock();

        auto itr    = image->begin<Type>();
        auto itrEnd = image->end<Type>();

        for (; itr != itrEnd; ++itr)
        {
            *itr = MIN + static_cast<Type>(std::rand()%static_cast<int>(RANGE));
        }

        Type resMin, resMax;

        image->at<Type>(156) = MIN;
        image->at<Type>(245) = MAX;

        data::fieldHelper::MedicalImageHelpers::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }

    {
        // Test on 3D image of type 'float'
        typedef float Type;
        const Type MIN   = -12.3f;
        const Type MAX   = 18.2f;
        const Type RANGE = MAX - MIN;

        data::Image::sptr image = data::Image::New();

        const data::Image::Size size       = {42, 34, 75};
        const data::Image::Spacing spacing = { 1., 1., 1.};
        const data::Image::Origin origin   = {0., 0., 0.};
        utestData::generator::Image::generateImage(image, size, spacing, origin, core::tools::Type::create<Type>(),
                                                   data::Image::PixelFormat::GRAY_SCALE);

        const auto dumpLock = image->lock();

        auto itr          = image->begin<Type>();
        const auto itrEnd = image->end<Type>();

        for (; itr != itrEnd; ++itr)
        {
            *itr = MIN + static_cast<Type>(std::rand()%static_cast<int>(RANGE));
        }

        Type resMin, resMax;

        image->at<Type>(16)  = MIN;
        image->at<Type>(286) = MAX;

        data::fieldHelper::MedicalImageHelpers::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("min values are not equal",
                                             static_cast<double>(MIN), static_cast<double>(resMin), 0.00001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("max values are not equal",
                                             static_cast<double>(MAX), static_cast<double>(resMax), 0.00001);
    }

    {
        // test of 2D image of type 'uint8'

        typedef std::uint8_t Type;

        const Type MIN   = 3;
        const Type MAX   = 245;
        const Type RANGE = MAX - MIN;

        data::Image::sptr image = data::Image::New();

        const data::Image::Size size       = {156, 208, 0};
        const data::Image::Spacing spacing = { 1., 1., 0.};
        const data::Image::Origin origin   = {0., 0., 0.};
        utestData::generator::Image::generateImage(image, size, spacing, origin, core::tools::Type::create<Type>(),
                                                   data::Image::PixelFormat::GRAY_SCALE);

        const auto dumpLock = image->lock();

        auto itr          = image->begin<Type>();
        const auto itrEnd = image->end<Type>();

        for (; itr != itrEnd; ++itr)
        {
            *itr = MIN + static_cast<Type>(std::rand()%static_cast<int>(RANGE));
        }

        Type resMin, resMax;

        image->at<Type>(5)    = MIN;
        image->at<Type>(2155) = MAX;

        data::fieldHelper::MedicalImageHelpers::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }
}

// ------------------------------------------------------------------------------

template <class P>
data::Image::sptr createImageFromPixelBuffer()
{
    constexpr size_t IMG_DIMENSIONS = 100;
    constexpr size_t N_COMPONENTS   = std::tuple_size<P>::value;
    using SubPixel = typename P::value_type;

    static_assert(N_COMPONENTS != 0, "Cannot test 0-dimensional pixel types");

    // Create a new image
    auto image             = data::Image::New();
    data::Image::Size size = {IMG_DIMENSIONS, IMG_DIMENSIONS, IMG_DIMENSIONS};
    image->setSize2(size);
    image->setType(core::tools::Type::create<SubPixel>());
    image->setNumberOfComponents(N_COMPONENTS);
    image->resize();
    image->setSpacing2({1., 1., 1.});
    image->setOrigin2({0., 0., 0.});

    // Zero the buffer
    const auto dumpLock = image->lock();
    std::fill(image->begin(), image->end(), 0);

    return image;
}

//------------------------------------------------------------------------------

template <class P>
void getPixelBufferTestHelper(const P& pixelValue)
{
    using SubPixel = typename P::value_type;
    constexpr size_t N_COMPONENTS = std::tuple_size<P>::value;
    data::Image::sptr image       = createImageFromPixelBuffer<P>();
    const auto size               = image->getSize2();

    // Pick some random coordinates and store the given pixel there
    size_t coords[3];
    std::generate_n(coords, 3, [&] () { return static_cast<size_t>(rand()) % size[0]; });
    const auto dumpLock = image->lock();
    auto imageBufferPtr = image->getBuffer();
    SubPixel* pixelPtr  = static_cast<SubPixel*>(imageBufferPtr) +
                          ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * N_COMPONENTS);
    std::copy(pixelValue.begin(), pixelValue.end(), pixelPtr);

    // Test that the helper returned pixel value is correct
    P value = image->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point<SubPixel>::value)
    {
        for(std::uint8_t i = 0; i != image->getNumberOfComponents(); ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal",
                                                 static_cast<double>(pixelValue[i]),
                                                 static_cast<double>(value[i]),
                                                 0.00001);
        }
    }
    else
    {
        for(std::uint8_t i = 0; i != image->getNumberOfComponents(); ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelValue[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::getPixelBufferTest()
{
    {
        std::array<uint8_t, 1> pGray = {{ 84 }};
        std::array<uint8_t, 3> pRGB  = {{ 42, 24, 21 }};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
    {
        std::array<uint32_t, 1> pGray = {{ 0xDEADBEEF }};
        std::array<uint32_t, 3> pRGB  = {{ 0xC0FFEE, 0xF100D, 0xDE7EC7ED }};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
    {
        std::array<float, 1> pGray = {{ 5423.2f }};
        std::array<float, 3> pRGB  = {{ 42.0f, 1487.4f, 0.1445f }};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
    {
        std::array<double, 1> pGray = {{ 541.254981 }};
        std::array<double, 3> pRGB  = {{ 841.567, 6476.874, 0.187487 }};
        getPixelBufferTestHelper(pGray);
        getPixelBufferTestHelper(pRGB);
    }
}

// ------------------------------------------------------------------------------

template <class P>
void setPixelBufferTestHelper(P& pixelValue)
{
    using SubPixel = typename P::value_type;
    auto image      = createImageFromPixelBuffer<P>();
    const auto size = image->getSize2();

    // Pick some random coordinates and store the given pixel there
    size_t coords[3];
    std::generate_n(coords, 3, [&] () { return static_cast<size_t>(rand()) % size[0]; });
    const size_t pixelIndex = (coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]);
    const auto dumpLock     = image->lock();
    image->setPixelBuffer(pixelIndex, reinterpret_cast<uint8_t*>(pixelValue.data()));

    // Test that the helper returned pixel value is correct
    data::Image::csptr constImage = image;
    const P value                 = constImage->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point<SubPixel>::value)
    {
        for(std::uint8_t i = 0; i != image->getNumberOfComponents(); ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal",
                                                 static_cast<double>(pixelValue[i]),
                                                 static_cast<double>(value[i]),
                                                 0.00001);
        }
    }
    else
    {
        for(std::uint8_t i = 0; i != image->getNumberOfComponents(); ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelValue[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::setPixelBufferTest()
{
    {
        std::array<uint8_t, 1> pGray = {{ 84 }};
        std::array<uint8_t, 3> pRGB  = {{ 42, 24, 21 }};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
    {
        std::array<uint32_t, 1> pGray = {{ 0xDEADBEEF }};
        std::array<uint32_t, 3> pRGB  = {{ 0xC0FFEE, 0xF100D, 0xDE7EC7ED }};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
    {
        std::array<float, 1> pGray = {{ 5423.2f }};
        std::array<float, 3> pRGB  = {{ 42.0f, 1487.4f, 0.1445f }};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
    {
        std::array<double, 1> pGray = {{ 541.254981 }};
        std::array<double, 3> pRGB  = {{ 841.567, 6476.874, 0.187487 }};
        setPixelBufferTestHelper(pGray);
        setPixelBufferTestHelper(pRGB);
    }
}

//------------------------------------------------------------------------------

void data::tools::ut::MedicalImageHelpersTest::isBufNull()
{
    auto image = createImageFromPixelBuffer<std::array<uint8_t, 3> >();

    {
        const auto dumpLock                   = image->lock();
        const data::Image::BufferType* pixBuf =
            static_cast< data::Image::BufferType* >(image->getPixelBuffer(0));

        bool isNull = data::fieldHelper::MedicalImageHelpers::isBufNull(pixBuf, 3);
        CPPUNIT_ASSERT_EQUAL(true, isNull);

        isNull = data::fieldHelper::MedicalImageHelpers::isBufNull(pixBuf, 100);
        CPPUNIT_ASSERT_EQUAL(true, isNull);

        {
            std::array<float, 3> pixelValue = {{ 42.0f, 1487.4f, 0.1445f }};
            image->setPixelBuffer(0, reinterpret_cast<uint8_t*>(pixelValue.data()));

            isNull = data::fieldHelper::MedicalImageHelpers::isBufNull(pixBuf, 3);
            CPPUNIT_ASSERT_EQUAL(false, isNull);

            const data::Image::BufferType* pixBuf =
                static_cast< data::Image::BufferType* >(image->getPixelBuffer(10));

            isNull = data::fieldHelper::MedicalImageHelpers::isBufNull(pixBuf, 3);
            CPPUNIT_ASSERT_EQUAL(true, isNull);

            image->setPixelBuffer(15, reinterpret_cast<uint8_t*>(pixelValue.data()));
            isNull = data::fieldHelper::MedicalImageHelpers::isBufNull(pixBuf, 5*3);
            CPPUNIT_ASSERT_EQUAL(true, isNull);
            isNull = data::fieldHelper::MedicalImageHelpers::isBufNull(pixBuf, 6*3);
            CPPUNIT_ASSERT_EQUAL(false, isNull);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace sight::data::tools
