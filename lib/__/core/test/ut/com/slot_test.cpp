/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/com/exception/bad_call.hpp"
#include "core/com/exception/bad_run.hpp"
#include "core/com/exception/no_worker.hpp"

#include <core/com/slot.hpp>

#include <core/mt/types.hpp>
#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <functional>
#include <future>
#include <thread>

namespace
{

struct test_a
{
    //------------------------------------------------------------------------------

    void method0()
    {
        m_method0 = true;
    }

    //------------------------------------------------------------------------------

    float method1(float _f)
    {
        m_method1 = true;
        return 2 * _f;
    }

    //------------------------------------------------------------------------------

    int method2(int _value)
    {
        m_method2 = _value;
        return m_method2;
    }

    bool m_method0 {false};
    bool m_method1 {false};
    int m_method2 {0};
};

struct test_b
{
    //------------------------------------------------------------------------------

    std::thread::id wait_seconds(const unsigned int _nb_seconds)
    {
        sight::core::mt::write_lock lock(m_mutex);
        std::thread::id old_id = m_thread_id;
        m_thread_id = std::this_thread::get_id();
        m_first_run = false;

        std::this_thread::sleep_for(std::chrono::seconds(_nb_seconds));

        return old_id;
    }

    std::thread::id m_thread_id;

    bool m_first_run {true};

    sight::core::mt::read_write_mutex m_mutex;
};

} // namespace

//-----------------------------------------------------------------------------

static int last_sum_result       = 0;
static int last_three_sum_result = 0;

//------------------------------------------------------------------------------

static int sum(int _a, int _b)
{
    last_sum_result = _a + _b;
    return last_sum_result;
}

//------------------------------------------------------------------------------

static int three_sum(int _a, int _b, int _c)
{
    last_three_sum_result = _a + _b + _c;
    return last_three_sum_result;
}

