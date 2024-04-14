/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "clock_test.hpp"

#include "core/clock.hpp"

#include <ios>
#include <thread>

#ifndef WIN32
#include <sys/time.h>
#endif

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::clock_test);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void clock_test::setUp()
{
}

//-----------------------------------------------------------------------------

void clock_test::tearDown()
{
}

//-----------------------------------------------------------------------------

void clock_test::get_time_test()
{
    {
        const std::uint16_t time = 1;

        const double start = clock::get_time_in_sec();
        std::this_thread::sleep_for(std::chrono::seconds(time));
        const double end = clock::get_time_in_sec();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), end - start);
    }

    {
        const std::uint16_t time = 1000;

        const double start = clock::get_time_in_milli_sec();
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        const double end = clock::get_time_in_milli_sec();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), end - start);
    }

    {
        const unsigned time = 1000000;

        const double start = clock::get_time_in_micro_sec();
        std::this_thread::sleep_for(std::chrono::microseconds(time));
        const double end = clock::get_time_in_micro_sec();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), end - start);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::core::ut
