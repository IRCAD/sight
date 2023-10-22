/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "worker_test.hpp"

#include <core/spy_log.hpp>
#include <core/thread/timer.hpp>
#include <core/thread/worker.hpp>

#include <utest/exception.hpp>

#include <atomic>
#include <exception>
#include <iostream>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::thread::ut::worker_test);

namespace sight::core::thread::ut
{

static utest::exception e(""); // force link with fwTest

//------------------------------------------------------------------------------

void worker_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void worker_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

struct test_handler
{
    test_handler() :
        m_constructor_thread_id(core::thread::get_current_thread_id())
    {
    }

    //------------------------------------------------------------------------------

    void next_step()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->next_step_no_sleep();
    }

    //------------------------------------------------------------------------------

    void next_step_no_sleep()
    {
        m_thread_check_ok = m_thread_check_ok.load()
                            && (m_constructor_thread_id != core::thread::get_current_thread_id())
                            && (m_worker_thread_id == core::thread::get_current_thread_id());

        ++m_step;
    }

    //------------------------------------------------------------------------------

    void set_worker_id(core::thread::thread_id_t _id)
    {
        m_worker_thread_id = _id;
    }

    std::atomic_int m_step {0};
    std::atomic_bool m_thread_check_ok {true};
    core::thread::thread_id_t m_constructor_thread_id;
    core::thread::thread_id_t m_worker_thread_id;
};

//-----------------------------------------------------------------------------

void worker_test::basic_test()
{
    {
        core::thread::worker::sptr worker = core::thread::worker::make();

        test_handler handler;
        handler.set_worker_id(worker->get_thread_id());
        worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});
        worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});
        worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});

        worker->stop();
        CPPUNIT_ASSERT_EQUAL(3, handler.m_step.load());
        CPPUNIT_ASSERT_EQUAL(true, handler.m_thread_check_ok.load());
    }

    {
        core::thread::worker::sptr worker = core::thread::worker::make();

        test_handler handler;
        handler.set_worker_id(worker->get_thread_id());
        worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
        worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
        worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});

        worker->stop();
        CPPUNIT_ASSERT_EQUAL(3, handler.m_step.load());
        CPPUNIT_ASSERT_EQUAL(true, handler.m_thread_check_ok.load());
    }
}

//-----------------------------------------------------------------------------

