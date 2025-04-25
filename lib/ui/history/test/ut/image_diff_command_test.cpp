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

#include "image_diff_command_test.hpp"

#include <ui/history/image_diff_command.hpp>

#include <utest_data//generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::history::ut::image_diff_command_test);

namespace sight::ui::history::ut
{

//------------------------------------------------------------------------------

void image_diff_command_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_diff_command_test::tearDown()
{
}

//------------------------------------------------------------------------------

void image_diff_command_test::undoredo_test()
{
    const data::image::size_t size                = {32, 32, 32};
    const data::image::spacing_t spacing          = {1., 1., 1.};
    const data::image::origin_t origin            = {0., 0., 0.};
    const data::image::orientation_t orientation  = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    const core::type type                         = core::type::UINT8;
    const enum data::image::pixel_format_t format = data::image::gray_scale;

    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image, size, spacing, origin, orientation, type, format);

    const auto dump_lock = image->dump_lock();

    filter::image::image_diff diff(image->type().size());

    std::uint8_t newvalue = 1;

    auto* new_buffer_value = reinterpret_cast<data::image::buffer_t*>(&newvalue);

    const std::vector<data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff. Write new values to the image.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        const data::image::index_t index = indices[i];

        const data::image::buffer_t* pix_buf =
            reinterpret_cast<data::image::buffer_t*>(image->get_pixel(index));

        diff.add_diff(index, pix_buf, new_buffer_value);
        image->set_pixel(index, new_buffer_value);

        CPPUNIT_ASSERT_EQUAL(i + 1, diff.num_elements());
        CPPUNIT_ASSERT_EQUAL(index, diff.get_element_diff_index(i));
    }

    // Create an imageDiffCommand to test
    ui::history::image_diff_command image_diff_command(image, diff);

    // Revert diff. Ensure that the image is the same as before (all values equal to zero).
    CPPUNIT_ASSERT(image_diff_command.undo());

    for(std::size_t it = 0 ; it < image->size_in_bytes() ; ++it)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->get_pixel(it)));
    }

    // Apply diff. Ensure all values are zero except the ones at the selected indices.
    CPPUNIT_ASSERT(image_diff_command.redo());

    for(std::size_t i = 0 ; i < image->size_in_bytes() ; ++i)
    {
        // Check if 'i' is an index
        auto index_it = std::find(indices.begin(), indices.end(), i);

        if(index_it != indices.end())
        {
            CPPUNIT_ASSERT_EQUAL(newvalue, *reinterpret_cast<std::uint8_t*>(image->get_pixel(i)));
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->get_pixel(i)));
        }
    }
}

//------------------------------------------------------------------------------

void image_diff_command_test::get_size_test()
{
    const data::image::size_t size                = {32, 32, 32};
    const data::image::spacing_t spacing          = {1., 1., 1.};
    const data::image::origin_t origin            = {0., 0., 0.};
    const data::image::orientation_t orientation  = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    const core::type type                         = core::type::UINT8;
    const enum data::image::pixel_format_t format = data::image::gray_scale;

    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image, size, spacing, origin, orientation, type, format);

    const auto dump_lock = image->dump_lock();

    filter::image::image_diff diff(image->type().size() * 64);

    std::uint8_t newvalue = 1;

    auto* new_buffer_value = reinterpret_cast<data::image::buffer_t*>(&newvalue);

    const std::vector<data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff. Write new values to the image.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        const data::image::index_t index = indices[i];

        const data::image::buffer_t* pix_buf =
            reinterpret_cast<data::image::buffer_t*>(image->get_pixel(index));

        diff.add_diff(index, pix_buf, new_buffer_value);
        image->set_pixel(index, new_buffer_value);

        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), diff.num_elements());
        CPPUNIT_ASSERT_EQUAL(index, diff.get_element_diff_index(i));
    }

    // Create an imageDiffCommand to test
    ui::history::image_diff_command image_diff_command(image, diff);

    // Ensure that the real size is at least bigger than the naive sizeof
    CPPUNIT_ASSERT(image_diff_command.size() > sizeof(image_diff_command));

    // Ensure that the real size is at least bigger than the size of the diff
    CPPUNIT_ASSERT(image_diff_command.size() > diff.size());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::history::ut
