/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "LazyInstantiatorTest.hpp"

#include <fwCore/util/LazyInstantiator.hpp>
#include <fwCore/mt/types.hpp>

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCore::ut::LazyInstantiatorTest );

namespace fwCore
{
namespace ut
{

void LazyInstantiatorTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

template < int SLEEP = 0 >
class StaticCounter
{
public:
    typedef std::shared_ptr< StaticCounter > sptr;

    StaticCounter()
    {
        ::fwCore::mt::ScopedLock lock(s_mutex);
        ++s_counter;
        std::this_thread::sleep_for( std::chrono::seconds(SLEEP));
    }

    static int s_counter;
    static ::fwCore::mt::Mutex s_mutex;
};

template < int SLEEP >
int StaticCounter<SLEEP>::s_counter = 0;

template < int SLEEP >
::fwCore::mt::Mutex StaticCounter<SLEEP>::s_mutex;

struct second_counter {};

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::lazyTest()
{
    StaticCounter<>::s_counter = 0;

    StaticCounter<> cpt;

    CPPUNIT_ASSERT_EQUAL(1, cpt.s_counter);

    StaticCounter<>::sptr counter1 = ::fwCore::util::LazyInstantiator< StaticCounter<> >::getInstance();
    CPPUNIT_ASSERT_EQUAL(2, counter1->s_counter);

    StaticCounter<>::sptr counter2 = ::fwCore::util::LazyInstantiator< StaticCounter<> >::getInstance();
    CPPUNIT_ASSERT_EQUAL(2, counter2->s_counter);

    StaticCounter<>::sptr counter3 = ::fwCore::util::LazyInstantiator< StaticCounter<>, second_counter >::getInstance();
    CPPUNIT_ASSERT_EQUAL(3, counter3->s_counter);

    StaticCounter<>::s_counter = 0;
}

//-----------------------------------------------------------------------------

struct thread_counter_tag {};

struct CounterThread
{
    typedef StaticCounter<5> CounterType;
    typedef std::shared_ptr< CounterThread > sptr;

    CounterThread()
    {
    }

    void run ()
    {
        CounterType::sptr counter;
        counter = ::fwCore::util::LazyInstantiator< CounterType, thread_counter_tag >::getInstance();
    }
};

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::threadSafetyTest()
{
    const int NB_THREAD(100);
    CounterThread::CounterType::s_counter = 0;

    std::vector< std::thread > tg;
    for(size_t i = 0; i <= NB_THREAD; i++)
    {
        CounterThread::sptr ct = std::make_shared<CounterThread>();
        tg.push_back(std::thread(std::bind(&CounterThread::run, ct) ));
    }
    for(auto& t : tg)
    {
        t.join();
    }

    CounterThread::CounterType::sptr counter;
    counter = ::fwCore::util::LazyInstantiator< CounterThread::CounterType, thread_counter_tag >::getInstance();
    CPPUNIT_ASSERT_EQUAL(1, counter->s_counter);

    CounterThread::CounterType::s_counter = 0;
}

} //namespace ut
} //namespace fwCore
