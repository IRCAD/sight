/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <filter/image/cast.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

template<typename INPUT, typename OUTPUT>
static void test_cast_conversion(
    const sight::data::image::size_t& _size,
    const sight::data::image::spacing_t& _spacing,
    const sight::data::image::origin_t& _origin,
    const sight::data::image::orientation_t& _orientation,
    sight::core::type _input_type,
    sight::core::type _output_type,
    sight::data::image::pixel_format_t _format,
    int _seed
)
{
    auto image_in = std::make_shared<sight::data::image>();

    sight::utest_data::generator::image::generate_image(
        image_in,
        _size,
        _spacing,
        _origin,
        _orientation,
        _input_type,
        _format,
        _seed
    );

    const auto in_dump_lock = image_in->dump_lock();

    auto image_out = sight::filter::image::cast_to(*image_in, _output_type);

    const auto out_dump_lock = image_out->dump_lock();

    // Check type conversion
    CHECK_EQ(image_out->type(), _output_type);

    // Check geometric parameters are preserved
    CHECK_EQ(image_out->size(), image_in->size());
    CHECK_EQ(image_out->spacing(), image_in->spacing());
    CHECK_EQ(image_out->origin(), image_in->origin());
    CHECK_EQ(image_out->orientation(), image_in->orientation());

    // Check pixel values are cast correctly
    for(std::size_t i = 0 ; i < _size[0] ; ++i)
    {
        for(std::size_t j = 0 ; j < _size[1] ; ++j)
        {
            for(std::size_t k = 0 ; k < _size[2] ; ++k)
            {
                const INPUT value_in   = image_in->at<INPUT>(i, j, k);
                const OUTPUT value_out = image_out->at<OUTPUT>(i, j, k);

                CHECK_EQ(static_cast<OUTPUT>(value_in), value_out);
            }
        }
    }
}

TEST_SUITE("sight::filter::image::cast")
{
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

    TEST_CASE("cast_uint8_to_uint16")
    {
        test_cast_conversion<std::uint8_t, std::uint16_t>(
            {10, 10, 10},
            {0.5, 0.5, 0.5},
            {1.0, 2.0, 3.0},
            {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6},
            sight::core::type::UINT8,
            sight::core::type::UINT16,
            sight::data::image::gray_scale,
            42
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_uint8_to_int16")
    {
        test_cast_conversion<std::uint8_t, std::int16_t>(
            {5, 5, 5},
            {1.0, 1.0, 1.0},
            {0.0, 0.0, 0.0},
            {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
            sight::core::type::UINT8,
            sight::core::type::INT16,
            sight::data::image::gray_scale,
            100
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_uint16_to_float")
    {
        test_cast_conversion<std::uint16_t, float>(
            {8, 8, 8},
            {0.1, 0.2, 0.3},
            {-5.0, 10.0, 0.0},
            {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6},
            sight::core::type::UINT16,
            sight::core::type::FLOAT,
            sight::data::image::gray_scale,
            512
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_double_to_uint8")
    {
        test_cast_conversion<double, std::uint8_t>(
            {6, 6, 6},
            {2.0, 2.0, 2.0},
            {0.0, 0.0, 0.0},
            {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6},
            sight::core::type::DOUBLE,
            sight::core::type::UINT8,
            sight::data::image::gray_scale,
            128
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_int32_to_float")
    {
        test_cast_conversion<std::int32_t, float>(
            {4, 4, 4},
            {1.0, 1.0, 1.0},
            {0.0, 0.0, 0.0},
            {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
            sight::core::type::INT32,
            sight::core::type::FLOAT,
            sight::data::image::gray_scale,
            200
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_preserves_empty_image")
    {
        const sight::data::image::size_t size                = {0, 0, 0};
        const sight::data::image::spacing_t spacing          = {0.0, 0.0, 0.0};
        const sight::data::image::origin_t origin            = {0.0, 0.0, 0.0};
        const sight::data::image::orientation_t orientation  = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
        const sight::core::type type                         = sight::core::type::UINT8;
        const enum sight::data::image::pixel_format_t format = sight::data::image::gray_scale;

        auto image_in = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(
            image_in,
            size,
            spacing,
            origin,
            orientation,
            type,
            format,
            0
        );

        const auto in_dump_lock = image_in->dump_lock();

        auto image_out = sight::filter::image::cast_to(*image_in, sight::core::type::FLOAT);

        // Check type conversion
        CHECK_EQ(image_out->type(), image_in->type());

        // Check that empty image remains empty
        CHECK_EQ(image_out->size()[0], 0);
        CHECK_EQ(image_out->size()[1], 0);
        CHECK_EQ(image_out->size()[2], 0);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::cast")
