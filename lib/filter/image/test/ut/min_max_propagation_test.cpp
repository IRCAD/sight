/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <filter/image/min_max_propagation.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::filter::image::min_max_propagation")
{
//------------------------------------------------------------------------------

    static std::size_t compute_offset(
        const std::size_t _x,
        const std::size_t _y,
        const std::size_t _z,
        sight::data::image::sptr _image
)
    {
        const sight::data::image::size_t size = _image->size();
        return _z * size[0] * size[1] + _y * size[0] + _x;
    }

//------------------------------------------------------------------------------

    static void draw_cube(sight::data::image::sptr _image, const std::uint8_t _value)
    {
        const auto dump_lock = _image->dump_lock();

        const auto* buffer_value = reinterpret_cast<const uint8_t*>(&_value);

        for(std::size_t x = 10 ; x < 20 ; ++x)
        {
            for(std::size_t y = 10 ; y < 20 ; ++y)
            {
                for(std::size_t z = 10 ; z < 20 ; ++z)
                {
                    _image->set_pixel(compute_offset(x, y, z, _image), buffer_value);
                }
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("min_propag")
    {
        // Create two 32*32*32*8 images
        const sight::data::image::size_t size               = {32, 32, 32};
        const sight::data::image::spacing_t spacing         = {1., 1., 1.};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const sight::core::type type                        = sight::core::type::UINT8;

        auto image_in  = std::make_shared<sight::data::image>();
        auto image_out = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(
            image_in,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        sight::utest_data::generator::image::generate_image(
            image_out,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        // Draw a cube at 10,10,10 with a 255 value
        draw_cube(image_in, 255);

        sight::filter::image::min_max_propagation::seeds_t seed = {{{15, 15, 15}}};

        // Propagate at 15,15,15 with a 255 value (same as cube)
        std::uint8_t value = 255;

        const auto dump_lock_in  = image_in->dump_lock();
        const auto dump_lock_out = image_out->dump_lock();

        sight::filter::image::min_max_propagation::process(
            image_in,
            image_out,
            nullptr,
            seed,
            value,
            500,
            true,
            sight::filter::image::min_max_propagation::min
        );

        // Check that the image is not changed because the propagated value is the same
        for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
        {
            const std::uint8_t value_in  = image_in->at<std::uint8_t>(index);
            const std::uint8_t value_out = image_out->at<std::uint8_t>(index);

            CHECK_EQ(value_in, value_out);
        }

        // Check that the cube is changed with a "3" propagated value
        draw_cube(image_in, 1);
        value = 3;

        sight::filter::image::min_max_propagation::process(
            image_in,
            image_out,
            nullptr,
            seed,
            value,
            500,
            true,
            sight::filter::image::min_max_propagation::min
        );

        for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
        {
            const std::uint8_t value_in  = image_in->at<std::uint8_t>(index);
            const std::uint8_t value_out = image_out->at<std::uint8_t>(index);

            if(value_in == 1)
            {
                CHECK_EQ(static_cast<std::uint8_t>(3), value_out);
            }
            else
            {
                CHECK_EQ(value_in, value_out);
            }
        }

        // Check that the entire image is completely filled with propagated value
        seed  = {{{0, 0, 0}}};
        value = 4;

        sight::filter::image::min_max_propagation::process(
            image_in,
            image_out,
            nullptr,
            seed,
            value,
            500,
            true,
            sight::filter::image::min_max_propagation::min
        );

        for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
        {
            const std::uint8_t value_out = image_out->at<std::uint8_t>(index);
            CHECK_EQ(static_cast<std::uint8_t>(4), value_out);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("max_propag")
    {
        // Create two 32*32*32*8 images
        const sight::data::image::size_t size               = {32, 32, 32};
        const sight::data::image::spacing_t spacing         = {1., 1., 1.};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const sight::core::type type                        = sight::core::type::UINT8;

        auto image_in  = std::make_shared<sight::data::image>();
        auto image_out = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(
            image_in,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        sight::utest_data::generator::image::generate_image(
            image_out,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        // Draw a cube at 10,10,10 with a 2 value
        draw_cube(image_in, 2);

        sight::filter::image::min_max_propagation::seeds_t seed = {{{15, 15, 15}}};

        // Propagate at 15,15,15 with a 3 value
        std::uint8_t value = 3;

        const auto dump_lock_in  = image_in->dump_lock();
        const auto dump_lock_out = image_out->dump_lock();

        sight::filter::image::min_max_propagation::process(
            image_in,
            image_out,
            nullptr,
            seed,
            value,
            500,
            true,
            sight::filter::image::min_max_propagation::max
        );

        // Check that the entire image is completely filled with propagated value
        for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
        {
            const std::uint8_t value_out = image_out->at<std::uint8_t>(index);
            CHECK_EQ(static_cast<std::uint8_t>(3), value_out);
        }

        // Propagate at 0,0,0 with a 2 value (outside cube)
        seed  = {{{0, 0, 0}}};
        value = 2;

        sight::filter::image::min_max_propagation::process(
            image_in,
            image_out,
            nullptr,
            seed,
            value,
            500,
            true,
            sight::filter::image::min_max_propagation::max
        );

        // Check that the entire image is completely filled with propagated value
        for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
        {
            const std::uint8_t value_in  = image_in->at<std::uint8_t>(index);
            const std::uint8_t value_out = image_out->at<std::uint8_t>(index);

            if(value_in == 0)
            {
                CHECK_EQ(static_cast<std::uint8_t>(2), value_out);
            }
            else
            {
                // Old value previously propagated
                CHECK_EQ(static_cast<std::uint8_t>(3), value_out);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("radius")
    {
        // Create two 32*32*32*8 images
        const sight::data::image::size_t size               = {33, 33, 33};
        const sight::data::image::spacing_t spacing         = {1., 1., 1.};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const sight::core::type type                        = sight::core::type::UINT8;

        auto image_in  = std::make_shared<sight::data::image>();
        auto image_out = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(
            image_in,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        sight::utest_data::generator::image::generate_image(
            image_out,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale
        );

        sight::filter::image::min_max_propagation::seeds_t seed = {{{16, 16, 16}}};

        // Propagate at 16,16,16 with a 3 value
        std::uint8_t value = 3;

        const auto dump_lock_in  = image_in->dump_lock();
        const auto dump_lock_out = image_out->dump_lock();

        sight::filter::image::min_max_propagation::process(
            image_in,
            image_out,
            nullptr,
            seed,
            value,
            3.5,
            true,
            sight::filter::image::min_max_propagation::min
        );

        // Check the voxel at 16,16,12
        std::uint8_t value_out = image_out->at<std::uint8_t>(16, 16, 12);
        CHECK_EQ(static_cast<std::uint8_t>(0), value_out);

        // Check the voxel at 16,16,13
        value_out = image_out->at<std::uint8_t>(16, 16, 13);
        CHECK_EQ(static_cast<std::uint8_t>(3), value_out);

        // Check the voxel at 16,16,16
        value_out = image_out->at<std::uint8_t>(16, 16, 16);
        CHECK_EQ(static_cast<std::uint8_t>(3), value_out);

        // Check the voxel at 16,16,20
        value_out = image_out->at<std::uint8_t>(16, 16, 20);
        CHECK_EQ(static_cast<std::uint8_t>(0), value_out);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::min_max_propagation")
