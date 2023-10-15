/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "medical_image_helpers_test.hpp"

#include <core/tools/random/generator.hpp>

#include <data/array.hpp>
#include <data/helper/Histogram.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>

#include <utest_data/generator/image.hpp>

#include <cmath>
#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::medical_image_helpers_test);
namespace medImHelper = sight::data::helper::MedicalImage;
namespace sight::data::tools::ut
{

using core::tools::random::safe_rand;

template<class P>
struct typeToPixelFormat;

template<>
struct typeToPixelFormat<std::array<unsigned char, 1> >
{
    static constexpr sight::data::image::PixelFormat value = sight::data::image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<unsigned char, 3> >
{
    static const auto value = sight::data::image::PixelFormat::RGB;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 1> >
{
    static constexpr sight::data::image::PixelFormat value = sight::data::image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 3> >
{
    static const auto value = sight::data::image::PixelFormat::RGB;
};

template<>
struct typeToPixelFormat<std::array<float, 1> >
{
    static constexpr sight::data::image::PixelFormat value = sight::data::image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<float, 3> >
{
    static const auto value = sight::data::image::PixelFormat::RGB;
};

template<>
struct typeToPixelFormat<std::array<double, 1> >
{
    static const auto value = sight::data::image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<double, 3> >
{
    static const auto value = sight::data::image::PixelFormat::RGB;
};

//------------------------------------------------------------------------------

sight::data::image::sptr generateImage()
{
    data::image::sptr image = std::make_shared<data::image>();

    const data::image::Size size       = {256, 150, 100};
    const data::image::Spacing spacing = {1., 1., 0.5};
    const data::image::Origin origin   = {0., 0., 0.};
    utest_data::generator::image::generateImage(
        image,
        size,
        spacing,
        origin,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );

    return image;
}

// ------------------------------------------------------------------------------

void medical_image_helpers_test::setUp()
{
    // Set up context before running a test.
}

// ------------------------------------------------------------------------------

void medical_image_helpers_test::tearDown()
{
    // Clean up after the test run.
}

// ------------------------------------------------------------------------------

void medical_image_helpers_test::getMinMaxTest()
{
    {
        // Test on 3D image of type 'int16'
        using Type = std::int16_t;

        const Type MIN   = 45;
        const Type MAX   = 345;
        const Type RANGE = MAX - MIN;

        data::image::sptr image = std::make_shared<data::image>();

        const data::image::Size size       = {125, 110, 45};
        const data::image::Spacing spacing = {1., 1., 1.};
        const data::image::Origin origin   = {0., 0., 0.};
        utest_data::generator::image::generateImage(
            image,
            size,
            spacing,
            origin,
            core::type::get<Type>(),
            data::image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();

        auto itr    = image->begin<Type>();
        auto itrEnd = image->end<Type>();

        for( ; itr != itrEnd ; ++itr)
        {
            *itr = static_cast<Type>(MIN + (safe_rand() % RANGE));
        }

        Type resMin = 0;
        Type resMax = 0;

        image->at<Type>(156) = MIN;
        image->at<Type>(245) = MAX;

        medImHelper::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }

    {
        // Test on 3D image of type 'float'
        using Type = float;
        const Type MIN   = -12.3F;
        const Type MAX   = 18.2F;
        const Type RANGE = MAX - MIN;

        data::image::sptr image = std::make_shared<data::image>();

        const data::image::Size size       = {42, 34, 75};
        const data::image::Spacing spacing = {1., 1., 1.};
        const data::image::Origin origin   = {0., 0., 0.};
        utest_data::generator::image::generateImage(
            image,
            size,
            spacing,
            origin,
            core::type::get<Type>(),
            data::image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();

        auto itr          = image->begin<Type>();
        const auto itrEnd = image->end<Type>();

        for( ; itr != itrEnd ; ++itr)
        {
            *itr = MIN + static_cast<Type>(safe_rand() % static_cast<int>(RANGE));
        }

        Type resMin = NAN;
        Type resMax = NAN;

        image->at<Type>(16)  = MIN;
        image->at<Type>(286) = MAX;

        medImHelper::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }

    {
        // test of 2D image of type 'uint8'

        using Type = std::uint8_t;

        const Type MIN   = 3;
        const Type MAX   = 245;
        const Type RANGE = MAX - MIN;

        data::image::sptr image = std::make_shared<data::image>();

        const data::image::Size size       = {156, 208, 0};
        const data::image::Spacing spacing = {1., 1., 0.};
        const data::image::Origin origin   = {0., 0., 0.};
        utest_data::generator::image::generateImage(
            image,
            size,
            spacing,
            origin,
            core::type::get<Type>(),
            data::image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();

        auto itr          = image->begin<Type>();
        const auto itrEnd = image->end<Type>();

        for( ; itr != itrEnd ; ++itr)
        {
            *itr = MIN + static_cast<Type>(safe_rand() % static_cast<int>(RANGE));
        }

        Type resMin = 0;
        Type resMax = 0;

        image->at<Type>(5)    = MIN;
        image->at<Type>(2155) = MAX;

        medImHelper::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }
}

// ------------------------------------------------------------------------------

template<class P>
data::image::sptr createImageFromPixelBuffer()
{
    constexpr std::size_t IMG_DIMENSIONS = 100;
    using SubPixel = typename P::value_type;

    // Create a new image
    auto image             = std::make_shared<data::image>();
    data::image::Size size = {IMG_DIMENSIONS, IMG_DIMENSIONS, IMG_DIMENSIONS};
    image->resize(size, core::type::get<SubPixel>(), typeToPixelFormat<P>::value);
    image->setSpacing({1., 1., 1.});
    image->setOrigin({0., 0., 0.});

    // Zero the buffer
    const auto dumpLock = image->dump_lock();
    std::fill(image->begin(), image->end(), std::int8_t(0));

    return image;
}

//------------------------------------------------------------------------------

template<class P>
void getPixelTestHelper(const P& pixelValue)
{
    using SubPixel = typename P::value_type;
    constexpr std::size_t N_COMPONENTS = std::tuple_size<P>::value;
    data::image::sptr image            = createImageFromPixelBuffer<P>();
    const auto size                    = image->size();

    // Pick some random coordinates and store the given pixel there
    std::array<std::size_t, 3> coords {};
    std::generate(coords.begin(), coords.end(), [&](){return static_cast<std::size_t>(safe_rand()) % size[0];});
    const auto dumpLock  = image->dump_lock();
    auto* imageBufferPtr = image->buffer();
    SubPixel* pixelPtr   = static_cast<SubPixel*>(imageBufferPtr)
                           + ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * N_COMPONENTS);
    std::copy(pixelValue.begin(), pixelValue.end(), pixelPtr);

    // Test that the helper returned pixel value is correct
    P value = image->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point_v<SubPixel>)
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelValue[i], value[i]);
        }
    }
    else
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelValue[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::getPixelTest()
{
    {
        std::array<uint8_t, 1> pGray = {84};
        std::array<uint8_t, 3> pRGB  = {42, 24, 21};
        getPixelTestHelper(pGray);
        getPixelTestHelper(pRGB);
    }
    {
        std::array<uint32_t, 1> pGray = {0xDEADBEEF};
        std::array<uint32_t, 3> pRGB  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        getPixelTestHelper(pGray);
        getPixelTestHelper(pRGB);
    }
    {
        std::array<float, 1> pGray = {5423.2F};
        std::array<float, 3> pRGB  = {42.0F, 1487.4F, 0.1445F};
        getPixelTestHelper(pGray);
        getPixelTestHelper(pRGB);
    }
    {
        std::array<double, 1> pGray = {541.254981};
        std::array<double, 3> pRGB  = {841.567, 6476.874, 0.187487};
        getPixelTestHelper(pGray);
        getPixelTestHelper(pRGB);
    }
}

// ------------------------------------------------------------------------------

template<class P>
void setPixelTestHelper(P& pixelValue)
{
    using SubPixel = typename P::value_type;
    auto image      = createImageFromPixelBuffer<P>();
    const auto size = image->size();

    // Pick some random coordinates and store the given pixel there
    std::array<std::size_t, 3> coords {};
    std::generate(coords.begin(), coords.end(), [&](){return static_cast<std::size_t>(safe_rand()) % size[0];});
    const std::size_t pixelIndex = (coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]);
    const auto dumpLock          = image->dump_lock();
    image->setPixel(pixelIndex, reinterpret_cast<uint8_t*>(pixelValue.data()));

    // Test that the helper returned pixel value is correct
    data::image::csptr constImage = image;
    const P value                 = constImage->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point_v<SubPixel>)
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelValue[i], value[i]);
        }
    }
    else
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelValue[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::setPixelTest()
{
    {
        std::array<uint8_t, 1> pGray = {84};
        std::array<uint8_t, 3> pRGB  = {42, 24, 21};
        setPixelTestHelper(pGray);
        setPixelTestHelper(pRGB);
    }
    {
        std::array<uint32_t, 1> pGray = {0xDEADBEEF};
        std::array<uint32_t, 3> pRGB  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        setPixelTestHelper(pGray);
        setPixelTestHelper(pRGB);
    }
    {
        std::array<float, 1> pGray = {5423.2F};
        std::array<float, 3> pRGB  = {42.0F, 1487.4F, 0.1445F};
        setPixelTestHelper(pGray);
        setPixelTestHelper(pRGB);
    }
    {
        std::array<double, 1> pGray = {541.254981};
        std::array<double, 3> pRGB  = {841.567, 6476.874, 0.187487};
        setPixelTestHelper(pGray);
        setPixelTestHelper(pRGB);
    }
}

//------------------------------------------------------------------------------

void data::tools::ut::medical_image_helpers_test::isBufNull()
{
    auto image = createImageFromPixelBuffer<std::array<uint8_t, 3> >();

    {
        const auto dumpLock                   = image->dump_lock();
        const data::image::BufferType* pixBuf =
            static_cast<data::image::BufferType*>(image->getPixel(0));

        bool isNull = medImHelper::isBufNull(pixBuf, 3);
        CPPUNIT_ASSERT_EQUAL(true, isNull);

        isNull = medImHelper::isBufNull(pixBuf, 100);
        CPPUNIT_ASSERT_EQUAL(true, isNull);

        {
            std::array<float, 3> pixelValue = {42.0F, 1487.4F, 0.1445F};
            image->setPixel(0, reinterpret_cast<uint8_t*>(pixelValue.data()));

            isNull = medImHelper::isBufNull(pixBuf, 3);
            CPPUNIT_ASSERT_EQUAL(false, isNull);

            const data::image::BufferType* pixBuf2 =
                static_cast<data::image::BufferType*>(image->getPixel(10));

            isNull = medImHelper::isBufNull(pixBuf2, 3);
            CPPUNIT_ASSERT_EQUAL(true, isNull);

            image->setPixel(15, reinterpret_cast<uint8_t*>(pixelValue.data()));
            isNull = medImHelper::isBufNull(pixBuf2, 5 * 3);
            CPPUNIT_ASSERT_EQUAL(true, isNull);
            isNull = medImHelper::isBufNull(pixBuf2, 6 * 3);
            CPPUNIT_ASSERT_EQUAL(false, isNull);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testLandmarks()
{
    data::image::sptr image = generateImage();

    // get landrmaks (should NOT be nullptr)
    const auto landmarksNotNull = medImHelper::getLandmarks(*image);
    CPPUNIT_ASSERT(landmarksNotNull);

    /// Set landmarks
    data::point::sptr p       = std::make_shared<data::point>(1., 2., 3.);
    data::point_list::sptr pt = std::make_shared<data::point_list>();
    pt->pushBack(p);
    CPPUNIT_ASSERT_THROW(medImHelper::setLandmarks(*image, nullptr), data::exception);
    medImHelper::setLandmarks(*image, pt);

    // get landmarks (should NOT be nullptr)
    const auto landmarks = medImHelper::getLandmarks(*image);
    CPPUNIT_ASSERT(landmarks);

    const auto points = landmarks->getPoints();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), points.size());

    const auto& point = points[0];

    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(p->getCoord()[i], point->getCoord()[i]);
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testImageValidity()
{
    // Valid image
    const auto valid_image = generateImage();
    // Invalid image (no buffer)
    const auto invalid_image = std::make_shared<data::image>();

    // Check validity of each
    auto validity = medImHelper::checkImageValidity(*valid_image);

    CPPUNIT_ASSERT_EQUAL(true, validity);

    validity = medImHelper::checkImageValidity(*invalid_image);

    CPPUNIT_ASSERT_EQUAL(false, validity);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testSliceIndex()
{
    const auto image = generateImage();

    // AXIAL
    {
        auto orientation = medImHelper::orientation_t::AXIAL;

        auto index = medImHelper::getSliceIndex(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(50), index.value());

        medImHelper::setSliceIndex(*image, orientation, std::int64_t(35));

        index = medImHelper::getSliceIndex(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(35), index.value());
    }

    // SAGITTAL
    {
        auto orientation = medImHelper::orientation_t::SAGITTAL;

        auto index = medImHelper::getSliceIndex(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(128), index.value());

        medImHelper::setSliceIndex(*image, orientation, std::int64_t(0));

        index = medImHelper::getSliceIndex(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(0), index.value());
    }

    // FRONTAL
    {
        auto orientation = medImHelper::orientation_t::FRONTAL;

        auto index = medImHelper::getSliceIndex(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(75), index.value());

        medImHelper::setSliceIndex(*image, orientation, std::int64_t(17));

        index = medImHelper::getSliceIndex(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(17), index.value());
    }

    // No slice index
    {
        const auto image_no_slices = std::make_shared<data::image>();
        auto orientation           = medImHelper::orientation_t::AXIAL;
        const auto index           = medImHelper::getSliceIndex(*image_no_slices, orientation);

        CPPUNIT_ASSERT_EQUAL(false, index.has_value());
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testDistances()
{
    const auto image = generateImage();

    const auto distances_null = medImHelper::getDistances(*image);

    CPPUNIT_ASSERT(!distances_null);

    data::vector::sptr distances     = std::make_shared<data::vector>();
    data::point_list::sptr pointList = std::make_shared<data::point_list>();
    pointList->getPoints().push_back(std::make_shared<data::point>(0., 1., 2.));
    pointList->getPoints().push_back(std::make_shared<data::point>(10., 11., 12.));

    distances->push_back(pointList);

    medImHelper::setDistances(*image, distances);

    const auto distances_not_null = medImHelper::getDistances(*image);

    CPPUNIT_ASSERT(distances_not_null);

    // TODO: compare values ?
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testDistanceVisibility()
{
    const auto image = generateImage();

    auto distance_visibility = medImHelper::getDistanceVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, distance_visibility);

    medImHelper::setDistanceVisibility(*image, false);

    distance_visibility = medImHelper::getDistanceVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, distance_visibility);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testLandmarksVisibility()
{
    const auto image = generateImage();

    auto lm_visibility = medImHelper::getLandmarksVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, lm_visibility);

    medImHelper::setLandmarksVisibility(*image, false);

    lm_visibility = medImHelper::getLandmarksVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, lm_visibility);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::computeHistogram()
{
    using ImageType = std::int16_t;
    const std::size_t sizeX     = 50;
    const std::size_t sizeY     = 50;
    const std::size_t sizeZ     = 50;
    const std::size_t imageSize = sizeX * sizeY * sizeZ;

    // Configure data hierarchy
    data::image::sptr image = std::make_shared<data::image>();

    // Create image.
    image->resize({sizeX, sizeY, sizeZ}, core::type::INT16, data::image::GRAY_SCALE);

    const auto dumpLock = image->dump_lock();

    std::size_t count = 0;
    for(auto& itr : image->range<ImageType>())
    {
        if(count < imageSize / 4)
        {
            itr = -1000;
        }
        else if(count < imageSize / 2)
        {
            itr = 1;
        }
        else if(count < 3 * imageSize / 4)
        {
            itr = 500;
        }
        else if(count < imageSize)
        {
            itr = 3000;
        }

        ++count;
    }

    data::helper::Histogram histogram(image);
    histogram.compute();
    auto values = histogram.sample(1);

    CPPUNIT_ASSERT_EQUAL((std::size_t) (3000 - (-1000) + 1), values.size());
    CPPUNIT_ASSERT_EQUAL(-1000., histogram.min());
    CPPUNIT_ASSERT_EQUAL(3000., histogram.max());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 4., values[0], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 4., values[1001], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 4., values[1500], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 4., values[4000], 0.000001);

    values = histogram.sample(1001);

    CPPUNIT_ASSERT_EQUAL((std::size_t) 4, values.size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 4., values[0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 2., values[1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., values[2], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 4., values[3], 0.0001);

    count = 0;
    for(auto& itr : image->range<ImageType>())
    {
        if(count < imageSize / 4)
        {
            itr = -200;
        }
        else if(count < imageSize / 2)
        {
            itr = 80;
        }
        else if(count < 3 * imageSize / 4)
        {
            itr = 90;
        }
        else if(count < imageSize)
        {
            itr = 99;
        }

        ++count;
    }

    histogram.compute();
    values = histogram.sample(100);

    CPPUNIT_ASSERT_EQUAL((std::size_t) 3, values.size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1. / 4., values[0], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., values[1], 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3. / 4., values[2], 0.0001);
}

//------------------------------------------------------------------------------

} // namespace sight::data::tools::ut
