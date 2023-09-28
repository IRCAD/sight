/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "hires_timer_test.hpp"

#include "core/hires_timer.hpp"

#include <cppunit/extensions/HelperMacros.h>

#include <array>
#include <thread>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::hires_timer_test);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void hires_timer_test::setUp()
{
}

//------------------------------------------------------------------------------

void hires_timer_test::tearDown()
{
}

enum class comparison
{
    EQ,
    GE
};

struct test_data
{
    double time;
    bool start;
    bool wait;
    bool stop;
    comparison comp;
    int reset;
    core::hires_clock::type (hires_timer::* get_elapsed_time_in_x)();
};

//------------------------------------------------------------------------------

void hires_timer_test::get_time_test()
{
    const std::array data {
        // Start the timer, stop the timer and get the time
        test_data {1, true, true, true, comparison::GE, -1, &hires_timer::get_elapsed_time_in_sec},
        test_data {1000, true, true, true, comparison::GE, -1, &hires_timer::get_elapsed_time_in_milli_sec},
        test_data {1000000, true, true, true, comparison::GE, -1, &hires_timer::get_elapsed_time_in_micro_sec},

        // Start the timer, reset it in the middle, stop it and get the time
        test_data {1, true, true, true, comparison::GE, 0, &hires_timer::get_elapsed_time_in_sec},
        test_data {1000, true, true, true, comparison::GE, 0, &hires_timer::get_elapsed_time_in_milli_sec},
        test_data {1000000, true, true, true, comparison::GE, 0, &hires_timer::get_elapsed_time_in_micro_sec},

        // Start the timer and get the time
        test_data {1, true, true, false, comparison::GE, -1, &hires_timer::get_elapsed_time_in_sec},
        test_data {1000, true, true, false, comparison::GE, -1, &hires_timer::get_elapsed_time_in_milli_sec},
        test_data {1000000, true, true, false, comparison::GE, -1, &hires_timer::get_elapsed_time_in_micro_sec},

        // Reset the timer and get the time
        test_data {1, false, false, false, comparison::EQ, 1000000, &hires_timer::get_elapsed_time_in_sec},
        test_data {1000, false, false, false, comparison::EQ, 1000000, &hires_timer::get_elapsed_time_in_milli_sec},
        test_data {1000000, false, false, false, comparison::EQ, 1000000, &hires_timer::get_elapsed_time_in_micro_sec}
    };

    for(size_t i = 0 ; i < sizeof(data) / sizeof(data[0]) ; i++)
    {
        const test_data& d = data[i];
        const double time  = d.time;
        hires_timer timer;

        if(d.start)
        {
            timer.start();
        }

        if(d.reset > -1)
        {
            timer.reset(d.reset);
        }

        if(d.wait)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if(d.stop)
        {
            timer.stop();
        }

        if(d.comp == comparison::EQ)
        {
            CPPUNIT_ASSERT_EQUAL(time, (timer.*d.get_elapsed_time_in_x)());
        }
        else if(d.comp == comparison::GE)
        {
            CPPUNIT_ASSERT_GREATEREQUAL(time, (timer.*d.get_elapsed_time_in_x)());
        }
    }
}

} // namespace sight::core::ut
