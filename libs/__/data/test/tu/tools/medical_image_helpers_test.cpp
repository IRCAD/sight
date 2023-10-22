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
#include <data/helper/histogram.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <utest_data/generator/image.hpp>

#include <cmath>
#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::medical_image_helpers_test);
namespace medImHelper = sight::data::helper::medical_image;
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

sight::data::image::sptr generate_image()
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

        const Type min   = 45;
        const Type max   = 345;
        const Type range = max - min;

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

        const auto dump_lock = image->dump_lock();

        auto itr     = image->begin<Type>();
        auto itr_end = image->end<Type>();

        for( ; itr != itr_end ; ++itr)
        {
            *itr = static_cast<Type>(min + (safe_rand() % range));
        }

        Type res_min = 0;
        Type res_max = 0;

        image->at<Type>(156) = min;
        image->at<Type>(245) = max;

        medImHelper::get_min_max(image, res_min, res_max);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", min, res_min);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", max, res_max);
    }

    {
        // Test on 3D image of type 'float'
        using Type = float;
        const Type min   = -12.3F;
        const Type max   = 18.2F;
        const Type range = max - min;

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

        const auto dump_lock = image->dump_lock();

        auto itr           = image->begin<Type>();
        const auto itr_end = image->end<Type>();

        for( ; itr != itr_end ; ++itr)
        {
            *itr = min + static_cast<Type>(safe_rand() % static_cast<int>(range));
        }

        Type res_min = NAN;
        Type res_max = NAN;

        image->at<Type>(16)  = min;
        image->at<Type>(286) = max;

        medImHelper::get_min_max(image, res_min, res_max);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", min, res_min);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", max, res_max);
    }

    {
        // test of 2D image of type 'uint8'

        using Type = std::uint8_t;

        const Type min   = 3;
        const Type max   = 245;
        const Type range = max - min;

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

        const auto dump_lock = image->dump_lock();

        auto itr           = image->begin<Type>();
        const auto itr_end = image->end<Type>();

        for( ; itr != itr_end ; ++itr)
        {
            *itr = min + static_cast<Type>(safe_rand() % static_cast<int>(range));
        }

        Type res_min = 0;
        Type res_max = 0;

        image->at<Type>(5)    = min;
        image->at<Type>(2155) = max;

        medImHelper::get_min_max(image, res_min, res_max);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", min, res_min);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", max, res_max);
    }
}

// ------------------------------------------------------------------------------

template<class P>
data::image::sptr create_image_from_pixel_buffer()
{
    constexpr std::size_t img_dimensions = 100;
    using SubPixel = typename P::value_type;

    // Create a new image
    auto image             = std::make_shared<data::image>();
    data::image::Size size = {img_dimensions, img_dimensions, img_dimensions};
    image->resize(size, core::type::get<SubPixel>(), typeToPixelFormat<P>::value);
    image->setSpacing({1., 1., 1.});
    image->setOrigin({0., 0., 0.});

    // Zero the buffer
    const auto dump_lock = image->dump_lock();
    std::fill(image->begin(), image->end(), std::int8_t(0));

    return image;
}

//------------------------------------------------------------------------------

template<class P>
void get_pixel_test_helper(const P& _pixel_value)
{
    using SubPixel = typename P::value_type;
    constexpr std::size_t n_components = std::tuple_size<P>::value;
    data::image::sptr image            = create_image_from_pixel_buffer<P>();
    const auto size                    = image->size();

    // Pick some random coordinates and store the given pixel there
    std::array<std::size_t, 3> coords {};
    std::generate(coords.begin(), coords.end(), [&](){return static_cast<std::size_t>(safe_rand()) % size[0];});
    const auto dump_lock   = image->dump_lock();
    auto* image_buffer_ptr = image->buffer();
    SubPixel* pixel_ptr    = static_cast<SubPixel*>(image_buffer_ptr)
                             + ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * n_components);
    std::copy(_pixel_value.begin(), _pixel_value.end(), pixel_ptr);

    // Test that the helper returned pixel value is correct
    P value = image->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point_v<SubPixel>)
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
    else
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::getPixelTest()
{
    {
        std::array<uint8_t, 1> p_gray = {84};
        std::array<uint8_t, 3> p_rgb  = {42, 24, 21};
        get_pixel_test_helper(p_gray);
        get_pixel_test_helper(p_rgb);
    }
    {
        std::array<uint32_t, 1> p_gray = {0xDEADBEEF};
        std::array<uint32_t, 3> p_rgb  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        get_pixel_test_helper(p_gray);
        get_pixel_test_helper(p_rgb);
    }
    {
        std::array<float, 1> p_gray = {5423.2F};
        std::array<float, 3> p_rgb  = {42.0F, 1487.4F, 0.1445F};
        get_pixel_test_helper(p_gray);
        get_pixel_test_helper(p_rgb);
    }
    {
        std::array<double, 1> p_gray = {541.254981};
        std::array<double, 3> p_rgb  = {841.567, 6476.874, 0.187487};
        get_pixel_test_helper(p_gray);
        get_pixel_test_helper(p_rgb);
    }
}