TEST_SUITE("sight::core::com::slot")
{
//------------------------------------------------------------------------------

    TEST_CASE("build")
    {
        test_a a;

        auto slot1 = sight::core::com::new_slot(&sum);
        auto slot2 = sight::core::com::new_slot(&test_a::method0, &a);
        auto slot3 = sight::core::com::new_slot(&test_a::method1, &a);
        auto slot4 = sight::core::com::new_slot(&three_sum);
        auto slot5 = sight::core::com::new_slot(&test_a::method2, &a);

        auto fn    = [object_ptr = &a]{object_ptr->method2(4321);};
        auto slot6 = std::make_shared<sight::core::com::slot<std::function<void(void)> > >(fn);

        CHECK(slot1);
        CHECK(slot2);
        CHECK(slot3);
        CHECK(slot4);
        CHECK(slot5);
        CHECK(slot6);

        CHECK_EQ(slot1->arity(), 2);
        CHECK_EQ(slot2->arity(), 0);
        CHECK_EQ(slot3->arity(), 1);
        CHECK_EQ(slot4->arity(), 3);
        CHECK_EQ(slot5->arity(), 1);
        CHECK_EQ(slot6->arity(), 0);

        auto slotcall1 = std::dynamic_pointer_cast<sight::core::com::slot_call<int(int, int)> >(slot1);
        auto slotcall2 = std::dynamic_pointer_cast<sight::core::com::slot_call<void()> >(slot2);
        auto slotcall3 = std::dynamic_pointer_cast<sight::core::com::slot_call<float(float)> >(slot3);
        auto slotcall4 = std::dynamic_pointer_cast<sight::core::com::slot_call<int(int, int, int)> >(slot4);

        CHECK(slotcall1);
        CHECK(slotcall2);
        CHECK(slotcall3);
        CHECK(slotcall4);

        auto slotrun1 = std::dynamic_pointer_cast<sight::core::com::slot_run<void(int, int)> >(slot1);
        auto slotrun2 = std::dynamic_pointer_cast<sight::core::com::slot_run<void()> >(slot2);
        auto slotrun3 = std::dynamic_pointer_cast<sight::core::com::slot_run<void(float)> >(slot3);
        auto slotrun4 = std::dynamic_pointer_cast<sight::core::com::slot_run<void(int, int, int)> >(slot4);

        CHECK(slotrun1);
        CHECK(slotrun2);
        CHECK(slotrun3);
        CHECK(slotrun4);

        auto slotbase1 = std::dynamic_pointer_cast<sight::core::com::slot_base>(slot1);
        auto slotbase2 = std::dynamic_pointer_cast<sight::core::com::slot_base>(slot2);
        auto slotbase3 = std::dynamic_pointer_cast<sight::core::com::slot_base>(slot3);
        auto slotbase4 = std::dynamic_pointer_cast<sight::core::com::slot_base>(slot4);

        CHECK(slotbase1);
        CHECK(slotbase2);
        CHECK(slotbase3);
        CHECK(slotbase4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("run")
    {
        test_a a;

        auto slot1 = sight::core::com::new_slot(&sum);
        auto slot2 = sight::core::com::new_slot(&test_a::method0, &a);
        auto slot3 = sight::core::com::new_slot(&test_a::method1, &a);
        auto slot4 = sight::core::com::new_slot(&three_sum);
        auto slot5 = sight::core::com::new_slot(&test_a::method2, &a);
        auto slot6 = sight::core::com::new_slot([&a](){a.method2(4321);});
        auto slot7 = sight::core::com::new_slot([&a](int _x){a.method2(_x);});

        last_sum_result = 0;
        CHECK(!a.m_method0);
        CHECK(!a.m_method1);
        last_three_sum_result = 0;

        slot1->run(34, 66);
        slot2->run();
        slot3->run(88.1236F);
        slot4->run(40, 2, 3);
        slot5->run(1234);

        CHECK_EQ(last_sum_result, 100);
        CHECK(a.m_method0);
        CHECK(a.m_method1);
        CHECK_EQ(last_three_sum_result, 45);
        CHECK_EQ(a.m_method2, 1234);

        slot6->run();
        CHECK_EQ(a.m_method2, 4321);

        slot7->run(9876);
        CHECK_EQ(a.m_method2, 9876);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("call")
    {
        test_a a;

        auto slot1 = sight::core::com::new_slot(&sum);
        auto slot2 = sight::core::com::new_slot(&test_a::method0, &a);
        auto slot3 = sight::core::com::new_slot(&test_a::method1, &a);
        auto slot4 = sight::core::com::new_slot(&three_sum);
        auto slot5 = sight::core::com::new_slot([&a](){a.method2(4321);});

        CHECK_EQ(slot1->call(40, 2), 42);
        slot2->call();
        CHECK(a.m_method0);
        CHECK_EQ(slot3->call(2.1F), 4.2F);
        CHECK(a.m_method1);
        CHECK_EQ(slot4->call(40, 2, 3), 45);
        slot5->call();
        CHECK_EQ(a.m_method2, 4321);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("async")
    {
        test_a a;

        auto slot1 = sight::core::com::new_slot(&sum);
        auto slot2 = sight::core::com::new_slot(&test_a::method0, &a);
        auto slot3 = sight::core::com::new_slot(&test_a::method1, &a);
        auto slot4 = sight::core::com::new_slot(&three_sum);
        auto slot5 = sight::core::com::new_slot([&a](){return a.method2(4321);});

        auto w = sight::core::thread::worker::make();

        slot1->set_worker(w);
        slot2->set_worker(w);
        slot4->set_worker(w);
        slot5->set_worker(w);

        slot1->async_run(40, 2).wait();
        slot2->async_run();
        slot3->async_run(w, 2.1F).wait();
        slot4->async_run(w, 40, 2, 3).wait();
        slot5->async_run().wait();

        CHECK_EQ(last_sum_result, 42);
        CHECK(a.m_method0);
        CHECK(a.m_method1);
        CHECK_EQ(last_three_sum_result, 45);

        last_sum_result       = 0;
        a.m_method0           = false;
        a.m_method1           = false;
        last_three_sum_result = 0;

        std::shared_future<int> f1   = slot1->async_call(40, 2);
        std::shared_future<void> f2  = slot2->async_call();
        std::shared_future<float> f3 = slot3->async_call(w, 2.1F);
        std::shared_future<int> f4   = slot4->async_call(w, 40, 2, 3);
        std::shared_future<int> f5   = slot5->async_call();

        f1.wait();
        CHECK(f1.valid());
        CHECK_EQ(f1.get(), 42);
        CHECK(f1.valid());

        f2.wait();
        CHECK(f2.valid());
        CHECK(a.m_method0);
        CHECK(f2.valid());

        f3.wait();
        CHECK(f3.valid());
        CHECK_EQ(f3.get(), 4.2F);
        CHECK(f3.valid());
        CHECK(a.m_method1);

        f4.wait();
        CHECK(f4.valid());
        CHECK_EQ(f4.get(), 45);
        CHECK(f4.valid());

        f5.wait();
        CHECK(f5.valid());
        CHECK_EQ(f5.get(), 4321);
        CHECK(f5.valid());
        CHECK_EQ(a.m_method2, 4321);

        w->stop();
    }

//-----------------------------------------------------------------------------

    TEST_CASE("slot_base")
    {
        test_a a;

        sight::core::com::slot_base::sptr slot1 = sight::core::com::new_slot(&sum);
        sight::core::com::slot_base::sptr slot2 = sight::core::com::new_slot(&test_a::method0, &a);
        sight::core::com::slot_base::sptr slot3 = sight::core::com::new_slot(&test_a::method1, &a);
        sight::core::com::slot_base::sptr slot4 = sight::core::com::new_slot(&three_sum);
        sight::core::com::slot_base::sptr slot5 = sight::core::com::new_slot([&a](){a.method2(4321);});

        last_sum_result       = 0;
        last_three_sum_result = 0;

        slot1->run(40, 2);
        slot2->run();
        slot3->run(2.1F);
        slot4->run(40, 2, 3);
        slot5->run();

        CHECK_EQ(last_sum_result, 42);
        CHECK(a.m_method0);
        CHECK(a.m_method1);
        CHECK_EQ(last_three_sum_result, 45);
        CHECK(a.m_method2);

        last_sum_result       = 0;
        a.m_method0           = false;
        a.m_method1           = false;
        last_three_sum_result = 0;

        CHECK_EQ(slot1->call<int>(40, 2), 42);
        slot2->call<void>();
        CHECK(a.m_method0);
        CHECK_EQ(slot3->call<float>(2.1F), 4.2F);
        CHECK(a.m_method1);
        CHECK_EQ(slot4->call<int>(40, 2, 3), 45);

        last_sum_result       = 0;
        a.m_method0           = false;
        a.m_method1           = false;
        last_three_sum_result = 0;

        auto w = sight::core::thread::worker::make();

        slot1->set_worker(w);
        slot2->set_worker(w);
        slot3->set_worker(w);
        slot4->set_worker(w);

        slot1->async_run(40, 2).wait();
        slot2->async_run();
        slot3->async_run(2.1F).wait();
        slot4->async_run(40, 2, 3).wait();

        CHECK_EQ(last_sum_result, 42);
        CHECK(a.m_method0);
        CHECK(a.m_method1);
        CHECK_EQ(last_three_sum_result, 45);

        last_sum_result       = 0;
        a.m_method0           = false;
        a.m_method1           = false;
        last_three_sum_result = 0;

        std::shared_future<int> f1   = slot1->async_call<int>(40, 2);
        std::shared_future<void> f2  = slot2->async_call<void>();
        std::shared_future<float> f3 = slot3->async_call<float>(2.1F);
        std::shared_future<int> f4   = slot4->async_call<int>(40, 2, 3);

        f1.wait();
        CHECK(f1.valid());
        CHECK_EQ(f1.get(), 42);
        CHECK(f1.valid());

        f2.wait();
        CHECK(f2.valid());
        CHECK(a.m_method0);
        CHECK(f2.valid());

        f3.wait();
        CHECK(f3.valid());
        CHECK_EQ(f3.get(), 4.2F);
        CHECK(f3.valid());
        CHECK(a.m_method1);

        f4.wait();
        CHECK(f4.valid());
        CHECK_EQ(f4.get(), 45);
        CHECK(f4.valid());

        w->stop();
    }

//-----------------------------------------------------------------------------

    TEST_CASE("exception")
    {
        sight::core::com::slot_base::sptr slot = sight::core::com::new_slot(&sum);

        CHECK_THROWS_AS(slot->run(), sight::core::com::exception::bad_run);
        CHECK_THROWS_AS(slot->call<int>(), sight::core::com::exception::bad_call);
        CHECK_THROWS_AS(slot->call<void>(3, 4), sight::core::com::exception::bad_call);
        CHECK_THROWS_AS(slot->async_run(), sight::core::com::exception::bad_run);
        CHECK_THROWS_AS(slot->async_call<int>(), sight::core::com::exception::bad_call);
        CHECK_THROWS_AS(slot->async_run(5, 6), sight::core::com::exception::no_worker);
        CHECK_THROWS_AS(slot->async_call<int>(4, 5), sight::core::com::exception::no_worker);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("worker_swap")
    {
        // Tests if weak call gets interrupted when slot worker is changed while
        // calls are pending.
        bool exception_thrown = false;
        while(!exception_thrown)
        {
            using signature = std::thread::id(const unsigned int);

            test_b b;

            auto w1 = sight::core::thread::worker::make();
            auto w2 = sight::core::thread::worker::make();

            sight::core::com::slot<signature>::sptr m0 = sight::core::com::new_slot(&test_b::wait_seconds, &b);

            CHECK(b.m_thread_id == std::thread::id());

            m0->set_worker(w1);
            sight::core::com::slot<signature>::void_shared_future_type future1 = m0->async_run(1);
            m0->set_worker(w2);

            {
                sight::core::mt::read_lock lock(b.m_mutex);
                if(b.m_thread_id == std::thread::id())
                {
                    exception_thrown = true;
                    CHECK_THROWS_AS(future1.get(), sight::core::com::exception::worker_changed);
                }
                else
                {
                    CHECK(b.m_thread_id == w1->get_thread_id());
                }
            }
            w1->stop();
            w2->stop();
        }

        //Tests weakcalls to hold slot worker while running weakcall (async_run test)
        {
            using signature = std::thread::id(const unsigned int);

            test_b b;

            auto w1 = sight::core::thread::worker::make();
            auto w2 = sight::core::thread::worker::make();

            sight::core::com::slot<signature>::sptr m0 = sight::core::com::new_slot(&test_b::wait_seconds, &b);

            CHECK(b.m_thread_id == std::thread::id());

            m0->set_worker(w1);
            sight::core::com::slot<signature>::void_shared_future_type future1 = m0->async_run(1);

            // wait until we entered in slot's function
            while(b.m_first_run)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            m0->set_worker(w2);
            future1.wait();
            CHECK(b.m_thread_id == w1->get_thread_id());

            sight::core::com::slot<signature>::void_shared_future_type future2 = m0->async_run(1);

            future2.wait();

            CHECK(b.m_thread_id == w2->get_thread_id());
            w1->stop();
            w2->stop();
        }

        //Tests weakcalls to hold slot worker while running weakcall (asyncCall test)
        {
            using signature = std::thread::id(const unsigned int);

            test_b b;

            auto w1 = sight::core::thread::worker::make();
            auto w2 = sight::core::thread::worker::make();

            sight::core::com::slot<signature>::sptr m0 = sight::core::com::new_slot(&test_b::wait_seconds, &b);

            CHECK(b.m_thread_id == std::thread::id());

            m0->set_worker(w1);
            sight::core::com::slot<signature>::shared_future_type future1 = m0->async_call(1);

            // wait until we entered in slot's function
            while(b.m_first_run)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            m0->set_worker(w2);
            future1.wait();
            CHECK(b.m_thread_id == w1->get_thread_id());

            sight::core::com::slot<signature>::shared_future_type future2 = m0->async_call(1);

            future2.wait();

            CHECK(b.m_thread_id == w2->get_thread_id());
            CHECK(future1.get() == std::thread::id());
            CHECK(future2.get() == w1->get_thread_id());
            w1->stop();
            w2->stop();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("sloppiness")
    {
        // Tests whether fallback when calling a slot with too many arguments works
        // correctly.
        test_a a;
        last_sum_result = 0;

        sight::core::com::slot_base::sptr slot1 = sight::core::com::new_slot(&sum);
        sight::core::com::slot_base::sptr slot2 = sight::core::com::new_slot(&test_a::method0, &a);
        sight::core::com::slot_base::sptr slot3 = sight::core::com::new_slot(&test_a::method1, &a);

        slot1->run(40, 2, 3);
        slot2->run("Hello world");
        slot3->run(2.1F, 4.2F);

        CHECK_EQ(last_sum_result, 42);
        CHECK(a.m_method0);
        CHECK(a.m_method1);

        a.m_method0 = false;
        a.m_method1 = false;

        CHECK_EQ(slot1->call<int>(40, 2, 3), 42);
        slot2->call<void>("Hello world");
        CHECK(a.m_method0);
        CHECK_EQ(slot3->call<float>(2.1F, 4.2F), 4.2F);
        CHECK(a.m_method1);

        last_sum_result = 0;
        a.m_method0     = false;
        a.m_method1     = false;

        auto w = sight::core::thread::worker::make();

        slot1->set_worker(w);
        slot2->set_worker(w);
        slot3->set_worker(w);

        slot1->async_run(40, 2, 3).wait();
        slot2->async_run("Hello world");
        slot3->async_run(2.1F, 4.2F).wait();

        CHECK_EQ(last_sum_result, 42);
        CHECK(a.m_method0);
        CHECK(a.m_method1);

        last_sum_result = 0;
        a.m_method0     = false;
        a.m_method1     = false;

        std::shared_future<int> f1   = slot1->async_call<int>(40, 2, 3);
        std::shared_future<void> f2  = slot2->async_call<void>("Hello world");
        std::shared_future<float> f3 = slot3->async_call<float>(2.1F, 4.2F);

        f1.wait();
        CHECK(f1.valid());
        CHECK_EQ(f1.get(), 42);
        CHECK(f1.valid());

        f2.wait();
        CHECK(f2.valid());
        CHECK(a.m_method0);
        CHECK(f2.valid());

        f3.wait();
        CHECK(f3.valid());
        CHECK_EQ(f3.get(), 4.2F);
        CHECK(f3.valid());
        CHECK(a.m_method1);

        w->stop();
    }

//------------------------------------------------------------------------------

    TEST_CASE("no_worker")
    {
        // Tests whether the good exception is thrown when asynchronously calling a
        // slot without worker
        sight::core::com::slot<int(int, int)>::sptr slot = sight::core::com::new_slot(&sum);

        CHECK_THROWS_AS(slot->async_run(5, 6), sight::core::com::exception::no_worker);
        CHECK_THROWS_AS(slot->async_call(4, 5), sight::core::com::exception::no_worker);

        auto w = sight::core::thread::worker::make();
        slot->set_worker(w);

        CHECK_THROWS_AS(slot->async_run(nullptr, 5, 6), sight::core::com::exception::no_worker);
        CHECK_THROWS_AS(slot->async_call(nullptr, 4, 5), sight::core::com::exception::no_worker);

        w->stop();
    }
} // TEST_SUITE
