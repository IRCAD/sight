/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "HiResClockTest.hpp"

#include "core/HiResClock.hpp"

#include <ios>
#include <thread>

#ifndef WIN32
#include <sys/time.h>
#endif

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::HisResClockTest);

namespace sight::core
{

namespace ut
{

//------------------------------------------------------------------------------

void HisResClockTest::setUp()
{
}

//-----------------------------------------------------------------------------

void HisResClockTest::tearDown()
{
}

//-----------------------------------------------------------------------------

void HisResClockTest::getTimeTest()
{
    {
        const unsigned short time = 1;

        const double start = HiResClock::getTimeInSec();
        std::this_thread::sleep_for(std::chrono::seconds(time));
        const double end = HiResClock::getTimeInSec();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), end - start);
    }

    {
        const unsigned short time = 1000;

        const double start = HiResClock::getTimeInMilliSec();
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        const double end = HiResClock::getTimeInMilliSec();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), end - start);
    }

    {
        const unsigned time = 1000000;

        const double start = HiResClock::getTimeInMicroSec();
        std::this_thread::sleep_for(std::chrono::microseconds(time));
        const double end = HiResClock::getTimeInMicroSec();

        CPPUNIT_ASSERT_GREATEREQUAL(static_cast<double>(time), end - start);
    }
}

//-----------------------------------------------------------------------------

} //namespace ut

} //namespace sight::core
