/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include <core/tools/random/Generator.hpp>

#include <data/Array.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <utestData/generator/Image.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::MedicalImageHelpersTest);
namespace medImHelper = sight::data::helper::MedicalImage;
namespace sight::data::tools
{

namespace ut
{

using core::tools::random::safeRand;

template<class P>
struct typeToPixelFormat;

template<>
struct typeToPixelFormat<std::array<unsigned char, 1> >
{
    static constexpr sight::data::Image::PixelFormat value = sight::data::Image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<unsigned char, 3> >
{
    static const auto value = sight::data::Image::PixelFormat::RGB;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 1> >
{
    static constexpr sight::data::Image::PixelFormat value = sight::data::Image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 3> >
{
    static const auto value = sight::data::Image::PixelFormat::RGB;
};

template<>
struct typeToPixelFormat<std::array<float, 1> >
{
    static constexpr sight::data::Image::PixelFormat value = sight::data::Image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<float, 3> >
{
    static const auto value = sight::data::Image::PixelFormat::RGB;
};

template<>
struct typeToPixelFormat<std::array<double, 1> >
{
    static const auto value = sight::data::Image::PixelFormat::GRAY_SCALE;
};

template<>
struct typeToPixelFormat<std::array<double, 3> >
{
    static const auto value = sight::data::Image::PixelFormat::RGB;
};

//------------------------------------------------------------------------------

sight::data::Image::sptr generateImage()
{
    data::Image::sptr image = data::Image::New();

    const data::Image::Size size       = {256, 150, 100};
    const data::Image::Spacing spacing = {1., 1., 0.5};
    const data::Image::Origin origin   = {0., 0., 0.};
    utestData::generator::Image::generateImage(
        image,
        size,
        spacing,
        origin,
        core::Type::UINT8,
        data::Image::PixelFormat::GRAY_SCALE
    );

    return image;
}

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
        const data::Image::Spacing spacing = {1., 1., 1.};
        const data::Image::Origin origin   = {0., 0., 0.};
        utestData::generator::Image::generateImage(
            image,
            size,
            spacing,
            origin,
            core::Type::get<Type>(),
            data::Image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();

        auto itr    = image->begin<Type>();
        auto itrEnd = image->end<Type>();

        for( ; itr != itrEnd ; ++itr)
        {
            *itr = MIN + static_cast<Type>(safeRand() % static_cast<int>(RANGE));
        }

        Type resMin, resMax;

        image->at<Type>(156) = MIN;
        image->at<Type>(245) = MAX;

        medImHelper::getMinMax(image, resMin, resMax);
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
        const data::Image::Spacing spacing = {1., 1., 1.};
        const data::Image::Origin origin   = {0., 0., 0.};
        utestData::generator::Image::generateImage(
            image,
            size,
            spacing,
            origin,
            core::Type::get<Type>(),
            data::Image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();

        auto itr          = image->begin<Type>();
        const auto itrEnd = image->end<Type>();

        for( ; itr != itrEnd ; ++itr)
        {
            *itr = MIN + static_cast<Type>(safeRand() % static_cast<int>(RANGE));
        }

        Type resMin, resMax;

        image->at<Type>(16)  = MIN;
        image->at<Type>(286) = MAX;

        medImHelper::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "min values are not equal",
            static_cast<double>(MIN),
            static_cast<double>(resMin),
            0.00001
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "max values are not equal",
            static_cast<double>(MAX),
            static_cast<double>(resMax),
            0.00001
        );
    }

    {
        // test of 2D image of type 'uint8'

        typedef std::uint8_t Type;

        const Type MIN   = 3;
        const Type MAX   = 245;
        const Type RANGE = MAX - MIN;

        data::Image::sptr image = data::Image::New();

        const data::Image::Size size       = {156, 208, 0};
        const data::Image::Spacing spacing = {1., 1., 0.};
        const data::Image::Origin origin   = {0., 0., 0.};
        utestData::generator::Image::generateImage(
            image,
            size,
            spacing,
            origin,
            core::Type::get<Type>(),
            data::Image::PixelFormat::GRAY_SCALE
        );

        const auto dumpLock = image->dump_lock();

        auto itr          = image->begin<Type>();
        const auto itrEnd = image->end<Type>();

        for( ; itr != itrEnd ; ++itr)
        {
            *itr = MIN + static_cast<Type>(safeRand() % static_cast<int>(RANGE));
        }

        Type resMin, resMax;

        image->at<Type>(5)    = MIN;
        image->at<Type>(2155) = MAX;

        medImHelper::getMinMax(image, resMin, resMax);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", MIN, resMin);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", MAX, resMax);
    }
}

// ------------------------------------------------------------------------------

template<class P>
data::Image::sptr createImageFromPixelBuffer()
{
    constexpr std::size_t IMG_DIMENSIONS = 100;
    using SubPixel = typename P::value_type;

    // Create a new image
    auto image             = data::Image::New();
    data::Image::Size size = {IMG_DIMENSIONS, IMG_DIMENSIONS, IMG_DIMENSIONS};
    image->resize(size, core::Type::get<SubPixel>(), typeToPixelFormat<P>::value);
    image->setSpacing({1., 1., 1.});
    image->setOrigin({0., 0., 0.});

    // Zero the buffer
    const auto dumpLock = image->dump_lock();
    std::fill(image->begin(), image->end(), 0);

    return image;
}

//------------------------------------------------------------------------------

template<class P>
void getPixelTestHelper(const P& pixelValue)
{
    using SubPixel = typename P::value_type;
    constexpr std::size_t N_COMPONENTS = std::tuple_size<P>::value;
    data::Image::sptr image            = createImageFromPixelBuffer<P>();
    const auto size                    = image->getSize();

    // Pick some random coordinates and store the given pixel there
    std::size_t coords[3];
    std::generate_n(coords, 3, [&](){return static_cast<std::size_t>(safeRand()) % size[0];});
    const auto dumpLock = image->dump_lock();
    auto imageBufferPtr = image->getBuffer();
    SubPixel* pixelPtr  = static_cast<SubPixel*>(imageBufferPtr)
                          + ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * N_COMPONENTS);
    std::copy(pixelValue.begin(), pixelValue.end(), pixelPtr);

    // Test that the helper returned pixel value is correct
    P value = image->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point<SubPixel>::value)
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "Pixel values are not equal",
                static_cast<double>(pixelValue[i]),
                static_cast<double>(value[i]),
                0.00001
            );
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

void MedicalImageHelpersTest::getPixelTest()
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
        std::array<float, 1> pGray = {5423.2f};
        std::array<float, 3> pRGB  = {42.0f, 1487.4f, 0.1445f};
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
    const auto size = image->getSize();

