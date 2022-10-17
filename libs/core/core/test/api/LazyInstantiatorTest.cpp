/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/LazyInstantiator.hpp>
#include <core/mt/types.hpp>

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::LazyInstantiatorTest);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

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

template<int SLEEP = 0>
class StaticCounter
{
public:

    using sptr = std::shared_ptr<StaticCounter>;

    StaticCounter()
    {
        core::mt::ScopedLock lock(s_mutex);
        ++s_counter;
        std::this_thread::sleep_for(std::chrono::seconds(SLEEP));
    }

    static int s_counter;
    static core::mt::Mutex s_mutex;
};

template<int SLEEP>
int StaticCounter<SLEEP>::s_counter = 0;

template<int SLEEP>
core::mt::Mutex StaticCounter<SLEEP>::s_mutex;

struct SecondCounter {};

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::lazyTest()
{
    StaticCounter<>::s_counter = 0;

    StaticCounter<> cpt;

    CPPUNIT_ASSERT_EQUAL(1, cpt.s_counter);

    StaticCounter<>::sptr counter1 = core::LazyInstantiator<StaticCounter<> >::getInstance();
    CPPUNIT_ASSERT_EQUAL(2, counter1->s_counter);

    StaticCounter<>::sptr counter2 = core::LazyInstantiator<StaticCounter<> >::getInstance();
    CPPUNIT_ASSERT_EQUAL(2, counter2->s_counter);

    StaticCounter<>::sptr counter3 = core::LazyInstantiator<StaticCounter<>, SecondCounter>::getInstance();
    CPPUNIT_ASSERT_EQUAL(3, counter3->s_counter);

    StaticCounter<>::s_counter = 0;
}

//-----------------------------------------------------------------------------

struct ThreadCounterTag {};

struct CounterThread
{
    using CounterType = StaticCounter<5>;
    using sptr        = std::shared_ptr<CounterThread>;

    CounterThread()
    = default;

    //------------------------------------------------------------------------------

    static void run()
    {
        CounterType::sptr counter;
        counter = core::LazyInstantiator<CounterType, ThreadCounterTag>::getInstance();
    }
};

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::threadSafetyTest()
{
    const int NB_THREAD(100);
    CounterThread::CounterType::s_counter = 0;

    std::vector<std::thread> tg;
    for(std::size_t i = 0 ; i <= NB_THREAD ; i++)
    {
        CounterThread::sptr ct = std::make_shared<CounterThread>();
        tg.emplace_back(&CounterThread::run);
    }

    for(auto& t : tg)
    {
        t.join();
    }

    CounterThread::CounterType::sptr counter;
    counter = core::LazyInstantiator<CounterThread::CounterType, ThreadCounterTag>::getInstance();
    CPPUNIT_ASSERT_EQUAL(1, counter->s_counter);

    CounterThread::CounterType::s_counter = 0;
}

} // namespace sight::core::ut
