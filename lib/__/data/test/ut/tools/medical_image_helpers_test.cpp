/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#include <core/tools/random/generator.hpp>

#include <data/array.hpp>
#include <data/helper/histogram.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <cmath>
#include <cstdint>

namespace  med_im_helper = sight::data::helper::medical_image;
using sight::core::tools::random::safe_rand;

template<class P>
struct typeToPixelFormat;

template<>
struct typeToPixelFormat<std::array<unsigned char, 1> >
{
    static constexpr enum sight::data::image::pixel_format_t VALUE = sight::data::image::pixel_format_t::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<unsigned char, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format_t::rgb;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 1> >
{
    static constexpr enum sight::data::image::pixel_format_t VALUE = sight::data::image::pixel_format_t::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<unsigned int, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format_t::rgb;
};

template<>
struct typeToPixelFormat<std::array<float, 1> >
{
    static constexpr enum sight::data::image::pixel_format_t VALUE = sight::data::image::pixel_format_t::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<float, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format_t::rgb;
};

template<>
struct typeToPixelFormat<std::array<double, 1> >
{
    static const auto VALUE = sight::data::image::pixel_format_t::gray_scale;
};

template<>
struct typeToPixelFormat<std::array<double, 3> >
{
    static const auto VALUE = sight::data::image::pixel_format_t::rgb;
};

//------------------------------------------------------------------------------

static sight::data::image::sptr generate_image()
{
    sight::data::image::sptr image = std::make_shared<sight::data::image>();

    const sight::data::image::size_t size               = {256, 150, 100};
    const sight::data::image::spacing_t spacing         = {1., 1., 0.5};
    const sight::data::image::origin_t origin           = {0., 0., 0.};
    const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    sight::utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        sight::core::type::UINT8,
        sight::data::image::pixel_format_t::gray_scale
    );

    return image;
}

TEST_SUITE("sight::data::tools::ut")
{
// ------------------------------------------------------------------------------

    TEST_CASE("get_min_max")
    {
        {
            // Test on 3D image of type 'int16'
            using type = std::int16_t;

            const type min   = 45;
            const type max   = 345;
            const type range = max - min;

            sight::data::image::sptr image = std::make_shared<sight::data::image>();

            const sight::data::image::size_t size               = {125, 110, 45};
            const sight::data::image::spacing_t spacing         = {1., 1., 1.};
            const sight::data::image::origin_t origin           = {0., 0., 0.};
            const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};

            sight::utest_data::generator::image::generate_image(
                image,
                size,
                spacing,
                origin,
                orientation,
                sight::core::type::get<type>(),
                sight::data::image::pixel_format_t::gray_scale
            );

            const auto dump_lock = image->dump_lock();

            auto itr     = image->begin<type>();
            auto itr_end = image->end<type>();

            for( ; itr != itr_end ; ++itr)
            {
                *itr = static_cast<type>(min + (safe_rand() % range));
            }

            image->at<type>(156) = min;
            image->at<type>(245) = max;

            const auto& [res_min, res_max] = med_im_helper::get_min_max<type>(image);
            CHECK_MESSAGE(min == res_min, "min values are not equal");
            CHECK_MESSAGE(max == res_max, "max values are not equal");
        }

        {
            // Test on 3D image of type 'float'
            using type = float;
            const type min   = -12.3F;
            const type max   = 18.2F;
            const type range = max - min;

            sight::data::image::sptr image = std::make_shared<sight::data::image>();

            const sight::data::image::size_t size               = {42, 34, 75};
            const sight::data::image::spacing_t spacing         = {1., 1., 1.};
            const sight::data::image::origin_t origin           = {0., 0., 0.};
            const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};

            sight::utest_data::generator::image::generate_image(
                image,
                size,
                spacing,
                origin,
                orientation,
                sight::core::type::get<type>(),
                sight::data::image::pixel_format_t::gray_scale
            );

            const auto dump_lock = image->dump_lock();

            auto itr           = image->begin<type>();
            const auto itr_end = image->end<type>();

            for( ; itr != itr_end ; ++itr)
            {
                *itr = min + static_cast<type>(safe_rand() % static_cast<int>(range));
            }

            image->at<type>(16)  = min;
            image->at<type>(286) = max;

            const auto& [res_min, res_max] = med_im_helper::get_min_max<type>(image);
            CHECK_MESSAGE(min == res_min, "min values are not equal");
            CHECK_MESSAGE(max == res_max, "max values are not equal");
        }

        {
            // test of 2D image of type 'uint8'

            using type = std::uint8_t;

            const type min   = 3;
            const type max   = 245;
            const type range = max - min;

            sight::data::image::sptr image = std::make_shared<sight::data::image>();

            const sight::data::image::size_t size               = {156, 208, 0};
            const sight::data::image::spacing_t spacing         = {1., 1., 0.};
            const sight::data::image::origin_t origin           = {0., 0., 0.};
            const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};

            sight::utest_data::generator::image::generate_image(
                image,
                size,
                spacing,
                origin,
                orientation,
                sight::core::type::get<type>(),
                sight::data::image::pixel_format_t::gray_scale
            );

            const auto dump_lock = image->dump_lock();

            auto itr           = image->begin<type>();
            const auto itr_end = image->end<type>();

            for( ; itr != itr_end ; ++itr)
            {
                *itr = min + static_cast<type>(safe_rand() % static_cast<int>(range));
            }

            image->at<type>(5)    = min;
            image->at<type>(2155) = max;

            const auto& [res_min, res_max] = med_im_helper::get_min_max<type>(image);
            CHECK_MESSAGE(min == res_min, "min values are not equal");
            CHECK_MESSAGE(max == res_max, "max values are not equal");
        }
    }

