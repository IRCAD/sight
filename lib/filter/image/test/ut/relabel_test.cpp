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

#include <data/image.hpp>

#include <filter/image/relabel.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

static std::shared_ptr<sight::data::image> create_test_image(
    const sight::data::image::size_t& _size,
    sight::core::type _type
)
{
    auto image = std::make_shared<sight::data::image>();

    image->resize(_size, _type, sight::data::image::gray_scale);
    image->set_spacing({1.0, 1.0, 1.0});

    return image;
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::filter::image::relabel")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic_uint8_mapping")
    {
        // Create a simple 2x2x2 image with values 1, 2, 3, 4
        auto image = create_test_image({2, 2, 2}, sight::core::type::UINT8);

        const auto dump_lock = image->dump_lock();
        {
            image->at<std::uint8_t>(0, 0, 0) = 1;
            image->at<std::uint8_t>(1, 0, 0) = 2;
            image->at<std::uint8_t>(0, 1, 0) = 3;
            image->at<std::uint8_t>(1, 1, 0) = 4;
            image->at<std::uint8_t>(0, 0, 1) = 1;
            image->at<std::uint8_t>(1, 0, 1) = 2;
            image->at<std::uint8_t>(0, 1, 1) = 3;
            image->at<std::uint8_t>(1, 1, 1) = 4;
        }

        // Create a dictionary: 1->10, 2->20, 3->30, 4->40, others unmapped (-1)
        std::map<std::int32_t, std::int32_t> dictionary;
        dictionary[1] = 10;
        dictionary[2] = 20;
        dictionary[3] = 30;
        dictionary[4] = 40;

        // Apply relabel
        sight::filter::image::relabel(*image, dictionary);

        // Check results
        {
            CHECK_EQ(image->at<std::uint8_t>(0, 0, 0), 10);
            CHECK_EQ(image->at<std::uint8_t>(1, 0, 0), 20);
            CHECK_EQ(image->at<std::uint8_t>(0, 1, 0), 30);
            CHECK_EQ(image->at<std::uint8_t>(1, 1, 0), 40);
            CHECK_EQ(image->at<std::uint8_t>(0, 0, 1), 10);
            CHECK_EQ(image->at<std::uint8_t>(1, 0, 1), 20);
            CHECK_EQ(image->at<std::uint8_t>(0, 1, 1), 30);
            CHECK_EQ(image->at<std::uint8_t>(1, 1, 1), 40);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("with_default_value")
    {
        // Create image with values 1, 2, 99 (unmapped)
        auto image = create_test_image({3, 1, 1}, sight::core::type::UINT8);

        const auto dump_lock = image->dump_lock();
        {
            image->at<std::uint8_t>(0, 0, 0) = 1;
            image->at<std::uint8_t>(1, 0, 0) = 2;
            image->at<std::uint8_t>(2, 0, 0) = 99;
        }

        // Dictionary with only 1 and 2 mapped
        std::map<std::int32_t, std::int32_t> dictionary;
        dictionary[1] = 100;
        dictionary[2] = 200;

        // Apply relabel with default value 50
        sight::filter::image::relabel(*image, dictionary, std::optional<std::uint8_t>(50));

        // Check results
        {
            CHECK_EQ(image->at<std::uint8_t>(0, 0, 0), 100);
            CHECK_EQ(image->at<std::uint8_t>(1, 0, 0), 200);
            CHECK_EQ(image->at<std::uint8_t>(2, 0, 0), 50); // Unmapped, uses default
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("without_default_leaves_unmapped")
    {
        // Create image with values 1, 2, 99 (unmapped)
        auto image = create_test_image({3, 1, 1}, sight::core::type::UINT8);

        const auto dump_lock = image->dump_lock();
        {
            image->at<std::uint8_t>(0, 0, 0) = 1;
            image->at<std::uint8_t>(1, 0, 0) = 2;
            image->at<std::uint8_t>(2, 0, 0) = 99;
        }

        // Dictionary with only 1 and 2 mapped
        std::map<std::int32_t, std::int32_t> dictionary;
        dictionary[1] = 100;
        dictionary[2] = 200;

        // Apply relabel WITHOUT default value
        sight::filter::image::relabel(*image, dictionary);

        // Check results
        {
            CHECK_EQ(image->at<std::uint8_t>(0, 0, 0), 100);
            CHECK_EQ(image->at<std::uint8_t>(1, 0, 0), 200);
            CHECK_EQ(image->at<std::uint8_t>(2, 0, 0), 99); // Unmapped, unchanged
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("uint16_type")
    {
        // Test with uint16_t type
        auto image = create_test_image({2, 1, 1}, sight::core::type::UINT16);

        const auto dump_lock = image->dump_lock();
        {
            image->at<std::uint16_t>(0, 0, 0) = 5;
            image->at<std::uint16_t>(1, 0, 0) = 10;
        }

        std::map<std::int32_t, std::int32_t> dictionary;
        dictionary[5]  = 50;
        dictionary[10] = 100;

        sight::filter::image::relabel(*image, dictionary);

        {
            CHECK_EQ(image->at<std::uint16_t>(0, 0, 0), 50);
            CHECK_EQ(image->at<std::uint16_t>(1, 0, 0), 100);
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
