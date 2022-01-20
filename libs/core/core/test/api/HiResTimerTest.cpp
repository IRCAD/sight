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

//------------------------------------------------------------------------------

void HiResTimerTest::getTimeTest()
{
    // Start the timer, stop the timer and get the time
    {
        const unsigned short time = 1;
        HiResTimer timer;

        timer.start();
        std::this_thread::sleep_for(std::chrono::seconds(time));
        timer.stop();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInSec());
    }

    {
        const unsigned short time = 1000;
        HiResTimer timer;

        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        timer.stop();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInMilliSec());
    }

    {
        const unsigned time = 1000000;
        HiResTimer timer;

        timer.start();
        std::this_thread::sleep_for(std::chrono::microseconds(time));
        timer.stop();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInMicroSec());
    }

    // Start the timer, reset it in the middle, stop it and get the time
    {
        const unsigned short time = 1;
        HiResTimer timer;

        timer.start();
        timer.reset();
        std::this_thread::sleep_for(std::chrono::seconds(time));
        timer.stop();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInSec());
    }

    {
        const unsigned short time = 1000;
        HiResTimer timer;

        timer.start();
        timer.reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        timer.stop();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInMilliSec());
    }

    {
        const unsigned time = 1000000;
        HiResTimer timer;

        timer.start();
        timer.reset();
        std::this_thread::sleep_for(std::chrono::microseconds(time));
        timer.stop();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInMicroSec());
    }

    // Start the timer and get the time
    {
        const unsigned short time = 1;
        HiResTimer timer;

        timer.start();
        std::this_thread::sleep_for(std::chrono::seconds(time));

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInSec());
    }

    {
        const unsigned short time = 1000;
        HiResTimer timer;

        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(time));

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInMilliSec());
    }

    {
        const unsigned time = 1000000;
        HiResTimer timer;

        timer.start();
        std::this_thread::sleep_for(std::chrono::microseconds(time));

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), timer.getElapsedTimeInMicroSec());
    }

    // Reset the timer and get the time
    {
        const unsigned short time = 1;
        HiResTimer timer;

        timer.reset(time * 1000000);

        CPPUNIT_ASSERT_EQUAL(static_cast<double>(time), timer.getElapsedTimeInSec());
    }

    {
        const unsigned short time = 1000;
        HiResTimer timer;

        timer.reset(time * 1000);

        CPPUNIT_ASSERT_EQUAL(static_cast<double>(time), timer.getElapsedTimeInMilliSec());
    }

    {
        const unsigned time = 1000000;
        HiResTimer timer;

        timer.reset(time);

        CPPUNIT_ASSERT_EQUAL(static_cast<double>(time), timer.getElapsedTimeInMicroSec());
    }
}

} // namespace sight::core
