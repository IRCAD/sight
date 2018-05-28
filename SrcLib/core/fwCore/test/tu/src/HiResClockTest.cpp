/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "HiResClockTest.hpp"

#include "fwCore/HiResClock.hpp"

#include <ios>
#include <thread>

#ifndef WIN32
#include <sys/time.h>
#endif

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCore::ut::HisResClockTest );

namespace fwCore
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
    unsigned short time  = 1000;
    unsigned short delta = 10;

#ifndef WIN32
    {
        timeval count;
        gettimeofday(&count, NULL);

        auto start = (count.tv_sec * 1000000.0) + count.tv_usec * 0.001;
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        auto end = HiResClock::getTimeInMilliSec();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(end-start, time, delta);
    }
#endif
    {
        auto start = HiResClock::getTimeInMilliSec();
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        auto end = HiResClock::getTimeInMilliSec();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(end-start, time, delta);
    }
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwCore
