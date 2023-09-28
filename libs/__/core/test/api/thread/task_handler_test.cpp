/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "task_handler_test.hpp"

#include <core/spy_log.hpp>
#include <core/thread/task_handler.hpp>
#include <core/thread/worker.hpp>

#include <exception>
#include <iostream>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::thread::ut::task_handler_test);

namespace sight::core::thread::ut
{

//------------------------------------------------------------------------------

void task_handler_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void task_handler_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

int copy(int val)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return val;
}

//-----------------------------------------------------------------------------

void throw_exception()
{
    throw std::exception();
}

//-----------------------------------------------------------------------------

void task_handler_test::basic_test()
{
    core::thread::worker::sptr worker = core::thread::worker::make();

    std::packaged_task<int()> task([]{return copy(5);});
    std::future<int> future = task.get_future();
    std::function<void()> f = move_task_into_function(task);

    std::packaged_task<int()> task2([]{return copy(8);});
    std::future<int> future2 = task2.get_future();
    std::function<void()> f2 = move_task_into_function(task2);

    worker->post(f);
    worker->post(f2);

    CPPUNIT_ASSERT(future.valid());
    CPPUNIT_ASSERT(future2.valid());

    future.wait();
    CPPUNIT_ASSERT(future.valid());
    CPPUNIT_ASSERT_EQUAL(5, future.get());
    CPPUNIT_ASSERT(!future.valid());

    future2.wait();
    CPPUNIT_ASSERT(future2.valid());
    CPPUNIT_ASSERT_EQUAL(8, future2.get());
    CPPUNIT_ASSERT(!future2.valid());

    std::packaged_task<int()> task3([]{return copy(5);});
    std::future<int> future3 = task3.get_future();
    std::function<void()> f3 = move_task_into_function(task3);

    std::packaged_task<int()> task4([]{return copy(8);});
    std::future<int> future4 = task4.get_future();
    std::function<void()> f4 = move_task_into_function(task4);

    worker->post(f3);
    worker->post(f4);

    CPPUNIT_ASSERT(future3.valid());
    CPPUNIT_ASSERT(future4.valid());

    worker->stop();

    CPPUNIT_ASSERT(future3.valid());
    CPPUNIT_ASSERT_EQUAL(5, future3.get());
    CPPUNIT_ASSERT(!future3.valid());

    CPPUNIT_ASSERT(future4.valid());
    CPPUNIT_ASSERT_EQUAL(8, future4.get());
    CPPUNIT_ASSERT(!future4.valid());
}

//-----------------------------------------------------------------------------

void task_handler_test::exception_test()
{
    core::thread::worker::sptr worker = core::thread::worker::make();

    std::packaged_task<void()> task([]{return throw_exception();});
    std::future<void> future = task.get_future();
    std::function<void()> f  = move_task_into_function(task);

    worker->post(f);

    future.wait();
    worker->stop();

    bool exception_is_catched = false;
    try
    {
        future.get();
    }
    catch(std::exception&)
    {
        exception_is_catched = true;
    }
    CPPUNIT_ASSERT(exception_is_catched);
}

//-----------------------------------------------------------------------------

} // namespace sight::core::thread::ut
