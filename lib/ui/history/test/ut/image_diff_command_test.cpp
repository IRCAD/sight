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

#include <ui/history/image_diff_command.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <algorithm>

TEST_SUITE("sight::ui::history")
{
//------------------------------------------------------------------------------

    TEST_CASE("undoredo")
    {
        const sight::data::image::size_t size                = {32, 32, 32};
        const sight::data::image::spacing_t spacing          = {1., 1., 1.};
        const sight::data::image::origin_t origin            = {0., 0., 0.};
        const sight::data::image::orientation_t orientation  = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const sight::core::type type                         = sight::core::type::UINT8;
        const enum sight::data::image::pixel_format_t format = sight::data::image::gray_scale;

        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(image, size, spacing, origin, orientation, type, format);

        const auto dump_lock = image->dump_lock();

        sight::filter::image::image_diff diff(image->type().size());

        std::uint8_t newvalue = 1;

        auto* new_buffer_value = reinterpret_cast<sight::data::image::buffer_t*>(&newvalue);

        const std::vector<sight::data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

        // Add 8 elements to the diff. Write new values to the image.
        for(std::size_t i = 0 ; i < 8 ; ++i)
        {
            const sight::data::image::index_t index = indices[i];

            const sight::data::image::buffer_t* pix_buf =
                reinterpret_cast<sight::data::image::buffer_t*>(image->get_pixel(index));

            diff.add_diff(index, pix_buf, new_buffer_value);
            image->set_pixel(index, new_buffer_value);

            CHECK_EQ(i + 1, diff.num_elements());
            CHECK_EQ(index, diff.get_element_diff_index(i));
        }

        // Create an imageDiffCommand to test
        sight::ui::history::image_diff_command image_diff_command(image, diff);

        // Revert diff. Ensure that the image is the same as before (all values equal to zero).
        CHECK(image_diff_command.undo());

        for(std::size_t it = 0 ; it < image->size_in_bytes() ; ++it)
        {
            CHECK_EQ(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->get_pixel(it)));
        }

        // Apply diff. Ensure all values are zero except the ones at the selected indices.
        CHECK(image_diff_command.redo());

        for(std::size_t i = 0 ; i < image->size_in_bytes() ; ++i)
        {
            // Check if 'i' is an index
            auto index_it = std::ranges::find(indices, i);

            if(index_it != indices.end())
            {
                CHECK_EQ(newvalue, *reinterpret_cast<std::uint8_t*>(image->get_pixel(i)));
            }
            else
            {
                CHECK_EQ(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->get_pixel(i)));
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("get_size")
    {
        const sight::data::image::size_t size                = {32, 32, 32};
        const sight::data::image::spacing_t spacing          = {1., 1., 1.};
        const sight::data::image::origin_t origin            = {0., 0., 0.};
        const sight::data::image::orientation_t orientation  = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const sight::core::type type                         = sight::core::type::UINT8;
        const enum sight::data::image::pixel_format_t format = sight::data::image::gray_scale;

        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(image, size, spacing, origin, orientation, type, format);

        const auto dump_lock = image->dump_lock();

        sight::filter::image::image_diff diff(image->type().size() * 64);

        std::uint8_t newvalue = 1;

        auto* new_buffer_value = reinterpret_cast<sight::data::image::buffer_t*>(&newvalue);

        const std::vector<sight::data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

        // Add 8 elements to the diff. Write new values to the image.
        for(std::size_t i = 0 ; i < 8 ; ++i)
        {
            const sight::data::image::index_t index = indices[i];

            const sight::data::image::buffer_t* pix_buf =
                reinterpret_cast<sight::data::image::buffer_t*>(image->get_pixel(index));

            diff.add_diff(index, pix_buf, new_buffer_value);
            image->set_pixel(index, new_buffer_value);

            CHECK_EQ(std::size_t(i + 1), diff.num_elements());
            CHECK_EQ(index, diff.get_element_diff_index(i));
        }

        // Create an imageDiffCommand to test
        sight::ui::history::image_diff_command image_diff_command(image, diff);

        // Ensure that the real size is at least bigger than the naive sizeof
        CHECK(image_diff_command.size() > sizeof(image_diff_command));

        // Ensure that the real size is at least bigger than the size of the diff
        CHECK(image_diff_command.size() > diff.size());
    }
} // TEST_SUITE
