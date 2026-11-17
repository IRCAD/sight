/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <exception>
#include <thread>

TEST_SUITE("sight::core::thread::task_handler")
{
//-----------------------------------------------------------------------------

    static int copy(int _val)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return _val;
    }

//-----------------------------------------------------------------------------

    static void throw_exception()
    {
        throw std::exception();
    }

//-----------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        auto worker = sight::core::thread::worker::make();

        std::packaged_task<int()> task([]{return copy(5);});
        std::future<int> future = task.get_future();
        std::function<void()> f = sight::core::thread::move_task_into_function(task);

        std::packaged_task<int()> task2([]{return copy(8);});
        std::future<int> future2 = task2.get_future();
        std::function<void()> f2 = sight::core::thread::move_task_into_function(task2);

        worker->post(f);
        worker->post(f2);

        CHECK(future.valid());
        CHECK(future2.valid());

        future.wait();
        CHECK(future.valid());
        CHECK_EQ(5, future.get());
        CHECK(!future.valid());

        future2.wait();
        CHECK(future2.valid());
        CHECK_EQ(8, future2.get());
        CHECK(!future2.valid());

        std::packaged_task<int()> task3([]{return copy(5);});
        std::future<int> future3 = task3.get_future();
        std::function<void()> f3 = sight::core::thread::move_task_into_function(task3);

        std::packaged_task<int()> task4([]{return copy(8);});
        std::future<int> future4 = task4.get_future();
        std::function<void()> f4 = sight::core::thread::move_task_into_function(task4);

        worker->post(f3);
        worker->post(f4);

        CHECK(future3.valid());
        CHECK(future4.valid());

        worker->stop();

        CHECK(future3.valid());
        CHECK_EQ(5, future3.get());
        CHECK(!future3.valid());

        CHECK(future4.valid());
        CHECK_EQ(8, future4.get());
        CHECK(!future4.valid());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("exception")
    {
        auto worker = sight::core::thread::worker::make();

        std::packaged_task<void()> task([]{throw_exception(); return;});
        std::future<void> future = task.get_future();
        std::function<void()> f  = sight::core::thread::move_task_into_function(task);

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
        CHECK(exception_is_catched);
    }
} // TEST_SUITE
