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

#include "core/timer.hpp"

#include <doctest/doctest.h>

#include <array>
#include <thread>

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
    sight::core::clock::type (sight::core::timer::* get_elapsed_time_in_x)() const;
};

TEST_SUITE("sight::core::timer")
{
    TEST_CASE("get_time")
    {
        const std::array data {
            test_data {.time                  = 1, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                       .reset                 = -1,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_sec
            },
            test_data {.time                  = 1000, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                       .reset                 = -1,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_milli_sec
            },
            test_data {.time                  = 1000000, .start = true, .wait = true, .stop = true,
                       .comp                  = comparison::ge, .reset = -1,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_micro_sec
            },

            test_data {.time                  = 1, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                       .reset                 = 0,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_sec
            },
            test_data {.time                  = 1000, .start = true, .wait = true, .stop = true, .comp = comparison::ge,
                       .reset                 = 0,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_milli_sec
            },
            test_data {.time                  = 1000000, .start = true, .wait = true, .stop = true,
                       .comp                  = comparison::ge, .reset = 0,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_micro_sec
            },

            test_data {.time                  = 1, .start = true, .wait = true, .stop = false, .comp = comparison::ge,
                       .reset                 = -1,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_sec
            },
            test_data {.time                  = 1000, .start = true, .wait = true, .stop = false,
                       .comp                  = comparison::ge, .reset = -1,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_milli_sec
            },
            test_data {.time                  = 1000000, .start = true, .wait = true, .stop = false,
                       .comp                  = comparison::ge, .reset = -1,
                       .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_micro_sec
            },

            test_data {.time  = 1, .start = false, .wait = false, .stop = false, .comp = comparison::eq,
                       .reset = 1000000, .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_sec
            },
            test_data {.time  = 1000, .start = false, .wait = false, .stop = false, .comp = comparison::eq,
                       .reset = 1000000, .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_milli_sec
            },
            test_data {.time  = 1000000, .start = false, .wait = false, .stop = false, .comp = comparison::eq,
                       .reset = 1000000, .get_elapsed_time_in_x = &sight::core::timer::get_elapsed_time_in_micro_sec
            }
        };

        for(const auto& d : data)
        {
            const double time = d.time;
            sight::core::timer timer;

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
                CHECK((timer.*d.get_elapsed_time_in_x)() == time);
            }
            else if(d.comp == comparison::ge)
            {
                CHECK((timer.*d.get_elapsed_time_in_x)() >= time);
            }
        }
    }

    TEST_CASE("cumulative_time")
    {
        sight::core::timer t;
        t.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        t.stop();
        const auto elapsed1 = t.get_elapsed_time_in_milli_sec();

        t.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        t.stop();
        const auto elapsed2 = t.get_elapsed_time_in_milli_sec();

        CHECK(elapsed2 >= 200.0);
        CHECK(elapsed2 >= elapsed1);
    }
}
