/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include "min_max_propagation_test.hpp"

#include <data/helper/medical_image.hpp>

#include <filter/image/min_max_propagation.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::min_max_propagation_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

static std::size_t compute_offset(
    const std::size_t _x,
    const std::size_t _y,
    const std::size_t _z,
    data::image::sptr _image
)
{
    const data::image::size_t size = _image->size();
    return _z * size[0] * size[1] + _y * size[0] + _x;
}

//------------------------------------------------------------------------------

static void draw_cube(data::image::sptr _image, const std::uint8_t _value)
{
    const auto dump_lock = _image->dump_lock();

    SPTR(data::image::buffer_t) buffer_value =
        data::helper::medical_image::get_pixel_in_image_space(_image, _value);

    for(std::size_t x = 10 ; x < 20 ; ++x)
    {
        for(std::size_t y = 10 ; y < 20 ; ++y)
        {
            for(std::size_t z = 10 ; z < 20 ; ++z)
            {
                _image->set_pixel(compute_offset(x, y, z, _image), buffer_value.get());
            }
        }
    }
}

//------------------------------------------------------------------------------

void min_max_propagation_test::setUp()
{
}

//------------------------------------------------------------------------------

void min_max_propagation_test::tearDown()
{
}

//------------------------------------------------------------------------------

void min_max_propagation_test::min_propag_test()
{
    // Create two 32*32*32*8 images
    const data::image::size_t size       = {{32, 32, 32}};
    const data::image::spacing_t spacing = {{1., 1., 1.}};
    const data::image::origin_t origin   = {{0., 0., 0.}};
    const core::type type                = core::type::UINT8;

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, data::image::gray_scale);
    utest_data::generator::image::generate_image(image_out, size, spacing, origin, type, data::image::gray_scale);

    // Draw a cube at 10,10,10 with a 255 value
    draw_cube(image_in, 255);

    min_max_propagation::seeds_t seed = {{{15, 15, 15}}};

    // Propagate at 15,15,15 with a 255 value (same as cube)
    std::uint8_t value = 255;

    SPTR(data::image::buffer_t) buffer_value = data::helper::medical_image::get_pixel_in_image_space(image_in, value);

    const auto dump_lock_in  = image_in->dump_lock();
    const auto dump_lock_out = image_out->dump_lock();

    min_max_propagation::process(
        image_in,
        image_out,
        nullptr,
        seed,
        value,
        500,
        true,
        min_max_propagation::min
    );

    // Check that the image is not changed because the propagated value is the same
    for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
    {
        const std::uint8_t value_in  = image_in->at<std::uint8_t>(index);
        const std::uint8_t value_out = image_out->at<std::uint8_t>(index);

        CPPUNIT_ASSERT_EQUAL(value_in, value_out);
    }

    // Check that the cube is changed with a "3" propagated value
    draw_cube(image_in, 1);

    value = 3;

    min_max_propagation::process(
        image_in,
        image_out,
        nullptr,
        seed,
        value,
        500,
        true,
        min_max_propagation::min
    );

    for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
    {
        const std::uint8_t value_in  = image_in->at<std::uint8_t>(index);
        const std::uint8_t value_out = image_out->at<std::uint8_t>(index);

        if(value_in == 1)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), value_out);
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(value_in, value_out);
        }
    }

    // Check that the entire image is completely filled with propagated value
    seed  = {{{0, 0, 0}}};
    value = 4;

    min_max_propagation::process(
        image_in,
        image_out,
        nullptr,
        seed,
        value,
        500,
        true,
        min_max_propagation::min
    );

    for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
    {
        const std::uint8_t value_out = image_out->at<std::uint8_t>(index);

        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4), value_out);
    }
}

//------------------------------------------------------------------------------

void min_max_propagation_test::max_propag_test()
{
    // Create two 32*32*32*8 images
    const data::image::size_t size       = {{32, 32, 32}};
    const data::image::spacing_t spacing = {{1., 1., 1.}};
    const data::image::origin_t origin   = {{0., 0., 0.}};
    const core::type type                = core::type::UINT8;

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, data::image::gray_scale);
    utest_data::generator::image::generate_image(image_out, size, spacing, origin, type, data::image::gray_scale);

    // Draw a cube at 10,10,10 with a 2 value
    draw_cube(image_in, 2);

    min_max_propagation::seeds_t seed = {{{15, 15, 15}}};

    // Propagate at 15,15,15 with a 3 value
    std::uint8_t value = 3;

    const auto dump_lock_in  = image_in->dump_lock();
    const auto dump_lock_out = image_out->dump_lock();

    min_max_propagation::process(
        image_in,
        image_out,
        nullptr,
        seed,
        value,
        500,
        true,
        min_max_propagation::max
    );

    // Check that the entire image is completely filled with propagated value
    for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
    {
        const std::uint8_t value_out = image_out->at<std::uint8_t>(index);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), value_out);
    }

    // Propagate at 0,0,0 with a 2 value (outside cube)
    seed  = {{{0, 0, 0}}};
    value = 2;

    min_max_propagation::process(
        image_in,
        image_out,
        nullptr,
        seed,
        value,
        500,
        true,
        min_max_propagation::max
    );

    // Check that the entire image is completely filled with propagated value
    for(std::size_t index = 0 ; index < image_in->size_in_bytes() ; ++index)
    {
        const std::uint8_t value_in  = image_in->at<std::uint8_t>(index);
        const std::uint8_t value_out = image_out->at<std::uint8_t>(index);

        if(value_in == 0)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(2), value_out);
        }
        else
        {
            // Old value previously propagated
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), value_out);
        }
    }
}

//------------------------------------------------------------------------------

void min_max_propagation_test::radius_test()
{
    // Create two 32*32*32*8 images
    const data::image::size_t size       = {{33, 33, 33}};
    const data::image::spacing_t spacing = {{1., 1., 1.}};
    const data::image::origin_t origin   = {{0., 0., 0.}};
    const core::type type                = core::type::UINT8;

    data::image::sptr image_in  = std::make_shared<data::image>();
    data::image::sptr image_out = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image_in, size, spacing, origin, type, data::image::gray_scale);
    utest_data::generator::image::generate_image(image_out, size, spacing, origin, type, data::image::gray_scale);

    min_max_propagation::seeds_t seed = {{{16, 16, 16}}};

    // Propagate at 16,16,16 with a 3 value
    std::uint8_t value = 3;

    const auto dump_lock_in  = image_in->dump_lock();
    const auto dump_lock_out = image_out->dump_lock();

    min_max_propagation::process(
        image_in,
        image_out,
        nullptr,
        seed,
        value,
        3.5,
        true,
        min_max_propagation::min
    );

    // Check the voxel at 16,16,12
    std::uint8_t value_out = image_out->at<std::uint8_t>(16, 16, 12);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);

    // Check the voxel at 16,16,13
    value_out = image_out->at<std::uint8_t>(16, 16, 13);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), value_out);

    // Check the voxel at 16,16,16
    value_out = image_out->at<std::uint8_t>(16, 16, 16);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(3), value_out);

    // Check the voxel at 16,16,20
    value_out = image_out->at<std::uint8_t>(16, 16, 20);

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
