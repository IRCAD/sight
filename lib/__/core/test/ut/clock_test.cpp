/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <thread>

TEST_SUITE("sight::core::clock")
{
    TEST_CASE("get_time_in_sec")
    {
        const std::uint16_t time = 1;
        const double start       = sight::core::clock::get_time_in_sec();
        std::this_thread::sleep_for(std::chrono::seconds(time));
        const double end = sight::core::clock::get_time_in_sec();
        CHECK((end - start) >= static_cast<double>(time));
    }

    TEST_CASE("get_time_in_milli_sec")
    {
        const std::uint16_t time = 1000;
        const double start       = sight::core::clock::get_time_in_milli_sec();
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        const double end = sight::core::clock::get_time_in_milli_sec();
        CHECK((end - start) >= static_cast<double>(time));
    }

    TEST_CASE("get_time_in_micro_sec")
    {
        const unsigned time = 1000000;
        const double start  = sight::core::clock::get_time_in_micro_sec();
        std::this_thread::sleep_for(std::chrono::microseconds(time));
        const double end = sight::core::clock::get_time_in_micro_sec();
        CHECK((end - start) >= static_cast<double>(time));
    }
}
