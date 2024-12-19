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

#include "image_diff_test.hpp"

#include <core/type.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <filter/image/image.hpp>
#include <filter/image/image_diff.hpp>

#include <utest_data/generator/image.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::image_diff_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

template<typename T>
static void test_diff_equality(const image_diff& _diff1, const image_diff& _diff2)
{
    CPPUNIT_ASSERT_EQUAL(_diff1.num_elements(), _diff2.num_elements());

    for(std::size_t i = 0 ; i < _diff1.num_elements() ; ++i)
    {
        image_diff::element_t diff1_elt = _diff1.get_element(i);
        image_diff::element_t diff2_elt = _diff2.get_element(i);

        CPPUNIT_ASSERT_EQUAL(diff1_elt.m_index, diff2_elt.m_index);
        CPPUNIT_ASSERT_EQUAL(
            *reinterpret_cast<const T*>(diff1_elt.m_old_value),
            *reinterpret_cast<const T*>(diff2_elt.m_old_value)
        );
        CPPUNIT_ASSERT_EQUAL(
            *reinterpret_cast<const T*>(diff1_elt.m_new_value),
            *reinterpret_cast<const T*>(diff2_elt.m_new_value)
        );
    }
}

//------------------------------------------------------------------------------

void image_diff_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_diff_test::tearDown()
{
}

//------------------------------------------------------------------------------

void image_diff_test::store_diffs_test()
{
    const std::uint8_t oldvalue = 0;
    const std::uint8_t newvalue = 1;

    image_diff diff(sizeof(oldvalue));

    const auto* old_buffer_value = reinterpret_cast<const data::image::buffer_t*>(&oldvalue);
    const auto* new_buffer_value = reinterpret_cast<const data::image::buffer_t*>(&newvalue);

    const std::vector<data::image::index_t> indices = {{51, 10, 8, 123, 1098, 23456, 6, 9999}};

    // Add 8 elements to the diff.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        const data::image::index_t index = indices[i];

        diff.add_diff(index, old_buffer_value, new_buffer_value);

        CPPUNIT_ASSERT_EQUAL(i + 1, diff.num_elements());
        CPPUNIT_ASSERT_EQUAL(index, diff.get_element_diff_index(i));
    }

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(std::size_t(8), diff.num_elements());

    // Ensure the elements where inserted.
    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        image_diff::element_t elt = diff.get_element(i);

        CPPUNIT_ASSERT_EQUAL(indices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(oldvalue, *reinterpret_cast<const std::uint8_t*>(elt.m_old_value));
        CPPUNIT_ASSERT_EQUAL(newvalue, *reinterpret_cast<const std::uint8_t*>(elt.m_new_value));
    }

    // Create a second diff with 3 elements.
    const std::vector<data::image::index_t> indices2 = {{66, 42, 8888}};
    image_diff diff2(sizeof(oldvalue));

    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        const data::image::index_t index = indices2[i];

        diff2.add_diff(index, old_buffer_value, new_buffer_value);

        CPPUNIT_ASSERT_EQUAL(i + 1, diff2.num_elements());
        CPPUNIT_ASSERT_EQUAL(index, diff2.get_element_diff_index(i));
    }

    // Merge both diffs. Ensure the total size is 11.
    diff.add_diff(diff2);
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), diff.num_elements());

    // Shrink and test that we didn't lose any values.
    diff.shrink();
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), diff.num_elements());

    std::vector<data::image::index_t> merged_indices;
    merged_indices.insert(merged_indices.end(), indices.begin(), indices.end());
    merged_indices.insert(merged_indices.end(), indices2.begin(), indices2.end());

    // Ensure the elements where merged.
    for(std::size_t i = 0 ; i < 11 ; ++i)
    {
        image_diff::element_t elt = diff.get_element(i);

        CPPUNIT_ASSERT_EQUAL(merged_indices[i], elt.m_index);
        CPPUNIT_ASSERT_EQUAL(oldvalue, *reinterpret_cast<const std::uint8_t*>(elt.m_old_value));
        CPPUNIT_ASSERT_EQUAL(newvalue, *reinterpret_cast<const std::uint8_t*>(elt.m_new_value));
    }

    // Copy constructor test.
    image_diff copy_diff(diff);
    test_diff_equality<std::uint8_t>(diff, copy_diff);

    // Copy assignement test.
    image_diff copy_diff2 = diff;
    test_diff_equality<std::uint8_t>(diff, copy_diff2);

    // Move constructor test.
    const image_diff move_diff(std::move(copy_diff));
    test_diff_equality<std::uint8_t>(diff, move_diff);

    // Move assignement test.
    image_diff move_diff2 = std::move(copy_diff2);
    test_diff_equality<std::uint8_t>(diff, move_diff2);

    // Clear the diff, test if it's really empty.
    diff.clear();

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), diff.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), diff.num_elements());
}

//------------------------------------------------------------------------------

void image_diff_test::undo_redo_test()
{
    const data::image::size_t size               = {32, 32, 32};
    const data::image::spacing_t spacing         = {1., 1., 1.};
    const data::image::origin_t origin           = {0., 0., 0.};
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    const core::type type                        = core::type::UINT8;
    const data::image::pixel_format_t format     = data::image::pixel_format_t::gray_scale;

    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(image, size, spacing, origin, orientation, type, format);

    const auto dump_lock = image->dump_lock();

    image_diff diff(image->type().size());

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
        CPPUNIT_ASSERT_EQUAL(newvalue, *reinterpret_cast<std::uint8_t*>(image->get_pixel(index)));
    }

    // Revert diff. Ensure that the image is the same as before (all values equal to zero).
    diff.revert_diff(image);

    for(std::size_t it = 0 ; it < image->size_in_bytes() ; ++it)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "it: " + std::to_string(it),
            (int) std::uint8_t(0),
            (int) *reinterpret_cast<std::uint8_t*>(image->get_pixel(it))
        );
    }

    // Apply diff. Ensure all values are zero except the ones at the selected indices.
    diff.apply_diff(image);

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

} // namespace sight::filter::image::ut
