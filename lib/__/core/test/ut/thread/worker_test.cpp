/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/exception.hpp>
#include <core/thread/timer.hpp>
#include <core/thread/worker.hpp>

#include <utest/exception.hpp>

#include <doctest/doctest.h>

#include <atomic>
#include <thread>

//-----------------------------------------------------------------------------
namespace
{

struct test_handler
{
    test_handler() :
        m_constructor_thread_id(sight::core::thread::get_current_thread_id())
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
                            && (m_constructor_thread_id != sight::core::thread::get_current_thread_id())
                            && (m_worker_thread_id == sight::core::thread::get_current_thread_id());

        ++m_step;
    }

    //------------------------------------------------------------------------------

    void set_worker_id(sight::core::thread::thread_id_t _id)
    {
        m_worker_thread_id = _id;
    }

    std::atomic_int m_step {0};
    std::atomic_bool m_thread_check_ok {true};
    sight::core::thread::thread_id_t m_constructor_thread_id;
    sight::core::thread::thread_id_t m_worker_thread_id;
};

} // namespace

TEST_SUITE("sight::core::thread::worker")
{
    static sight::utest::exception e(""); // force link with fwTest

//-----------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        {
            auto worker = sight::core::thread::worker::make();

            test_handler handler;
            handler.set_worker_id(worker->get_thread_id());
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});

            worker->stop();
            CHECK_EQ(3, handler.m_step.load());
            CHECK_EQ(true, handler.m_thread_check_ok.load());
        }

        {
            auto worker = sight::core::thread::worker::make();

            test_handler handler;
            handler.set_worker_id(worker->get_thread_id());
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});

            worker->stop();
            CHECK_EQ(3, handler.m_step.load());
            CHECK_EQ(true, handler.m_thread_check_ok.load());
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("timer")
    {
        auto worker = sight::core::thread::worker::make();

        test_handler handler;
        handler.set_worker_id(worker->get_thread_id());

        auto timer = worker->create_timer();

        std::chrono::milliseconds duration = std::chrono::milliseconds(100);

        timer->set_function([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
        timer->set_duration(duration);

        CHECK(!timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_EQ(0, handler.m_step.load());

        timer->start();

        CHECK(timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_EQ(0, handler.m_step.load());

        std::this_thread::sleep_for(duration / 10.);

        for(int i = 1 ; i < 50 ; ++i)
        {
            std::this_thread::sleep_for(duration);

            CHECK(timer->is_running());
            CHECK(handler.m_thread_check_ok.load());
        }

        // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
        CHECK_GE(handler.m_step.load(), 45);
        CHECK_LE(handler.m_step.load(), 51);

        timer->stop();

        int last_step = handler.m_step.load();
        std::this_thread::sleep_for(duration * 3);

        CHECK(!timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_LE(handler.m_step.load(), last_step + 5);

        // test start after stop
        handler.m_step.store(0);

        timer->start();

        CHECK(timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_EQ(0, handler.m_step.load());

        std::this_thread::sleep_for(duration / 10.);

        for(int i = 1 ; i < 50 ; ++i)
        {
            std::this_thread::sleep_for(duration);
            CHECK(timer->is_running());
            CHECK(handler.m_thread_check_ok.load());
        }

        // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
        CHECK_GE(handler.m_step.load(), 40);
        CHECK_LE(handler.m_step.load(), 51);

        timer->stop();

        last_step = handler.m_step.load();
        std::this_thread::sleep_for(duration * 3);

        CHECK(!timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_LE(handler.m_step.load(), last_step + 5);

        // change timer duration on the fly
        // change timer duration
        handler.m_step.store(0);

        timer->start();

        CHECK(timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_EQ(0, handler.m_step.load());

        std::this_thread::sleep_for(duration / 10.);

        for(int i = 1 ; i < 25 ; ++i)
        {
            std::this_thread::sleep_for(duration);

            CHECK(timer->is_running());
            CHECK(handler.m_thread_check_ok.load());
        }

        // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
        CHECK_GE(handler.m_step.load(), 20);
        CHECK_LE(handler.m_step.load(), 30);

        duration = std::chrono::milliseconds(50);
        timer->set_duration(duration);

        for(int i = 24 ; i < 50 ; ++i)
        {
            std::this_thread::sleep_for(duration);

            CHECK(timer->is_running());
            CHECK(handler.m_thread_check_ok.load());
        }

        // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
        CHECK_GE(handler.m_step.load(), 40);
        CHECK_LE(handler.m_step.load(), 60);

        timer->stop();

        last_step = handler.m_step.load();
        std::this_thread::sleep_for(duration * 3);

        CHECK(!timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_LE(handler.m_step.load(), last_step + 5);

        // one shot test
        handler.m_step.store(0);

        duration = std::chrono::milliseconds(10);
        timer->set_duration(duration);
        timer->set_one_shot(true);

        timer->start();

        CHECK(timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_EQ(0, handler.m_step.load());

        std::this_thread::sleep_for(duration * 10);

        CHECK(!timer->is_running());
        CHECK(handler.m_thread_check_ok.load());
        CHECK_GE(handler.m_step.load(), 1);

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
            auto timer2 = worker->create_timer();
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

    TEST_CASE("registry")
    {
        {
            CHECK_EQ(sight::core::thread::worker::sptr(), sight::core::thread::get_worker("worker1"));

            auto worker1 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker1", worker1);

            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));

            auto worker2 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker2", worker2);

            auto worker3 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker3", worker3);

            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));
            CHECK_EQ(worker2, sight::core::thread::get_worker("worker2"));
            CHECK_EQ(worker3, sight::core::thread::get_worker("worker3"));

            sight::core::thread::remove_worker("worker2");
            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));
            CHECK_EQ(sight::core::thread::worker::sptr(), sight::core::thread::get_worker("worker2"));
            CHECK_EQ(worker3, sight::core::thread::get_worker("worker3"));

            auto worker4 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker2", worker4);
            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));
            CHECK_EQ(worker4, sight::core::thread::get_worker("worker2"));
            CHECK_EQ(worker3, sight::core::thread::get_worker("worker3"));

            sight::core::thread::remove_worker("worker1");
            sight::core::thread::remove_worker("worker2");
            sight::core::thread::remove_worker("worker3");
        }

        {
            auto init_worker = sight::core::thread::get_default_worker();
            CHECK(init_worker != nullptr);
        }
        {
            auto worker1 = sight::core::thread::worker::make();
            sight::core::thread::set_default_worker(worker1);
            CHECK_EQ(worker1, sight::core::thread::get_default_worker());

            auto worker2 = sight::core::thread::worker::make();

            // simulates that worker1 is still in use because of its sptr above
            CHECK_THROWS_AS(sight::core::thread::set_default_worker(worker2), sight::core::exception);
            worker2->stop();
        }
        {
            auto default_worker = sight::core::thread::worker::make();
            sight::core::thread::set_default_worker(default_worker);
            CHECK_EQ(default_worker, sight::core::thread::get_default_worker());
        }

        CHECK_THROWS_AS(
            sight::core::thread::set_default_worker(sight::core::thread::worker::sptr()),
            sight::core::exception
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("thread_name")
    {
        // Current thread (implicit api)
        {
            const auto thread_name     = sight::core::thread::get_thread_name();
            const auto new_thread_name = thread_name + "_pf0";
            sight::core::thread::set_thread_name(new_thread_name);
            const auto modified_thread_name = sight::core::thread::get_thread_name();
            CHECK_EQ(new_thread_name, modified_thread_name);

            sight::core::thread::set_thread_name(thread_name);
        }

        // Current thread (implicit api) and thread name too long
        {
            const auto thread_name = sight::core::thread::get_thread_name();

            const auto desired_thread_name  = std::string("desired_thread_name_too_long");
            const auto expected_thread_name = std::string("desired_thread_");

            sight::core::thread::set_thread_name(desired_thread_name);
            const auto current_thread_name = sight::core::thread::get_thread_name();
            CHECK_EQ(expected_thread_name, current_thread_name);

            sight::core::thread::set_thread_name(thread_name);
        }

        // Current thread (explicit api)
        {
            const auto current_native_id = sight::core::thread::get_current_thread_native_id();

            const auto thread_name     = sight::core::thread::get_thread_name(current_native_id);
            const auto new_thread_name = thread_name + "_pf1";
            sight::core::thread::set_thread_name(new_thread_name, current_native_id);
            const auto modified_thread_name = sight::core::thread::get_thread_name(current_native_id);
            CHECK_EQ(new_thread_name, modified_thread_name);
        }
    }
} // TEST_SUITE
