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

#include <core/round.hpp>

#include <doctest/doctest.h>

#include <cstdint>

TEST_SUITE("sight::core::round_half_to_even")
{
//------------------------------------------------------------------------------

    TEST_CASE("round_toward_even_on_half_double")
    {
        // 0.5 -> 0 (0 is even)
        CHECK_EQ(std::int64_t(0), sight::core::round_half_to_even(0.5));
        // 1.5 -> 2 (2 is even)
        CHECK_EQ(std::int64_t(2), sight::core::round_half_to_even(1.5));
        // 2.5 -> 2 (2 is even)
        CHECK_EQ(std::int64_t(2), sight::core::round_half_to_even(2.5));
        // 3.5 -> 4 (4 is even)
        CHECK_EQ(std::int64_t(4), sight::core::round_half_to_even(3.5));
        // 4.5 -> 4 (4 is even)
        CHECK_EQ(std::int64_t(4), sight::core::round_half_to_even(4.5));
        // 5.5 -> 6 (6 is even)
        CHECK_EQ(std::int64_t(6), sight::core::round_half_to_even(5.5));
    }

//------------------------------------------------------------------------------

    TEST_CASE("round_toward_even_on_half_negative_double")
    {
        // -0.5 -> 0 (0 is even)
        CHECK_EQ(std::int64_t(0), sight::core::round_half_to_even(-0.5));
        // -1.5 -> -2 (-2 is even)
        CHECK_EQ(std::int64_t(-2), sight::core::round_half_to_even(-1.5));
        // -2.5 -> -2 (-2 is even)
        CHECK_EQ(std::int64_t(-2), sight::core::round_half_to_even(-2.5));
        // -3.5 -> -4 (-4 is even)
        CHECK_EQ(std::int64_t(-4), sight::core::round_half_to_even(-3.5));
        // -4.5 -> -4 (-4 is even)
        CHECK_EQ(std::int64_t(-4), sight::core::round_half_to_even(-4.5));
    }

//------------------------------------------------------------------------------

    TEST_CASE("round_non_half_double")
    {
        CHECK_EQ(std::int64_t(1), sight::core::round_half_to_even(1.2));
        CHECK_EQ(std::int64_t(2), sight::core::round_half_to_even(1.7));
        CHECK_EQ(std::int64_t(0), sight::core::round_half_to_even(0.1));
        CHECK_EQ(std::int64_t(1), sight::core::round_half_to_even(0.9));
        CHECK_EQ(std::int64_t(-1), sight::core::round_half_to_even(-1.2));
        CHECK_EQ(std::int64_t(-2), sight::core::round_half_to_even(-1.7));
    }

//------------------------------------------------------------------------------

    TEST_CASE("round_toward_even_on_half_float")
    {
        CHECK_EQ(std::int64_t(0), sight::core::round_half_to_even(0.5F));
        CHECK_EQ(std::int64_t(2), sight::core::round_half_to_even(1.5F));
        CHECK_EQ(std::int64_t(2), sight::core::round_half_to_even(2.5F));
        CHECK_EQ(std::int64_t(4), sight::core::round_half_to_even(3.5F));
        CHECK_EQ(std::int64_t(0), sight::core::round_half_to_even(-0.5F));
        CHECK_EQ(std::int64_t(-2), sight::core::round_half_to_even(-1.5F));
    }

//------------------------------------------------------------------------------

    TEST_CASE("round_custom_return_type")
    {
        CHECK_EQ(int(2), sight::core::round_half_to_even<int>(1.5));
        CHECK_EQ(int(2), sight::core::round_half_to_even<int>(2.5));
        CHECK_EQ(std::int32_t(4), sight::core::round_half_to_even<std::int32_t>(3.5));
        CHECK_EQ(std::int16_t(6), sight::core::round_half_to_even<std::int16_t>(5.5));
    }

//------------------------------------------------------------------------------

    TEST_CASE("round_integer_values")
    {
        CHECK_EQ(std::int64_t(0), sight::core::round_half_to_even(0.0));
        CHECK_EQ(std::int64_t(1), sight::core::round_half_to_even(1.0));
        CHECK_EQ(std::int64_t(-1), sight::core::round_half_to_even(-1.0));
        CHECK_EQ(std::int64_t(100), sight::core::round_half_to_even(100.0));
    }
} // TEST_SUITE
