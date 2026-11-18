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

#include "core/clock.hpp"

#include <doctest/doctest.h>

#include <chrono>

//------------------------------------------------------------------------------

template<typename Unit>
static double since_epoch(std::chrono::system_clock::time_point _time)
{
    return static_cast<double>(std::chrono::duration_cast<Unit>(_time.time_since_epoch()).count());
}

TEST_SUITE("sight::core::clock")
{
    // The wall clock can be corrected at any time (NTP), so a duration slept is no bound on its progress: each value
    // lies between two readings of the system clock taken around it.
    TEST_CASE("get_time_in_sec")
    {
        const auto before = std::chrono::system_clock::now();
        const double time = sight::core::clock::get_time_in_sec();
        const auto after  = std::chrono::system_clock::now();
        CHECK(time >= since_epoch<std::chrono::seconds>(before));
        CHECK(time <= since_epoch<std::chrono::seconds>(after));
    }

    TEST_CASE("get_time_in_milli_sec")
    {
        const auto before = std::chrono::system_clock::now();
        const double time = sight::core::clock::get_time_in_milli_sec();
        const auto after  = std::chrono::system_clock::now();
        CHECK(time >= since_epoch<std::chrono::milliseconds>(before));
        CHECK(time <= since_epoch<std::chrono::milliseconds>(after));
    }

    TEST_CASE("get_time_in_micro_sec")
    {
        const auto before = std::chrono::system_clock::now();
        const double time = sight::core::clock::get_time_in_micro_sec();
        const auto after  = std::chrono::system_clock::now();
        CHECK(time >= since_epoch<std::chrono::microseconds>(before));
        CHECK(time <= since_epoch<std::chrono::microseconds>(after));
    }
}
