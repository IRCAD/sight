/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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
namespace med_im_helper = sight::data::helper::medical_image;
namespace sight::data::tools::ut
{

using core::tools::random::safe_rand;

template<class P>
struct typeToPixelFormat;

template<>
struct typeToPixelFormat<std::array<unsigned char, 1> >
{
    static constexpr enum sight::data::image::pixel_format VALUE = sight::data::image::pixel_format::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<unsigned char, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format::rgb;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 1> >
{
    static constexpr enum sight::data::image::pixel_format VALUE = sight::data::image::pixel_format::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format::rgb;
};

template<>
struct typeToPixelFormat<std::array<float, 1> >
{
    static constexpr enum sight::data::image::pixel_format VALUE = sight::data::image::pixel_format::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<float, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format::rgb;
};

template<>
struct typeToPixelFormat<std::array<double, 1> >
{
    static const auto VALUE = sight::data::image::pixel_format::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<double, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format::rgb;
};

//------------------------------------------------------------------------------

sight::data::image::sptr generate_image()
{
    data::image::sptr image = std::make_shared<data::image>();

    const data::image::size_t size       = {256, 150, 100};
    const data::image::spacing_t spacing = {1., 1., 0.5};
    const data::image::origin_t origin   = {0., 0., 0.};
    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        core::type::UINT8,
        data::image::pixel_format::gray_scale
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

void medical_image_helpers_test::get_min_max_test()
{
    {
        // Test on 3D image of type 'int16'
        using type = std::int16_t;

        const type min   = 45;
        const type max   = 345;
        const type range = max - min;

        data::image::sptr image = std::make_shared<data::image>();

        const data::image::size_t size       = {125, 110, 45};
        const data::image::spacing_t spacing = {1., 1., 1.};
        const data::image::origin_t origin   = {0., 0., 0.};
        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            core::type::get<type>(),
            data::image::pixel_format::gray_scale
        );

        const auto dump_lock = image->dump_lock();

        auto itr     = image->begin<type>();
        auto itr_end = image->end<type>();

        for( ; itr != itr_end ; ++itr)
        {
            *itr = static_cast<type>(min + (safe_rand() % range));
        }

        type res_min = 0;
        type res_max = 0;

        image->at<type>(156) = min;
        image->at<type>(245) = max;

        med_im_helper::get_min_max(image, res_min, res_max);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", min, res_min);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", max, res_max);
    }

    {
        // Test on 3D image of type 'float'
        using type = float;
        const type min   = -12.3F;
        const type max   = 18.2F;
        const type range = max - min;

        data::image::sptr image = std::make_shared<data::image>();

        const data::image::size_t size       = {42, 34, 75};
        const data::image::spacing_t spacing = {1., 1., 1.};
        const data::image::origin_t origin   = {0., 0., 0.};
        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            core::type::get<type>(),
            data::image::pixel_format::gray_scale
        );

        const auto dump_lock = image->dump_lock();

        auto itr           = image->begin<type>();
        const auto itr_end = image->end<type>();

        for( ; itr != itr_end ; ++itr)
        {
            *itr = min + static_cast<type>(safe_rand() % static_cast<int>(range));
        }

        type res_min = NAN;
        type res_max = NAN;

        image->at<type>(16)  = min;
        image->at<type>(286) = max;

        med_im_helper::get_min_max(image, res_min, res_max);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", min, res_min);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", max, res_max);
    }

    {
        // test of 2D image of type 'uint8'

        using type = std::uint8_t;

        const type min   = 3;
        const type max   = 245;
        const type range = max - min;

        data::image::sptr image = std::make_shared<data::image>();

        const data::image::size_t size       = {156, 208, 0};
        const data::image::spacing_t spacing = {1., 1., 0.};
        const data::image::origin_t origin   = {0., 0., 0.};
        utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            core::type::get<type>(),
            data::image::pixel_format::gray_scale
        );

        const auto dump_lock = image->dump_lock();

        auto itr           = image->begin<type>();
        const auto itr_end = image->end<type>();

        for( ; itr != itr_end ; ++itr)
        {
            *itr = min + static_cast<type>(safe_rand() % static_cast<int>(range));
        }

        type res_min = 0;
        type res_max = 0;

        image->at<type>(5)    = min;
        image->at<type>(2155) = max;

        med_im_helper::get_min_max(image, res_min, res_max);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("min values are not equal", min, res_min);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("max values are not equal", max, res_max);
    }
}