void worker_test::timer_test()
{
    core::thread::worker::sptr worker = core::thread::worker::make();

    test_handler handler;
    handler.set_worker_id(worker->get_thread_id());

    core::thread::timer::sptr timer = worker->create_timer();

    std::chrono::milliseconds duration = std::chrono::milliseconds(100);

    timer->set_function([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
    timer->set_duration(duration);

    CPPUNIT_ASSERT(!timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    timer->start();

    CPPUNIT_ASSERT(timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration / 10.);

    for(int i = 1 ; i < 50 ; ++i)
    {
        std::this_thread::sleep_for(duration);

        CPPUNIT_ASSERT(timer->is_running());
        CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(45, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(51, handler.m_step.load());

    timer->stop();

    int last_step = handler.m_step.load();
    std::this_thread::sleep_for(duration * 3);

    CPPUNIT_ASSERT(!timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_LESSEQUAL(last_step + 5, handler.m_step.load());

    // test start after stop
    handler.m_step.store(0);

    timer->start();

    CPPUNIT_ASSERT(timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration / 10.);

    for(int i = 1 ; i < 50 ; ++i)
    {
        std::this_thread::sleep_for(duration);
        CPPUNIT_ASSERT(timer->is_running());
        CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(40, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(51, handler.m_step.load());

    timer->stop();

    last_step = handler.m_step.load();
    std::this_thread::sleep_for(duration * 3);

    CPPUNIT_ASSERT(!timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_LESSEQUAL(last_step + 5, handler.m_step.load());

    // change timer duration on the fly
    // change timer duration
    handler.m_step.store(0);

    timer->start();

    CPPUNIT_ASSERT(timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration / 10.);

    for(int i = 1 ; i < 25 ; ++i)
    {
        std::this_thread::sleep_for(duration);

        CPPUNIT_ASSERT(timer->is_running());
        CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(20, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(30, handler.m_step.load());

    duration = std::chrono::milliseconds(50);
    timer->set_duration(duration);

    for(int i = 24 ; i < 50 ; ++i)
    {
        std::this_thread::sleep_for(duration);

        CPPUNIT_ASSERT(timer->is_running());
        CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(40, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(60, handler.m_step.load());

    timer->stop();

    last_step = handler.m_step.load();
    std::this_thread::sleep_for(duration * 3);

    CPPUNIT_ASSERT(!timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_LESSEQUAL(last_step + 5, handler.m_step.load());

    // one shot test
    handler.m_step.store(0);

    duration = std::chrono::milliseconds(10);
    timer->set_duration(duration);
    timer->set_one_shot(true);

    timer->start();

    CPPUNIT_ASSERT(timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration * 10);

    CPPUNIT_ASSERT(!timer->is_running());
    CPPUNIT_ASSERT(handler.m_thread_check_ok.load());
    CPPUNIT_ASSERT_GREATEREQUAL(1, handler.m_step.load());

    // This test was added to reproduce a bug that is now fixed
    // The timer could be deleted before the call back is over
    // To reproduce that issue, we need to ensure that the callee access the memory in the bounds of the timer
    // at the end of the callback
    // We could not derive easily from WorkerAsio to create the conditions of the crash, but you can modify it
    // if necessary to reproduce. It is quite hard to achieve since it depends on the actual runtime memory layout
    // - Declare a new *last* member in WorkerAsio -> int64_t m_checkMemory;
    // - Initialize it to 12345 in the constructor
    // - Assert that m_checkMemory==12345 in TimerAsio::call() at the end of the if
    // - You may need to uncomment the tests above
    {
        core::thread::timer::sptr timer2 = worker->create_timer();
        duration = std::chrono::milliseconds(10);
        timer2->set_function(
            [duration]()
            {
                std::this_thread::sleep_for(duration * 90);
            });

        timer2->set_duration(duration);

        timer2->start();
        std::this_thread::sleep_for(duration * 2);
        timer2->stop();

        timer2.reset();
    }

    worker->stop();
}

//-----------------------------------------------------------------------------

void worker_test::registry_test()
{
    {
        CPPUNIT_ASSERT_EQUAL(core::thread::worker::sptr(), core::thread::get_worker("worker1"));

        auto worker1 = core::thread::worker::make();
        core::thread::add_worker("worker1", worker1);

        CPPUNIT_ASSERT_EQUAL(worker1, core::thread::get_worker("worker1"));

        auto worker2 = core::thread::worker::make();
        core::thread::add_worker("worker2", worker2);

        auto worker3 = core::thread::worker::make();
        core::thread::add_worker("worker3", worker3);

        CPPUNIT_ASSERT_EQUAL(worker1, core::thread::get_worker("worker1"));
        CPPUNIT_ASSERT_EQUAL(worker2, core::thread::get_worker("worker2"));
        CPPUNIT_ASSERT_EQUAL(worker3, core::thread::get_worker("worker3"));

        core::thread::remove_worker("worker2");
        CPPUNIT_ASSERT_EQUAL(worker1, core::thread::get_worker("worker1"));
        CPPUNIT_ASSERT_EQUAL(core::thread::worker::sptr(), core::thread::get_worker("worker2"));
        CPPUNIT_ASSERT_EQUAL(worker3, core::thread::get_worker("worker3"));

        auto worker4 = core::thread::worker::make();
        core::thread::add_worker("worker2", worker4);
        CPPUNIT_ASSERT_EQUAL(worker1, core::thread::get_worker("worker1"));
        CPPUNIT_ASSERT_EQUAL(worker4, core::thread::get_worker("worker2"));
        CPPUNIT_ASSERT_EQUAL(worker3, core::thread::get_worker("worker3"));

        core::thread::remove_worker("worker1");
        core::thread::remove_worker("worker2");
        core::thread::remove_worker("worker3");
    }

    {
        auto init_worker = core::thread::get_default_worker();
        CPPUNIT_ASSERT(init_worker != nullptr);
    }
    {
        auto worker1 = core::thread::worker::make();
        core::thread::set_default_worker(worker1);
        CPPUNIT_ASSERT_EQUAL(worker1, core::thread::get_default_worker());

        auto worker2 = core::thread::worker::make();

        // simulates that worker1 is still in use because of its sptr above
        CPPUNIT_ASSERT_THROW(core::thread::set_default_worker(worker2), core::exception);
        worker2->stop();
    }
    {
        auto default_worker = core::thread::worker::make();
        core::thread::set_default_worker(default_worker);
        CPPUNIT_ASSERT_EQUAL(default_worker, core::thread::get_default_worker());
    }

    CPPUNIT_ASSERT_THROW(core::thread::set_default_worker(core::thread::worker::sptr()), core::exception);
}

//-----------------------------------------------------------------------------

} // namespace sight::core::thread::ut
