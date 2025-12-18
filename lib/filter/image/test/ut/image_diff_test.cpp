/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
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

#include <core/type.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <filter/image/image.hpp>
#include <filter/image/image_diff.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

TEST_SUITE("sight::filter::image::image_diff")
{
//------------------------------------------------------------------------------

    template<typename T>
    static void test_diff_equality(
        const sight::filter::image::image_diff& _diff1,
        const sight::filter::image::image_diff& _diff2
)
    {
        CHECK_EQ(_diff1.num_elements(), _diff2.num_elements());

        for(std::size_t i = 0 ; i < _diff1.num_elements() ; ++i)
        {
            sight::filter::image::image_diff::element_t diff1_elt = _diff1.get_element(i);
            sight::filter::image::image_diff::element_t diff2_elt = _diff2.get_element(i);

            CHECK_EQ(diff1_elt.m_index, diff2_elt.m_index);
            CHECK_EQ(
                *reinterpret_cast<const T*>(diff1_elt.m_old_value),
                *reinterpret_cast<const T*>(diff2_elt.m_old_value)
            );
            CHECK_EQ(
                *reinterpret_cast<const T*>(diff1_elt.m_new_value),
                *reinterpret_cast<const T*>(diff2_elt.m_new_value)
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("store_diffs")
    {
        const std::uint8_t old_value = 0;
        const std::uint8_t new_value = 1;

        sight::filter::image::image_diff diff(sizeof(old_value));

        const auto* old_buffer_value = reinterpret_cast<const sight::data::image::buffer_t*>(&old_value);
        const auto* new_buffer_value = reinterpret_cast<const sight::data::image::buffer_t*>(&new_value);

        const std::vector<sight::data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

        // Add 8 elements to the diff.
        for(std::size_t i = 0 ; i < 8 ; ++i)
        {
            const sight::data::image::index_t index = indices[i];

            diff.add_diff(index, old_buffer_value, new_buffer_value);

            CHECK_EQ(i + 1, diff.num_elements());
            CHECK_EQ(index, diff.get_element_diff_index(i));
        }

        // Shrink and test that we didn't lose any values.
        diff.shrink();
        CHECK_EQ(std::size_t(8), diff.num_elements());

        // Ensure the elements where inserted.
        for(std::size_t i = 0 ; i < 8 ; ++i)
        {
            sight::filter::image::image_diff::element_t elt = diff.get_element(i);

            CHECK_EQ(indices[i], elt.m_index);
            CHECK_EQ(old_value, *reinterpret_cast<const std::uint8_t*>(elt.m_old_value));
            CHECK_EQ(new_value, *reinterpret_cast<const std::uint8_t*>(elt.m_new_value));
        }

        // Create a second diff with 3 elements.
        const std::vector<sight::data::image::index_t> indices2 = {{66, 42, 8888}};

        sight::filter::image::image_diff diff2(sizeof(old_value));

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            const sight::data::image::index_t index = indices2[i];

            diff2.add_diff(index, old_buffer_value, new_buffer_value);

            CHECK_EQ(i + 1, diff2.num_elements());
            CHECK_EQ(index, diff2.get_element_diff_index(i));
        }

        // Merge both diffs. Ensure the total size is 11.
        diff.add_diff(diff2);
        CHECK_EQ(std::size_t(11), diff.num_elements());

        // Shrink and test that we didn't lose any values.
        diff.shrink();
        CHECK_EQ(std::size_t(11), diff.num_elements());

        std::vector<sight::data::image::index_t> merged_indices;
        merged_indices.insert(merged_indices.end(), indices.begin(), indices.end());
        merged_indices.insert(merged_indices.end(), indices2.begin(), indices2.end());

        // Ensure the elements where merged.
        for(std::size_t i = 0 ; i < 11 ; ++i)
        {
            sight::filter::image::image_diff::element_t elt = diff.get_element(i);

            CHECK_EQ(merged_indices[i], elt.m_index);
            CHECK_EQ(old_value, *reinterpret_cast<const std::uint8_t*>(elt.m_old_value));
            CHECK_EQ(new_value, *reinterpret_cast<const std::uint8_t*>(elt.m_new_value));
        }

        // Copy constructor test.
        sight::filter::image::image_diff copy_diff(diff);
        test_diff_equality<std::uint8_t>(diff, copy_diff);

        // Copy assignment test.
        sight::filter::image::image_diff copy_diff2 = diff;
        test_diff_equality<std::uint8_t>(diff, copy_diff2);

        // Move constructor test.
        const sight::filter::image::image_diff move_diff(std::move(copy_diff));
        test_diff_equality<std::uint8_t>(diff, move_diff);

        // Move assignment test.
        sight::filter::image::image_diff move_diff2 = std::move(copy_diff2);
        test_diff_equality<std::uint8_t>(diff, move_diff2);

        // Clear the diff, test if it's really empty.
        diff.clear();
        CHECK_EQ(std::size_t(0), diff.size());
        CHECK_EQ(std::size_t(0), diff.num_elements());
    }

//------------------------------------------------------------------------------

    TEST_CASE("undo_redo")
    {
        const sight::data::image::size_t size               = {32, 32, 32};
        const sight::data::image::spacing_t spacing         = {1., 1., 1.};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const sight::core::type type                        = sight::core::type::UINT8;
        const sight::data::image::pixel_format_t format     = sight::data::image::pixel_format_t::gray_scale;

        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(image, size, spacing, origin, orientation, type, format);

        const auto dump_lock = image->dump_lock();

        sight::filter::image::image_diff diff(image->type().size());

        std::uint8_t new_value = 1;
        auto* new_buffer_value = reinterpret_cast<sight::data::image::buffer_t*>(&new_value);

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
            CHECK_EQ(new_value, *reinterpret_cast<std::uint8_t*>(image->get_pixel(index)));
        }

        // Revert diff. Ensure that the image is the same as before (all values equal to zero).
        diff.revert_diff(image);

        for(std::size_t it = 0 ; it < image->size_in_bytes() ; ++it)
        {
            CHECK_MESSAGE(
                (int) std::uint8_t(0) == (int) *reinterpret_cast<std::uint8_t*>(image->get_pixel(it)),
                "it: ",
                std::to_string(it)
            );
        }

        // Apply diff. Ensure all values are zero except the ones at the selected indices.
        diff.apply_diff(image);

        for(std::size_t i = 0 ; i < image->size_in_bytes() ; ++i)
        {
            // Check if 'i' is an index
            if(std::ranges::find(indices, i) != indices.end())
            {
                CHECK_EQ(new_value, *reinterpret_cast<std::uint8_t*>(image->get_pixel(i)));
            }
            else
            {
                CHECK_EQ(std::uint8_t(0), *reinterpret_cast<std::uint8_t*>(image->get_pixel(i)));
            }
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::sight::filter::image::image_diff")
