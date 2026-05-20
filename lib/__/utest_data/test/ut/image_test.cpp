/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/type.hpp>

#include <data/image.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

inline static void stress_test_with_type(sight::core::type _type, int _nb_test)
{
    for(int nb = 0 ; nb < _nb_test ; ++nb)
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, _type);

        sight::data::image::sptr image2;
        image2 = sight::data::object::copy(image);
        CHECK(*image == *image2);
    }
}

TEST_SUITE("sight::utest_data::image")
{
    TEST_CASE("generator_3d")
    {
        sight::data::image::sptr image                      = std::make_shared<sight::data::image>();
        const sight::data::image::size_t size               = {10, 15, 23};
        const sight::data::image::spacing_t spacing         = {0.85, 2.6, 1.87};
        const sight::data::image::origin_t origin           = {-45.6, 25.97, -53.9};
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        sight::core::type type                              = sight::core::type::INT32;

        sight::utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::pixel_format_t::gray_scale
        );

        CHECK(image->size() == size);
        CHECK(image->spacing() == spacing);
        CHECK(image->origin() == origin);
        CHECK_EQ(type, image->type());
    }

    TEST_CASE("generator_2d")
    {
        sight::data::image::sptr image                      = std::make_shared<sight::data::image>();
        const sight::data::image::size_t size               = {10, 15, 0};
        const sight::data::image::spacing_t spacing         = {0.85, 2.6, 0.};
        const sight::data::image::origin_t origin           = {-45.6, 25.97, 0.};
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        sight::core::type type                              = sight::core::type::INT16;

        sight::utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::pixel_format_t::gray_scale
        );
        CHECK(image->size() == size);
        CHECK(image->spacing() == spacing);
        CHECK(image->origin() == origin);
        CHECK_EQ(type, image->type());
    }

    TEST_CASE("deep_copy")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::data::image::sptr image2;
        sight::core::type type = sight::core::type::INT32;
        sight::utest_data::generator::image::generate_random_image(image, type);

        image2 = sight::data::object::copy(image);
        CHECK(*image == *image2);
    }

    TEST_CASE("stress")
    {
        sight::core::type type = sight::core::type::INT8;
        stress_test_with_type(type, 10);

        type = sight::core::type::UINT8;
        stress_test_with_type(type, 10);

        type = sight::core::type::INT16;
        stress_test_with_type(type, 10);

        type = sight::core::type::UINT16;
        stress_test_with_type(type, 10);

        type = sight::core::type::INT32;
        stress_test_with_type(type, 10);

        type = sight::core::type::UINT32;
        stress_test_with_type(type, 10);

        type = sight::core::type::INT64;
        stress_test_with_type(type, 10);

        type = sight::core::type::UINT64;
        stress_test_with_type(type, 10);

        type = sight::core::type::FLOAT;
        stress_test_with_type(type, 10);

        type = sight::core::type::DOUBLE;
        stress_test_with_type(type, 10);
    }
}
