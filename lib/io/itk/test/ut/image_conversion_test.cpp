/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "helper.hpp"

#include <core/base.hpp>
#include <core/tools/random/generator.hpp>

#include <data/object.hpp>

#include <io/itk/itk.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

using sight::core::tools::random::safe_rand;

//-----------------------------------------------------------------------------

template<class TYPE>
static void stress_test_for_a_type()
{
    for(unsigned char k = 0 ; k < 5 ; k++)
    {
        auto image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::get<TYPE>());

        constexpr sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        image->set_orientation(orientation);

        using image_t = ::itk::Image<TYPE, 3>;
        typename image_t::Pointer itk_image = sight::io::itk::move_to_itk<image_t>(image);

        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        sight::io::itk::move_from_itk<image_t>(itk_image, *image2, false);

        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);

        sight::data::image::sptr image3 = sight::io::itk::move_from_itk<image_t>(itk_image, false);
        image3->set_window_center(image->window_center());
        image3->set_window_width(image->window_width());

        CHECK(*image == *image3);
    }
}

TEST_SUITE("sight::io::itk::image_conversion")
{
    TEST_CASE("conversion")
    {
        // create Image
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::INT16);
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        image->set_orientation(orientation);

        using image_t = ::itk::Image<std::int16_t, 3>;
        image_t::Pointer itk_image      = sight::io::itk::move_to_itk<image_t>(image);
        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        sight::io::itk::move_from_itk<image_t>(itk_image, *image2, false);

        sight::io::itk::ut::helper::round_spacing(image);
        sight::io::itk::ut::helper::round_spacing(image2);

        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);

        sight::data::image::sptr image3 = sight::io::itk::move_from_itk<image_t>(itk_image, false);
        image3->set_window_center(image->window_center());
        image3->set_window_width(image->window_width());

        CHECK(*image == *image3);
    }

    TEST_CASE("stress")
    {
        stress_test_for_a_type<std::int8_t>();
        stress_test_for_a_type<std::uint8_t>();

        stress_test_for_a_type<std::int16_t>();
        stress_test_for_a_type<std::uint16_t>();

        stress_test_for_a_type<std::int32_t>();
        stress_test_for_a_type<std::uint32_t>();

        stress_test_for_a_type<std::int64_t>();
        stress_test_for_a_type<std::uint64_t>();

        stress_test_for_a_type<float>();
        stress_test_for_a_type<double>();
    }

    TEST_CASE("conversion_2d")
    {
        // create Image
        sight::data::image::sptr image  = std::make_shared<sight::data::image>();
        sight::data::image::size_t size = {
            static_cast<std::size_t>(safe_rand() % 100 + 2), static_cast<std::size_t>(safe_rand() % 100 + 2), 0
        };
        sight::data::image::spacing_t spacing = {(safe_rand() % 200 + 1) / 100., (safe_rand() % 200 + 1) / 100., 0.
        };
        sight::data::image::origin_t origin = {(safe_rand() % 200 - 100) / 3., (safe_rand() % 200 - 100) / 3., 0.
        };
        sight::data::image::orientation_t orientation = {0.36F, 0.48F, 0.0F, -0.8F, 0.6F, 0.0F, 0.0F, 0.0F, 1.0F};
        sight::core::type type                        = sight::core::type::INT16;

        sight::utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale,
            0
        );

        using image_t = ::itk::Image<std::int16_t, 2>;

        image_t::Pointer itk_image = sight::io::itk::move_to_itk<image_t>(image);

        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        bool image2_manages_his_buffer  = false;
        sight::io::itk::move_from_itk<image_t>(itk_image, *image2, image2_manages_his_buffer);
        sight::io::itk::ut::helper::round_spacing(image);
        sight::io::itk::ut::helper::round_spacing(image2);

        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);

        sight::data::image::sptr image3 = sight::io::itk::move_from_itk<image_t>(itk_image, false);
        image3->set_window_center(image->window_center());
        image3->set_window_width(image->window_width());

        CHECK(*image == *image3);
    }
} // TEST_SUITE