// ------------------------------------------------------------------------------

    template<class P>
    static sight::data::image::sptr create_image_from_pixel_buffer()
    {
        constexpr std::size_t img_dimensions = 100;
        using sub_pixel_t = typename P::value_type;

        // Create a new image
        auto image                      = std::make_shared<sight::data::image>();
        sight::data::image::size_t size = {img_dimensions, img_dimensions, img_dimensions};
        image->resize(size, sight::core::type::get<sub_pixel_t>(), typeToPixelFormat<P>::VALUE);
        image->set_spacing({1., 1., 1.});
        image->set_origin({0., 0., 0.});

        // Zero the buffer
        const auto dump_lock = image->dump_lock();
        std::fill(image->begin(), image->end(), std::int8_t(0));

        return image;
    }

//------------------------------------------------------------------------------

    template<class P>
    static void get_pixel_test_helper(const P& _pixel_value)
    {
        using sub_pixel_t = typename P::value_type;
        constexpr std::size_t n_components = std::tuple_size<P>::value;
        sight::data::image::sptr image     = create_image_from_pixel_buffer<P>();
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
                CHECK_MESSAGE(_pixel_value[i] == value[i], "Pixel values are not equal");
            }
        }
        else
        {
            for(std::uint8_t i = 0 ; i != image->num_components() ; ++i)
            {
                CHECK_MESSAGE(_pixel_value[i] == value[i], "Pixel values are not equal");
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("get_pixel")
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
    static void set_pixel_test_helper(P& _pixel_value)
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
        sight::data::image::csptr const_image = image;
        const P value                         = const_image->at<P>(coords[0], coords[1], coords[2]);
        if(std::is_floating_point_v<sub_pixel_t>)
        {
            for(std::uint8_t i = 0 ; i != image->num_components() ; ++i)
            {
                CHECK_MESSAGE(_pixel_value[i] == value[i], "Pixel values are not equal");
            }
        }
        else
        {
            for(std::uint8_t i = 0 ; i != image->num_components() ; ++i)
            {
                CHECK_MESSAGE(_pixel_value[i] == value[i], "Pixel values are not equal");
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("set_pixel")
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

    TEST_CASE("test_landmarks")
    {
        sight::data::image::sptr image = generate_image();

        // get landrmaks (should NOT be nullptr)
        const auto landmarks_not_null = med_im_helper::get_landmarks(*image);
        CHECK(landmarks_not_null);

        /// Set landmarks
        sight::data::point::sptr p       = std::make_shared<sight::data::point>(1., 2., 3.);
        sight::data::point_list::sptr pt = std::make_shared<sight::data::point_list>();
        pt->push_back(p);
        CHECK_THROWS_AS(med_im_helper::set_landmarks(*image, nullptr), sight::data::exception);
        med_im_helper::set_landmarks(*image, pt);

        // get landmarks (should NOT be nullptr)
        const auto landmarks = med_im_helper::get_landmarks(*image);
        CHECK(landmarks);

        const auto points = landmarks->get_points();

        CHECK_EQ(std::size_t(1), points.size());

        const auto& point = points[0];

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            CHECK_EQ((*p)[i], (*point)[i]);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("test_image_validity")
    {
        // Valid image
        const auto valid_image = generate_image();
        // Invalid image (no buffer)
        const auto invalid_image = std::make_shared<sight::data::image>();

        // Check validity of each
        auto validity = med_im_helper::check_image_validity(*valid_image);

        CHECK_EQ(true, validity);

        validity = med_im_helper::check_image_validity(*invalid_image);

        CHECK_EQ(false, validity);
    }

//------------------------------------------------------------------------------
    TEST_CASE("test_slice_position")
    {
        const auto image = generate_image();
        // AXIAL
        {
            auto orientation = med_im_helper::axis_t::axial;

            auto position = med_im_helper::get_slice_position(*image, orientation);

            CHECK_EQ(true, position.has_value());

            CHECK_EQ(double(25), position.value());

            med_im_helper::set_slice_position(*image, orientation, std::double_t(35.0));

            position = med_im_helper::get_slice_position(*image, orientation);

            CHECK_EQ(true, position.has_value());

            CHECK_EQ(double(35), position.value());
        }

        // SAGITTAL
        {
            auto orientation = med_im_helper::axis_t::sagittal;

            auto index = med_im_helper::get_slice_position(*image, orientation);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(double(128), index.value());

            med_im_helper::set_slice_position(*image, orientation, std::double_t(0.0));

            index = med_im_helper::get_slice_position(*image, orientation);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(double(0), index.value());
        }
        // FRONTAL
        {
            auto orientation = med_im_helper::axis_t::frontal;

            auto index = med_im_helper::get_slice_position(*image, orientation);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(double(75), index.value());

            med_im_helper::set_slice_position(*image, orientation, std::double_t(17.0));

            index = med_im_helper::get_slice_position(*image, orientation);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(double(17), index.value());
        }

        // No slice index
        {
            const auto image_no_slices = std::make_shared<sight::data::image>();
            auto axis                  = med_im_helper::axis_t::axial;
            const auto index           = med_im_helper::get_slice_position(*image_no_slices, axis);

            CHECK_EQ(false, index.has_value());
        }
    }

//------------------------------------------------------------------------------
    TEST_CASE("test_slice_index")
    {
        const auto image = generate_image();

        // AXIAL
        {
            auto axis = med_im_helper::axis_t::axial;

            auto index = med_im_helper::get_slice_index(*image, axis);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(std::int64_t(50), index.value());

            med_im_helper::set_slice_index(*image, axis, std::int64_t(35));

            index = med_im_helper::get_slice_index(*image, axis);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(std::int64_t(35), index.value());
        }

        // SAGITTAL
        {
            auto axis = med_im_helper::axis_t::sagittal;

            auto index = med_im_helper::get_slice_index(*image, axis);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(std::int64_t(128), index.value());

            med_im_helper::set_slice_index(*image, axis, std::int64_t(0));

            index = med_im_helper::get_slice_index(*image, axis);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(std::int64_t(0), index.value());
        }

        // FRONTAL
        {
            auto axis = med_im_helper::axis_t::frontal;

            auto index = med_im_helper::get_slice_index(*image, axis);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(std::int64_t(75), index.value());

            med_im_helper::set_slice_index(*image, axis, std::int64_t(17));

            index = med_im_helper::get_slice_index(*image, axis);

            CHECK_EQ(true, index.has_value());

            CHECK_EQ(std::int64_t(17), index.value());
        }

        // No slice index
        {
            const auto image_no_slices = std::make_shared<sight::data::image>();
            auto axis                  = med_im_helper::axis_t::axial;
            const auto index           = med_im_helper::get_slice_index(*image_no_slices, axis);

            CHECK_EQ(false, index.has_value());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("test_distances")
    {
        const auto image = generate_image();

        const auto distances_null = med_im_helper::get_distances(*image);

        CHECK(!distances_null);

        sight::data::vector::sptr distances      = std::make_shared<sight::data::vector>();
        sight::data::point_list::sptr point_list = std::make_shared<sight::data::point_list>();
        point_list->get_points().push_back(std::make_shared<sight::data::point>(0., 1., 2.));
        point_list->get_points().push_back(std::make_shared<sight::data::point>(10., 11., 12.));

        distances->push_back(point_list);

        med_im_helper::set_distances(*image, distances);

        const auto distances_not_null = med_im_helper::get_distances(*image);

        CHECK(distances_not_null);

        // TODO: compare values ?
    }

//------------------------------------------------------------------------------

    TEST_CASE("test_distance_visibility")
    {
        const auto image = generate_image();

        auto distance_visibility = med_im_helper::get_distance_visibility(*image);

        CHECK_EQ(true, distance_visibility);

        med_im_helper::set_distance_visibility(*image, false);

        distance_visibility = med_im_helper::get_distance_visibility(*image);

        CHECK_EQ(false, distance_visibility);
    }

//------------------------------------------------------------------------------

    TEST_CASE("test_landmarks_visibility")
    {
        const auto image = generate_image();

        auto lm_visibility = med_im_helper::get_landmarks_visibility(*image);

        CHECK_EQ(true, lm_visibility);

        med_im_helper::set_landmarks_visibility(*image, false);

        lm_visibility = med_im_helper::get_landmarks_visibility(*image);

        CHECK_EQ(false, lm_visibility);
    }

//------------------------------------------------------------------------------

    TEST_CASE("compute_histogram")
    {
        using image_t = std::int16_t;
        const std::size_t size_x     = 50;
        const std::size_t size_y     = 50;
        const std::size_t size_z     = 50;
        const std::size_t image_size = size_x * size_y * size_z;

        // Configure data hierarchy
        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        // Create image.
        image->resize({size_x, size_y, size_z}, sight::core::type::INT16, sight::data::image::gray_scale);

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

        sight::data::helper::histogram histogram(image);
        histogram.compute();
        auto values = histogram.sample(1);

        CHECK_EQ((std::size_t) (3000 - (-1000) + 1), values.size());
        CHECK_EQ(-1000., histogram.min());
        CHECK_EQ(3000., histogram.max());

        CHECK_EQ(1. / 4., doctest::Approx(values[0]).epsilon(0.000001));
        CHECK_EQ(1. / 4., doctest::Approx(values[1001]).epsilon(0.000001));
        CHECK_EQ(1. / 4., doctest::Approx(values[1500]).epsilon(0.000001));
        CHECK_EQ(1. / 4., doctest::Approx(values[4000]).epsilon(0.000001));

        values = histogram.sample(1001);

        CHECK_EQ((std::size_t) 4, values.size());
        CHECK_EQ(1. / 4., doctest::Approx(values[0]).epsilon(0.0001));
        CHECK_EQ(1. / 2., doctest::Approx(values[1]).epsilon(0.0001));
        CHECK_EQ(0., doctest::Approx(values[2]).epsilon(0.0001));
        CHECK_EQ(1. / 4., doctest::Approx(values[3]).epsilon(0.0001));

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

        CHECK_EQ((std::size_t) 3, values.size());
        CHECK_EQ(1. / 4., doctest::Approx(values[0]).epsilon(0.0001));
        CHECK_EQ(0., doctest::Approx(values[1]).epsilon(0.0001));
        CHECK_EQ(3. / 4., doctest::Approx(values[2]).epsilon(0.0001));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::tools::ut")