// ------------------------------------------------------------------------------

template<class P>
data::image::sptr create_image_from_pixel_buffer()
{
    constexpr std::size_t img_dimensions = 100;
    using sub_pixel_t = typename P::value_type;

    // Create a new image
    auto image               = std::make_shared<data::image>();
    data::image::size_t size = {img_dimensions, img_dimensions, img_dimensions};
    image->resize(size, core::type::get<sub_pixel_t>(), typeToPixelFormat<P>::VALUE);
    image->set_spacing({1., 1., 1.});
    image->set_origin({0., 0., 0.});

    // Zero the buffer
    const auto dump_lock = image->dump_lock();
    std::fill(image->begin(), image->end(), std::int8_t(0));

    return image;
}

//------------------------------------------------------------------------------

template<class P>
void get_pixel_test_helper(const P& _pixel_value)
{
    using sub_pixel_t = typename P::value_type;
    constexpr std::size_t n_components = std::tuple_size<P>::value;
    data::image::sptr image            = create_image_from_pixel_buffer<P>();
    const auto size                    = image->size();

    // Pick some random coordinates and store the given pixel there
    std::array<std::size_t, 3> coords {};
    std::generate(coords.begin(), coords.end(), [&](){return static_cast<std::size_t>(safe_rand()) % size[0];});
    const auto dump_lock   = image->dump_lock();
    auto* image_buffer_ptr = image->buffer();
    sub_pixel_t* pixel_ptr = static_cast<sub_pixel_t*>(image_buffer_ptr)
                             + ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * n_components);
    std::copy(_pixel_value.begin(), _pixel_value.end(), pixel_ptr);

    // Test that the helper returned pixel value is correct
    P value = image->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point_v<sub_pixel_t>)
    {
        for(std::uint8_t i = 0 ; i != image->num_components() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
    else
    {
        for(std::uint8_t i = 0 ; i != image->num_components() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::get_pixel_test()
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
    using sub_pixel_t = typename P::value_type;
    auto image      = create_image_from_pixel_buffer<P>();
    const auto size = image->size();

    // Pick some random coordinates and store the given pixel there
    std::array<std::size_t, 3> coords {};
    std::generate(coords.begin(), coords.end(), [&](){return static_cast<std::size_t>(safe_rand()) % size[0];});
    const std::size_t pixel_index = (coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]);
    const auto dump_lock          = image->dump_lock();
    image->set_pixel(pixel_index, reinterpret_cast<uint8_t*>(_pixel_value.data()));

    // Test that the helper returned pixel value is correct
    data::image::csptr const_image = image;
    const P value                  = const_image->at<P>(coords[0], coords[1], coords[2]);
    if(std::is_floating_point_v<sub_pixel_t>)
    {
        for(std::uint8_t i = 0 ; i != image->num_components() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
    else
    {
        for(std::uint8_t i = 0 ; i != image->num_components() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", _pixel_value[i], value[i]);
        }
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::set_pixel_test()
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

void data::tools::ut::medical_image_helpers_test::is_buf_null()
{
    auto image = create_image_from_pixel_buffer<std::array<uint8_t, 3> >();

    {
        const auto dump_lock                 = image->dump_lock();
        const data::image::buffer_t* pix_buf =
            static_cast<data::image::buffer_t*>(image->get_pixel(0));

        bool is_null = med_im_helper::is_buf_null(pix_buf, 3);
        CPPUNIT_ASSERT_EQUAL(true, is_null);

        is_null = med_im_helper::is_buf_null(pix_buf, 100);
        CPPUNIT_ASSERT_EQUAL(true, is_null);

        {
            std::array<float, 3> pixel_value = {42.0F, 1487.4F, 0.1445F};
            image->set_pixel(0, reinterpret_cast<uint8_t*>(pixel_value.data()));

            is_null = med_im_helper::is_buf_null(pix_buf, 3);
            CPPUNIT_ASSERT_EQUAL(false, is_null);

            const data::image::buffer_t* pix_buf2 =
                static_cast<data::image::buffer_t*>(image->get_pixel(10));

            is_null = med_im_helper::is_buf_null(pix_buf2, 3);
            CPPUNIT_ASSERT_EQUAL(true, is_null);

            image->set_pixel(15, reinterpret_cast<uint8_t*>(pixel_value.data()));
            is_null = med_im_helper::is_buf_null(pix_buf2, 5 * 3);
            CPPUNIT_ASSERT_EQUAL(true, is_null);
            is_null = med_im_helper::is_buf_null(pix_buf2, 6 * 3);
            CPPUNIT_ASSERT_EQUAL(false, is_null);
        }
    }
}

//------------------------------------------------------------------------------

void data::tools::ut::medical_image_helpers_test::compute_voxel_indices()
{
    data::image image;

    const data::image::size_t size       = {567, 789, 39};
    const data::image::spacing_t spacing = {1.4, 0.8, 0.5};
    const data::image::origin_t origin   = {-18.3, 54., 230.};

    image.resize(size, core::type::INT16, data::image::gray_scale);
    image.set_spacing(spacing);
    image.set_origin(origin);

    {
        const auto indices = med_im_helper::compute_voxel_indices(image, {16, 543.65, 456.});

        med_im_helper::index_t expected_indices {25, 612, 452};
        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(expected_indices[i], indices[i]);
        }
    }
    {
        const auto indices = med_im_helper::compute_voxel_indices(image, {2., 34.4, 7.});
        med_im_helper::index_t expected_indices {15, -25, -446};
        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(expected_indices[i], indices[i]);
        }
    }
}

//------------------------------------------------------------------------------

void data::tools::ut::medical_image_helpers_test::compute_bounding_box()
{
    data::image image;

    const data::image::size_t size       = {567, 789, 39};
    const data::image::spacing_t spacing = {1.4, 0.8, 0.5};
    const data::image::origin_t origin   = {-18.3, 54., 230.};

    image.resize(size, core::type::INT16, data::image::gray_scale);
    image.set_spacing(spacing);
    image.set_origin(origin);

    const auto& [min, max] = med_im_helper::compute_bounding_box(image);

    med_im_helper::vec3_t expected_min {-18.3, 54., 230.};
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expected_min[i], min[i]);
    }

    med_im_helper::vec3_t expected_max {775.5, 685.2, 249.5};
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expected_max[i], max[i]);
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::test_landmarks()
{
    data::image::sptr image = generate_image();

    // get landrmaks (should NOT be nullptr)
    const auto landmarks_not_null = med_im_helper::get_landmarks(*image);
    CPPUNIT_ASSERT(landmarks_not_null);

    /// Set landmarks
    data::point::sptr p       = std::make_shared<data::point>(1., 2., 3.);
    data::point_list::sptr pt = std::make_shared<data::point_list>();
    pt->push_back(p);
    CPPUNIT_ASSERT_THROW(med_im_helper::set_landmarks(*image, nullptr), data::exception);
    med_im_helper::set_landmarks(*image, pt);

    // get landmarks (should NOT be nullptr)
    const auto landmarks = med_im_helper::get_landmarks(*image);
    CPPUNIT_ASSERT(landmarks);

    const auto points = landmarks->get_points();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), points.size());

    const auto& point = points[0];

    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(p->get_coord()[i], point->get_coord()[i]);
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::test_image_validity()
{
    // Valid image
    const auto valid_image = generate_image();
    // Invalid image (no buffer)
    const auto invalid_image = std::make_shared<data::image>();

    // Check validity of each
    auto validity = med_im_helper::check_image_validity(*valid_image);

    CPPUNIT_ASSERT_EQUAL(true, validity);

    validity = med_im_helper::check_image_validity(*invalid_image);

    CPPUNIT_ASSERT_EQUAL(false, validity);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::test_slice_index()
{
    const auto image = generate_image();

    // AXIAL
    {
        auto orientation = med_im_helper::orientation_t::axial;

        auto index = med_im_helper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(50), index.value());

        med_im_helper::set_slice_index(*image, orientation, std::int64_t(35));

        index = med_im_helper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(35), index.value());
    }

    // SAGITTAL
    {
        auto orientation = med_im_helper::orientation_t::sagittal;

        auto index = med_im_helper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(128), index.value());

        med_im_helper::set_slice_index(*image, orientation, std::int64_t(0));

        index = med_im_helper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(0), index.value());
    }

    // FRONTAL
    {
        auto orientation = med_im_helper::orientation_t::frontal;

        auto index = med_im_helper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(75), index.value());

        med_im_helper::set_slice_index(*image, orientation, std::int64_t(17));

        index = med_im_helper::get_slice_index(*image, orientation);

        CPPUNIT_ASSERT_EQUAL(true, index.has_value());

        CPPUNIT_ASSERT_EQUAL(std::int64_t(17), index.value());
    }

    // No slice index
    {
        const auto image_no_slices = std::make_shared<data::image>();
        auto orientation           = med_im_helper::orientation_t::axial;
        const auto index           = med_im_helper::get_slice_index(*image_no_slices, orientation);

        CPPUNIT_ASSERT_EQUAL(false, index.has_value());
    }
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::test_distances()
{
    const auto image = generate_image();

    const auto distances_null = med_im_helper::get_distances(*image);

    CPPUNIT_ASSERT(!distances_null);

    data::vector::sptr distances      = std::make_shared<data::vector>();
    data::point_list::sptr point_list = std::make_shared<data::point_list>();
    point_list->get_points().push_back(std::make_shared<data::point>(0., 1., 2.));
    point_list->get_points().push_back(std::make_shared<data::point>(10., 11., 12.));

    distances->push_back(point_list);

    med_im_helper::set_distances(*image, distances);

    const auto distances_not_null = med_im_helper::get_distances(*image);

    CPPUNIT_ASSERT(distances_not_null);

    // TODO: compare values ?
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::test_distance_visibility()
{
    const auto image = generate_image();

    auto distance_visibility = med_im_helper::get_distance_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, distance_visibility);

    med_im_helper::set_distance_visibility(*image, false);

    distance_visibility = med_im_helper::get_distance_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, distance_visibility);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::test_landmarks_visibility()
{
    const auto image = generate_image();

    auto lm_visibility = med_im_helper::get_landmarks_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(true, lm_visibility);

    med_im_helper::set_landmarks_visibility(*image, false);

    lm_visibility = med_im_helper::get_landmarks_visibility(*image);

    CPPUNIT_ASSERT_EQUAL(false, lm_visibility);
}

//------------------------------------------------------------------------------

void medical_image_helpers_test::compute_histogram()
{
    using image_t = std::int16_t;
    const std::size_t size_x     = 50;
    const std::size_t size_y     = 50;
    const std::size_t size_z     = 50;
    const std::size_t image_size = size_x * size_y * size_z;

    // Configure data hierarchy
    data::image::sptr image = std::make_shared<data::image>();

    // Create image.
    image->resize({size_x, size_y, size_z}, core::type::INT16, data::image::gray_scale);

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
