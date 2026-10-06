/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <algorithm>
#include <data/image.hpp>

#include <filter/image/threshold.hpp>

#include <doctest/doctest.h>

#include <array>
#include <cstdint>
#include <memory>

TEST_SUITE("sight::filter::image::threshold")
{
//-----------------------------------------------------------------------------

    TEST_CASE("binary_threshold")
    {
        auto input = std::make_shared<sight::data::image>();
        input->resize({5, 1, 1}, sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
        input->set_spacing({1., 1., 1.});
        input->set_origin({0., 0., 0.});
        input->set_orientation({1., 0., 0., 0., 1., 0., 0., 0., 1.});

        {
            const auto dump_lock = input->dump_lock();
            const std::array<std::uint8_t, 5> values {1, 2, 10, 28, 29};
            std::ranges::copy(values, input->begin<std::uint8_t>());
        }

        auto output = std::make_shared<sight::data::image>();
        sight::filter::image::threshold(*input, 2, 28, *output);

        const auto dump_lock = output->dump_lock();
        const std::array<std::uint8_t, 5> expected {0, 255, 255, 255, 0};
        CHECK(std::equal(expected.begin(), expected.end(), output->begin<std::uint8_t>()));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("preserve_values_in_range")
    {
        auto input = std::make_shared<sight::data::image>();
        input->resize({5, 1, 1}, sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
        input->set_spacing({1., 1., 1.});
        input->set_origin({0., 0., 0.});
        input->set_orientation({1., 0., 0., 0., 1., 0., 0., 0., 1.});

        {
            const auto dump_lock = input->dump_lock();
            const std::array<std::uint8_t, 5> values {1, 2, 10, 28, 29};
            std::ranges::copy(values, input->begin<std::uint8_t>());
        }

        auto output = std::make_shared<sight::data::image>();
        sight::filter::image::threshold(*input, 2, 28, *output, false);

        const auto dump_lock = output->dump_lock();
        const std::array<std::uint8_t, 5> expected {0, 2, 10, 28, 0};
        CHECK(std::equal(expected.begin(), expected.end(), output->begin<std::uint8_t>()));
    }

//-----------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::threshold")