// ------------------------------------------------------------------------------

template<class P>
void set_pixel_test_helper(P& _pixel_value)
{
    using SubPixel = typename P::value_type;
    auto image      = create_image_from_pixel_buffer<P>();
    const auto size = image->size();

    // Pick some random coordinates and store the given pixel there
    std::array<std::size_t, 3> coords {};
    std::generate(coords.begin(), coords.end(), [&](){return static_cast<std::size_t>(safe_rand()) % size[0];});
    const std::size_t pixel_index = (coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]);
    const auto dump_lock          = image->dump_lock();
    image->setPixel(pixel_index, reinterpret_cast<uint8_t*>(_pixel_value.data()));

    // Test that the helper returned pixel value is correct
    data::image::csptr const_image = image;
    const P value                  = const_image->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point_v<SubPixel>)
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
    else
    {
        for(std::uint8_t i = 0 ; i != image->numComponents() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::setPixelTest()
{
    {
        std::array<uint8_t, 1> p_gray = {84};
        std::array<uint8_t, 3> p_rgb  = {42, 24, 21};
        set_pixel_test_helper(p_gray);
        set_pixel_test_helper(p_rgb);
    }
    {
        std::array<uint32_t, 1> p_gray = {0xDEADBEEF};
        std::array<uint32_t, 3> p_rgb  = {0xC0FFEE, 0xF100D, 0xDE7EC7ED};
        set_pixel_test_helper(p_gray);
        set_pixel_test_helper(p_rgb);
    }
    {
        std::array<float, 1> p_gray = {5423.2F};
        std::array<float, 3> p_rgb  = {42.0F, 1487.4F, 0.1445F};
        set_pixel_test_helper(p_gray);
        set_pixel_test_helper(p_rgb);
    }
    {
        std::array<double, 1> p_gray = {541.254981};
        std::array<double, 3> p_rgb  = {841.567, 6476.874, 0.187487};
        set_pixel_test_helper(p_gray);
        set_pixel_test_helper(p_rgb);
    }
}

//------------------------------------------------------------------------------

void data::tools::ut::medical_image_helpers_test::isBufNull()
{
    auto image = create_image_from_pixel_buffer<std::array<uint8_t, 3> >();

    {
        const auto dump_lock                 = image->dump_lock();
        const data::image::buffer_t* pix_buf =
            static_cast<data::image::buffer_t*>(image->getPixel(0));

        bool is_null = medImHelper::is_buf_null(pix_buf, 3);
        CPPUNIT_ASSERT_EQUAL(true, is_null);

        is_null = medImHelper::is_buf_null(pix_buf, 100);
        CPPUNIT_ASSERT_EQUAL(true, is_null);

        {
            std::array<float, 3> pixel_value = {42.0F, 1487.4F, 0.1445F};
            image->setPixel(0, reinterpret_cast<uint8_t*>(pixel_value.data()));

            is_null = medImHelper::is_buf_null(pix_buf, 3);
            CPPUNIT_ASSERT_EQUAL(false, is_null);

            const data::image::buffer_t* pix_buf2 =
                static_cast<data::image::buffer_t*>(image->getPixel(10));

            is_null = medImHelper::is_buf_null(pix_buf2, 3);
            CPPUNIT_ASSERT_EQUAL(true, is_null);

            image->setPixel(15, reinterpret_cast<uint8_t*>(pixel_value.data()));
            is_null = medImHelper::is_buf_null(pix_buf2, 5 * 3);
            CPPUNIT_ASSERT_EQUAL(true, is_null);
            is_null = medImHelper::is_buf_null(pix_buf2, 6 * 3);
            CPPUNIT_ASSERT_EQUAL(false, is_null);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testLandmarks()
{
    data::image::sptr image = generate_image();

    // get landrmaks (should NOT be nullptr)
    const auto landmarks_not_null = medImHelper::get_landmarks(*image);
    CPPUNIT_ASSERT(landmarks_not_null);

    /// Set landmarks
    data::point::sptr p       = std::make_shared<data::point>(1., 2., 3.);
    data::point_list::sptr pt = std::make_shared<data::point_list>();
    pt->pushBack(p);
    CPPUNIT_ASSERT_THROW(medImHelper::set_landmarks(*image, nullptr), data::exception);
    medImHelper::set_landmarks(*image, pt);

    // get landmarks (should NOT be nullptr)
    const auto landmarks = medImHelper::get_landmarks(*image);
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
    const auto valid_image = generate_image();
    // Invalid image (no buffer)
    const auto invalid_image = std::make_shared<data::image>();

    // Check validity of each
    auto validity = medImHelper::check_image_validity(*valid_image);

    CPPUNIT_ASSERT_EQUAL(true, validity);

    validity = medImHelper::check_image_validity(*invalid_image);

    CPPUNIT_ASSERT_EQUAL(false, validity);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testSliceIndex()
{
    const auto image = generate_image();

    // AXIAL
    {
        auto orientation = medImHelper::orientation_t::AXIAL;

        auto index = medImHelper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(50), index.value());

        medImHelper::set_slice_index(*image, orientation, std::int64_t(35));

        index = medImHelper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(35), index.value());
    }

    // SAGITTAL
    {
        auto orientation = medImHelper::orientation_t::SAGITTAL;

        auto index = medImHelper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(128), index.value());

        medImHelper::set_slice_index(*image, orientation, std::int64_t(0));

        index = medImHelper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(0), index.value());
    }

    // FRONTAL
    {
        auto orientation = medImHelper::orientation_t::FRONTAL;

        auto index = medImHelper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(75), index.value());

        medImHelper::set_slice_index(*image, orientation, std::int64_t(17));

        index = medImHelper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(17), index.value());
    }

    // No slice index
    {
        const auto image_no_slices = std::make_shared<data::image>();
        auto orientation           = medImHelper::orientation_t::AXIAL;
        const auto index           = medImHelper::get_slice_index(*image_no_slices, orientation);

        CPPUNIT_ASSERT_EQUAL(false, index.has_value());
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testDistances()
{
    const auto image = generate_image();

    const auto distances_null = medImHelper::get_distances(*image);

    CPPUNIT_ASSERT(!distances_null);

    data::vector::sptr distances      = std::make_shared<data::vector>();
    data::point_list::sptr point_list = std::make_shared<data::point_list>();
    point_list->getPoints().push_back(std::make_shared<data::point>(0., 1., 2.));
    point_list->getPoints().push_back(std::make_shared<data::point>(10., 11., 12.));

    distances->push_back(point_list);

    medImHelper::set_distances(*image, distances);

    const auto distances_not_null = medImHelper::get_distances(*image);

    CPPUNIT_ASSERT(distances_not_null);

    // TODO: compare values ?
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testDistanceVisibility()
{
    const auto image = generate_image();

    auto distance_visibility = medImHelper::get_distance_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, distance_visibility);

    medImHelper::set_distance_visibility(*image, false);

    distance_visibility = medImHelper::get_distance_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, distance_visibility);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::testLandmarksVisibility()
{
    const auto image = generate_image();

    auto lm_visibility = medImHelper::get_landmarks_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, lm_visibility);

    medImHelper::set_landmarks_visibility(*image, false);

    lm_visibility = medImHelper::get_landmarks_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, lm_visibility);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::computeHistogram()
{
    using image_t = std::int16_t;
    const std::size_t size_x     = 50;
    const std::size_t size_y     = 50;
    const std::size_t size_z     = 50;
    const std::size_t image_size = size_x * size_y * size_z;

    // Configure data hierarchy
    data::image::sptr image = std::make_shared<data::image>();

    // Create image.
    image->resize({size_x, size_y, size_z}, core::type::INT16, data::image::GRAY_SCALE);

    const auto dump_lock = image->dump_lock();

    std::size_t count = 0;
    for(auto& itr : image->range<image_t>())
    {
        if(count < image_size / 4)
        {
            itr = -1000;
        }
        else if(count < image_size / 2)
        {
            itr = 1;
        }
        else if(count < 3 * image_size / 4)
        {
            itr = 500;
        }
        else if(count < image_size)
        {
            itr = 3000;
        }

        ++count;
    }

    data::helper::histogram histogram(image);
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
    for(auto& itr : image->range<image_t>())
    {
        if(count < image_size / 4)
        {
            itr = -200;
        }
        else if(count < image_size / 2)
        {
            itr = 80;
        }
        else if(count < 3 * image_size / 4)
        {
            itr = 90;
        }
        else if(count < image_size)
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