    // Pick some random coordinates and store the given pixel there
    std::size_t coords[3];
    std::generate_n(coords, 3, [&](){return static_cast<std::size_t>(safeRand()) % size[0];});
    const std::size_t pixelIndex = (coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]);
    const auto dumpLock          = image->dump_lock();
    image->setPixel(pixelIndex, reinterpret_cast<uint8_t*>(pixelValue.data()));

    // Test that the helper returned pixel value is correct
    data::Image::csptr constImage = image;
    const P value                 = constImage->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point<SubPixel>::value)
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "Pixel values are not equal",
                static_cast<double>(pixelValue[i]),
                static_cast<double>(value[i]),
                0.00001
            );
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

void MedicalImageHelpersTest::setPixelTest()
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
        std::array<float, 1> pGray = {5423.2f};
        std::array<float, 3> pRGB  = {42.0f, 1487.4f, 0.1445f};
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

void data::tools::ut::MedicalImageHelpersTest::isBufNull()
{
    auto image = createImageFromPixelBuffer<std::array<uint8_t, 3> >();

    {
        const auto dumpLock                   = image->dump_lock();
        const data::Image::BufferType* pixBuf =
            static_cast<data::Image::BufferType*>(image->getPixel(0));

        bool isNull = medImHelper::isBufNull(pixBuf, 3);
        CPPUNIT_ASSERT_EQUAL(true, isNull);

        isNull = medImHelper::isBufNull(pixBuf, 100);
        CPPUNIT_ASSERT_EQUAL(true, isNull);

        {
            std::array<float, 3> pixelValue = {42.0f, 1487.4f, 0.1445f};
            image->setPixel(0, reinterpret_cast<uint8_t*>(pixelValue.data()));

            isNull = medImHelper::isBufNull(pixBuf, 3);
            CPPUNIT_ASSERT_EQUAL(false, isNull);

            const data::Image::BufferType* pixBuf2 =
                static_cast<data::Image::BufferType*>(image->getPixel(10));

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

void MedicalImageHelpersTest::testLandmarks()
{
    data::Image::sptr image = generateImage();

    // get landrmaks (should NOT be nullptr)
    const auto landmarksNotNull = medImHelper::getLandmarks(*image);
    CPPUNIT_ASSERT(landmarksNotNull);

    /// Set landmarks
    data::Point::sptr p      = data::Point::New(1., 2., 3.);
    data::PointList::sptr pt = data::PointList::New();
    pt->pushBack(p);
    medImHelper::setLandmarks(*image, pt);

    // get landmarks (should NOT be nullptr)
    const auto landmarks = medImHelper::getLandmarks(*image);
    CPPUNIT_ASSERT(landmarks);

    const auto points = landmarks->getPoints();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), points.size());

    const auto point = points[0];

    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(p->getCoord()[i], point->getCoord()[i], std::numeric_limits<double>::epsilon());
    }
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::testImageValidity()
{
    // Valid image
    const auto valid_image = generateImage();
    // Invalid image (no buffer)
    const auto invalid_image = data::Image::New();

    // Check validity of each
    auto validity = medImHelper::checkImageValidity(*valid_image);

    CPPUNIT_ASSERT_EQUAL(true, validity);

    validity = medImHelper::checkImageValidity(*invalid_image);

    CPPUNIT_ASSERT_EQUAL(false, validity);
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::testSliceIndex()
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
        const auto image_no_slices = data::Image::New();
        auto orientation           = medImHelper::orientation_t::AXIAL;
        const auto index           = medImHelper::getSliceIndex(*image_no_slices, orientation);

        CPPUNIT_ASSERT_EQUAL(false, index.has_value());
    }
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::testDistances()
{
    const auto image = generateImage();

    const auto distances_null = medImHelper::getDistances(*image);

    CPPUNIT_ASSERT(!distances_null);

    data::Vector::sptr distances    = data::Vector::New();
    data::PointList::sptr pointList = data::PointList::New();
    pointList->getPoints().push_back(data::Point::New(0., 1., 2.));
    pointList->getPoints().push_back(data::Point::New(10., 11., 12.));

    distances->getContainer().push_back(pointList);

    medImHelper::setDistances(*image, distances);

    const auto distances_not_null = medImHelper::getDistances(*image);

    CPPUNIT_ASSERT(distances_not_null);

    // TODO: compare values ?
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::testDistanceVisibility()
{
    const auto image = generateImage();

    auto distance_visibility = medImHelper::getDistanceVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, distance_visibility);

    medImHelper::setDistanceVisibility(*image, false);

    distance_visibility = medImHelper::getDistanceVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, distance_visibility);
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::testLandmarksVisibility()
{
    const auto image = generateImage();

    auto lm_visibility = medImHelper::getLandmarksVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, lm_visibility);

    medImHelper::setLandmarksVisibility(*image, false);

    lm_visibility = medImHelper::getLandmarksVisibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, lm_visibility);
}

//------------------------------------------------------------------------------

void MedicalImageHelpersTest::testTransferFunction()
{
    data::Image::sptr image = generateImage();

    // get transfer function composite (should NOT be nullptr)
    const auto tfpool_not_null = medImHelper::getTransferFunction(*image);
    CPPUNIT_ASSERT(tfpool_not_null);

    // set transfer function composite

    data::Composite::sptr tfPool = data::Composite::New();

    medImHelper::setTransferFunction(*image, tfPool);

    const auto new_tf_pool = medImHelper::getTransferFunction(*image);
    CPPUNIT_ASSERT(new_tf_pool);

    CPPUNIT_ASSERT_EQUAL(tfPool, new_tf_pool);
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::data::tools
