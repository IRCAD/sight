/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "timer_test.hpp"

#include "core/timer.hpp"

#include <cppunit/extensions/HelperMacros.h>

#include <array>
#include <thread>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::timer_test);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void timer_test::setUp()
{
}

//------------------------------------------------------------------------------

void timer_test::tearDown()
{
}

enum class comparison : std::uint8_t
{
    eq,
    ge
};

struct test_data
{
    double time;
    bool start;
    bool wait;
    bool stop;
    comparison comp;
    int reset;
    core::clock::type (timer::* get_elapsed_time_in_x)() const;
};

//------------------------------------------------------------------------------

void timer_test::get_time_test()
{
    const std::array data {
        // Start the timer, stop the timer and get the time
        test_data {.time                  = 1, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                   .reset                 = -1,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_sec
        },
        test_data {.time                  = 1000, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                   .reset                 = -1,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_milli_sec
        },
        test_data {.time                  = 1000000, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                   .reset                 = -1,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_micro_sec
        },

        // Start the timer, reset it in the middle, stop it and get the time
        test_data {.time                  = 1, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                   .reset                 = 0,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_sec
        },
        test_data {.time                  = 1000, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                   .reset                 = 0,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_milli_sec
        },
        test_data {.time                  = 1000000, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                   .reset                 = 0,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_micro_sec
        },

        // Start the timer and get the time
        test_data {.time                  = 1, .start = true, .wait = true, .stop = false, .comp = comparison::ge,
                   .reset                 = -1,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_sec
        },
        test_data {.time                  = 1000, .start = true, .wait = true, .stop = false, .comp = comparison::ge,
                   .reset                 = -1,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_milli_sec
        },
        test_data {.time                  = 1000000, .start = true, .wait = true, .stop = false, .comp = comparison::ge,
                   .reset                 = -1,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_micro_sec
        },

        // Reset the timer and get the time
        test_data {.time                  = 1, .start = false, .wait = false, .stop = false, .comp = comparison::eq,
                   .reset                 = 1000000,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_sec
        },
        test_data {.time                  = 1000, .start = false, .wait = false, .stop = false, .comp = comparison::eq,
                   .reset                 = 1000000,
                   .get_elapsed_time_in_x = &timer::get_elapsed_time_in_milli_sec
        },
        test_data {.time  = 1000000, .start = false, .wait = false, .stop = false, .comp = comparison::eq,
                   .reset = 1000000, .get_elapsed_time_in_x = &timer::get_elapsed_time_in_micro_sec
        }
    };

    for(size_t i = 0 ; i < sizeof(data) / sizeof(data[0]) ; i++)
    {
        const test_data& d = data[i];
        const double time  = d.time;
        timer timer;

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

        if(d.comp == comparison::eq)
        {
            CPPUNIT_ASSERT_EQUAL(time, (timer.*d.get_elapsed_time_in_x)());
        }
        else if(d.comp == comparison::ge)
        {
            CPPUNIT_ASSERT_GREATEREQUAL(time, (timer.*d.get_elapsed_time_in_x)());
        }
    }
}

//------------------------------------------------------------------------------

void timer_test::cumulative_time_test()
{
    timer t;
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    t.stop();
    const auto elapsed1 = t.get_elapsed_time_in_milli_sec();

    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    t.stop();
    const auto elapsed2 = t.get_elapsed_time_in_milli_sec();

    // The second elapsed time should be at least 200ms
    CPPUNIT_ASSERT(elapsed2 >= 200.0);
    CPPUNIT_ASSERT(elapsed2 >= elapsed1);
}

} // namespace sight::core::ut
