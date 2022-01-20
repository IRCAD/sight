/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "HiResTimerTest.hpp"

#include "core/HiResTimer.hpp"

#include <cppunit/extensions/HelperMacros.h>

#include <thread>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::HiResTimerTest);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void HiResTimerTest::setUp()
{
}

//------------------------------------------------------------------------------

void HiResTimerTest::tearDown()
{
}

enum class Comparison
{
    EQ,
    GE
};

struct TestData
{
    double time;
    bool start;
    bool wait;
    bool stop;
    Comparison comp;
    int reset;
    core::HiResClock::HiResClockType (HiResTimer::* getElapsedTimeInX)();
};

//------------------------------------------------------------------------------

void HiResTimerTest::getTimeTest()
{
    const TestData data[] = {
        // Start the timer, stop the timer and get the time
        {1, true, true, true, Comparison::GE, -1, &HiResTimer::getElapsedTimeInSec},
        {1000, true, true, true, Comparison::GE, -1, &HiResTimer::getElapsedTimeInMilliSec},
        {1000000, true, true, true, Comparison::GE, -1, &HiResTimer::getElapsedTimeInMicroSec},

        // Start the timer, reset it in the middle, stop it and get the time
        {1, true, true, true, Comparison::GE, 0, &HiResTimer::getElapsedTimeInSec},
        {1000, true, true, true, Comparison::GE, 0, &HiResTimer::getElapsedTimeInMilliSec},
        {1000000, true, true, true, Comparison::GE, 0, &HiResTimer::getElapsedTimeInMicroSec},

        // Start the timer and get the time
        {1, true, true, false, Comparison::GE, -1, &HiResTimer::getElapsedTimeInSec},
        {1000, true, true, false, Comparison::GE, -1, &HiResTimer::getElapsedTimeInMilliSec},
        {1000000, true, true, false, Comparison::GE, -1, &HiResTimer::getElapsedTimeInMicroSec},

        // Reset the timer and get the time
        {1, false, false, false, Comparison::EQ, 1000000, &HiResTimer::getElapsedTimeInSec},
        {1000, false, false, false, Comparison::EQ, 1000000, &HiResTimer::getElapsedTimeInMilliSec},
        {1000000, false, false, false, Comparison::EQ, 1000000, &HiResTimer::getElapsedTimeInMicroSec}
    };

    for(size_t i = 0 ; i < sizeof(data) / sizeof(data[0]) ; i++)
    {
        const TestData& d = data[i];
        const double time = d.time;
        HiResTimer timer;

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

        if(d.comp == Comparison::EQ)
        {
            CPPUNIT_ASSERT_EQUAL(time, (timer.*d.getElapsedTimeInX)());
        }
        else if(d.comp == Comparison::GE)
        {
            CPPUNIT_ASSERT_GREATEREQUAL(time, (timer.*d.getElapsedTimeInX)());
        }
    }
}

} // namespace sight::core
