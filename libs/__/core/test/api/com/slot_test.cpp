/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "slot_test.hpp"

#include "core/com/exception/bad_call.hpp"
#include "core/com/exception/bad_run.hpp"
#include "core/com/exception/no_worker.hpp"
#include "core/com/util/auto_bind.hpp"
#include "core/com/util/auto_bind.hxx"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/mt/types.hpp>
#include <core/thread/worker.hpp>

#include <functional>
#include <future>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::slot_test);

namespace sight::core::com::ut
{

//------------------------------------------------------------------------------

void slot_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void slot_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

struct A
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

static int last_sum_result       = 0;
static int last_three_sum_result = 0;

//------------------------------------------------------------------------------

int sum(int _a, int _b)
{
    last_sum_result = _a + _b;
    return last_sum_result;
}

//------------------------------------------------------------------------------

int three_sum(int _a, int _b, int _c)
{
    last_three_sum_result = _a + _b + _c;
    return last_three_sum_result;
}

//------------------------------------------------------------------------------

void slot_test::build_test()
{
    A a;

    auto slot1 = core::com::new_slot(&sum);
    auto slot2 = core::com::new_slot(&A::method0, &a);
    auto slot3 = core::com::new_slot(&A::method1, &a);
    auto slot4 = core::com::new_slot(&three_sum);
    auto slot5 = core::com::new_slot(&A::method2, &a);

    auto fn    = [object_ptr = &a]{object_ptr->method2(4321);};
    auto slot6 = std::make_shared<slot<std::function<void(void)> > >(fn);

    CPPUNIT_ASSERT(slot1);
    CPPUNIT_ASSERT(slot2);
    CPPUNIT_ASSERT(slot3);
    CPPUNIT_ASSERT(slot4);
    CPPUNIT_ASSERT(slot5);
    CPPUNIT_ASSERT(slot6);

    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, slot1->arity());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 0, slot2->arity());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, slot3->arity());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 3, slot4->arity());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, slot5->arity());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 0, slot6->arity());

    core::com::slot_call<int(int, int)>::sptr slotcall1 =
        std::dynamic_pointer_cast<core::com::slot_call<int(int, int)> >(slot1);
    core::com::slot_call<void()>::sptr slotcall2 =
        std::dynamic_pointer_cast<core::com::slot_call<void()> >(slot2);
    core::com::slot_call<float(float)>::sptr slotcall3 =
        std::dynamic_pointer_cast<core::com::slot_call<float(float)> >(slot3);
    core::com::slot_call<int(int, int, int)>::sptr slotcall4 =
        std::dynamic_pointer_cast<core::com::slot_call<int(int, int, int)> >(slot4);

    CPPUNIT_ASSERT(slotcall1);
    CPPUNIT_ASSERT(slotcall2);
    CPPUNIT_ASSERT(slotcall3);
    CPPUNIT_ASSERT(slotcall4);

    core::com::slot_run<void(int, int)>::sptr slotrun1 =
        std::dynamic_pointer_cast<core::com::slot_run<void(int, int)> >(slot1);
    core::com::slot_run<void()>::sptr slotrun2 =
        std::dynamic_pointer_cast<core::com::slot_run<void()> >(slot2);
    core::com::slot_run<void(float)>::sptr slotrun3 =
        std::dynamic_pointer_cast<core::com::slot_run<void(float)> >(slot3);
    core::com::slot_run<void(int, int, int)>::sptr slotrun4 =
        std::dynamic_pointer_cast<core::com::slot_run<void(int, int, int)> >(slot4);

    CPPUNIT_ASSERT(slotrun1);
    CPPUNIT_ASSERT(slotrun2);
    CPPUNIT_ASSERT(slotrun3);
    CPPUNIT_ASSERT(slotrun4);

    core::com::slot_base::sptr slotbase1 = std::dynamic_pointer_cast<core::com::slot_base>(slot1);
    core::com::slot_base::sptr slotbase2 = std::dynamic_pointer_cast<core::com::slot_base>(slot2);
    core::com::slot_base::sptr slotbase3 = std::dynamic_pointer_cast<core::com::slot_base>(slot3);
    core::com::slot_base::sptr slotbase4 = std::dynamic_pointer_cast<core::com::slot_base>(slot4);

    CPPUNIT_ASSERT(slotbase1);
    CPPUNIT_ASSERT(slotbase2);
    CPPUNIT_ASSERT(slotbase3);
    CPPUNIT_ASSERT(slotbase4);
}

//------------------------------------------------------------------------------

void slot_test::run_test()
{
    A a;

    auto slot1 = core::com::new_slot(&sum);
    auto slot2 = core::com::new_slot(&A::method0, &a);
    auto slot3 = core::com::new_slot(&A::method1, &a);
    auto slot4 = core::com::new_slot(&three_sum);
    auto slot5 = core::com::new_slot(&A::method2, &a);
    auto slot6 = core::com::new_slot([&a](){a.method2(4321);});
    auto slot7 = core::com::new_slot([&a](int _x){a.method2(_x);});

    last_sum_result = 0;
    CPPUNIT_ASSERT(!a.m_method0);
    CPPUNIT_ASSERT(!a.m_method1);
    last_three_sum_result = 0;

    slot1->run(34, 66);
    slot2->run();
    slot3->run(88.1236F);
    slot4->run(40, 2, 3);
    slot5->run(1234);

    CPPUNIT_ASSERT_EQUAL(100, last_sum_result);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, last_three_sum_result);
    CPPUNIT_ASSERT_EQUAL(1234, a.m_method2);

    slot6->run();
    CPPUNIT_ASSERT_EQUAL(4321, a.m_method2);

    slot7->run(9876);
    CPPUNIT_ASSERT_EQUAL(9876, a.m_method2);
}

//-----------------------------------------------------------------------------

void slot_test::call_test()
{
    A a;

    auto slot1 = core::com::new_slot(&sum);
    auto slot2 = core::com::new_slot(&A::method0, &a);
    auto slot3 = core::com::new_slot(&A::method1, &a);
    auto slot4 = core::com::new_slot(&three_sum);
    auto slot5 = core::com::new_slot([&a](){a.method2(4321);});

    CPPUNIT_ASSERT_EQUAL(42, slot1->call(40, 2));
    slot2->call();
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT_EQUAL(4.2F, slot3->call(2.1F));
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, slot4->call(40, 2, 3));
    slot5->call();
    CPPUNIT_ASSERT_EQUAL(4321, a.m_method2);
}

//-----------------------------------------------------------------------------

void slot_test::async_test()
{
    A a;

    auto slot1 = core::com::new_slot(&sum);
    auto slot2 = core::com::new_slot(&A::method0, &a);
    auto slot3 = core::com::new_slot(&A::method1, &a);
    auto slot4 = core::com::new_slot(&three_sum);
    auto slot5 = core::com::new_slot([&a](){return a.method2(4321);});

    core::thread::worker::sptr w = core::thread::worker::make();

    slot1->set_worker(w);
    slot2->set_worker(w);
    slot4->set_worker(w);
    slot5->set_worker(w);

    slot1->async_run(40, 2).wait();
    slot2->async_run();
    slot3->async_run(w, 2.1F).wait();
    slot4->async_run(w, 40, 2, 3).wait();
    slot5->async_run().wait();

    CPPUNIT_ASSERT_EQUAL(42, last_sum_result);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, last_three_sum_result);

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
    CPPUNIT_ASSERT(f1.valid());
    CPPUNIT_ASSERT_EQUAL(42, f1.get());
    CPPUNIT_ASSERT(f1.valid());

    f2.wait();
    CPPUNIT_ASSERT(f2.valid());
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(f2.valid());

    f3.wait();
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT_EQUAL(4.2F, f3.get());
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT(a.m_method1);

    f4.wait();
    CPPUNIT_ASSERT(f4.valid());
    CPPUNIT_ASSERT_EQUAL(45, f4.get());
    CPPUNIT_ASSERT(f4.valid());

    f5.wait();
    CPPUNIT_ASSERT(f5.valid());
    CPPUNIT_ASSERT_EQUAL(4321, f5.get());
    CPPUNIT_ASSERT(f5.valid());
    CPPUNIT_ASSERT_EQUAL(4321, a.m_method2);

    w->stop();
}

//-----------------------------------------------------------------------------

void slot_test::slot_base_test()
{
    A a;

    core::com::slot_base::sptr slot1 = core::com::new_slot(&sum);
    core::com::slot_base::sptr slot2 = core::com::new_slot(&A::method0, &a);
    core::com::slot_base::sptr slot3 = core::com::new_slot(&A::method1, &a);
    core::com::slot_base::sptr slot4 = core::com::new_slot(&three_sum);
    core::com::slot_base::sptr slot5 = core::com::new_slot([&a](){a.method2(4321);});

    last_sum_result       = 0;
    last_three_sum_result = 0;

    slot1->run(40, 2);
    slot2->run();
    slot3->run(2.1F);
    slot4->run(40, 2, 3);
    slot5->run();

    CPPUNIT_ASSERT_EQUAL(42, last_sum_result);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, last_three_sum_result);
    CPPUNIT_ASSERT(a.m_method2);

    last_sum_result       = 0;
    a.m_method0           = false;
    a.m_method1           = false;
    last_three_sum_result = 0;

    CPPUNIT_ASSERT_EQUAL(42, slot1->call<int>(40, 2));
    slot2->call<void>();
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT_EQUAL(4.2F, slot3->call<float>(2.1F));
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, slot4->call<int>(40, 2, 3));

    last_sum_result       = 0;
    a.m_method0           = false;
    a.m_method1           = false;
    last_three_sum_result = 0;

    core::thread::worker::sptr w = core::thread::worker::make();

    slot1->set_worker(w);
    slot2->set_worker(w);
    slot3->set_worker(w);
    slot4->set_worker(w);

    slot1->async_run(40, 2).wait();
    slot2->async_run();
    slot3->async_run(2.1F).wait();
    slot4->async_run(40, 2, 3).wait();

    CPPUNIT_ASSERT_EQUAL(42, last_sum_result);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, last_three_sum_result);

    last_sum_result       = 0;
    a.m_method0           = false;
    a.m_method1           = false;
    last_three_sum_result = 0;

    std::shared_future<int> f1   = slot1->async_call<int>(40, 2);
    std::shared_future<void> f2  = slot2->async_call<void>();
    std::shared_future<float> f3 = slot3->async_call<float>(2.1F);
    std::shared_future<int> f4   = slot4->async_call<int>(40, 2, 3);

    f1.wait();
    CPPUNIT_ASSERT(f1.valid());
    CPPUNIT_ASSERT_EQUAL(42, f1.get());
    CPPUNIT_ASSERT(f1.valid());

    f2.wait();
    CPPUNIT_ASSERT(f2.valid());
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(f2.valid());

    f3.wait();
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT_EQUAL(4.2F, f3.get());
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT(a.m_method1);

    f4.wait();
    CPPUNIT_ASSERT(f4.valid());
    CPPUNIT_ASSERT_EQUAL(45, f4.get());
    CPPUNIT_ASSERT(f4.valid());

    w->stop();
}

//-----------------------------------------------------------------------------

void slot_test::exception_test()
{
    core::com::slot_base::sptr slot = core::com::new_slot(&sum);

    CPPUNIT_ASSERT_THROW(slot->run(), core::com::exception::bad_run);
    CPPUNIT_ASSERT_THROW(slot->call<int>(), core::com::exception::bad_call);
    CPPUNIT_ASSERT_THROW(slot->call<void>(3, 4), core::com::exception::bad_call);
    CPPUNIT_ASSERT_THROW(slot->async_run(), core::com::exception::bad_run);
    CPPUNIT_ASSERT_THROW(slot->async_call<int>(), core::com::exception::bad_call);
    CPPUNIT_ASSERT_THROW(slot->async_run(5, 6), core::com::exception::no_worker);
    CPPUNIT_ASSERT_THROW(slot->async_call<int>(4, 5), core::com::exception::no_worker);
}

//-----------------------------------------------------------------------------

struct B
{
    B()
    = default;

    //------------------------------------------------------------------------------

    std::thread::id wait_seconds(const unsigned int _nb_seconds)
    {
        core::mt::write_lock lock(m_mutex);
        std::thread::id old_id = m_thread_id;
        m_thread_id = std::this_thread::get_id();
        m_first_run = false;

        std::this_thread::sleep_for(std::chrono::seconds(_nb_seconds));

        return old_id;
    }

    std::thread::id m_thread_id;

    bool m_first_run {true};

    core::mt::read_write_mutex m_mutex;
};

//------------------------------------------------------------------------------

void slot_test::worker_swap_test()
{
    // Tests if weak call gets interrupted when slot worker is changed while
    // calls are pending.
    bool exception_thrown = false;
    while(!exception_thrown)
    {
        using signature = std::thread::id(const unsigned int);

        B b;

        core::thread::worker::sptr w1 = core::thread::worker::make();
        core::thread::worker::sptr w2 = core::thread::worker::make();

        core::com::slot<signature>::sptr m0 = core::com::new_slot(&B::wait_seconds, &b);

        CPPUNIT_ASSERT(b.m_thread_id == std::thread::id());

        m0->set_worker(w1);
        core::com::slot<signature>::void_shared_future_type future1 = m0->async_run(1);
        m0->set_worker(w2);

        {
            core::mt::read_lock lock(b.m_mutex);
            if(b.m_thread_id == std::thread::id())
            {
                exception_thrown = true;
                CPPUNIT_ASSERT_THROW(future1.get(), core::com::exception::worker_changed);
            }
            else
            {
                CPPUNIT_ASSERT(b.m_thread_id == w1->get_thread_id());
            }
        }
        w1->stop();
        w2->stop();
    }

    //Tests weakcalls to hold slot worker while running weakcall (async_run test)
    {
        using signature = std::thread::id(const unsigned int);

        B b;

        core::thread::worker::sptr w1 = core::thread::worker::make();
        core::thread::worker::sptr w2 = core::thread::worker::make();

        core::com::slot<signature>::sptr m0 = core::com::new_slot(&B::wait_seconds, &b);

        CPPUNIT_ASSERT(b.m_thread_id == std::thread::id());

        m0->set_worker(w1);
        core::com::slot<signature>::void_shared_future_type future1 = m0->async_run(1);

        // wait until we entered in slot's function
        while(b.m_first_run)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        m0->set_worker(w2);
        future1.wait();
        CPPUNIT_ASSERT(b.m_thread_id == w1->get_thread_id());

        core::com::slot<signature>::void_shared_future_type future2 = m0->async_run(1);

        future2.wait();

        CPPUNIT_ASSERT(b.m_thread_id == w2->get_thread_id());
        w1->stop();
        w2->stop();
    }

    //Tests weakcalls to hold slot worker while running weakcall (asyncCall test)
    {
        using signature = std::thread::id(const unsigned int);

        B b;

        core::thread::worker::sptr w1 = core::thread::worker::make();
        core::thread::worker::sptr w2 = core::thread::worker::make();

        core::com::slot<signature>::sptr m0 = core::com::new_slot(&B::wait_seconds, &b);

        CPPUNIT_ASSERT(b.m_thread_id == std::thread::id());

        m0->set_worker(w1);
        core::com::slot<signature>::shared_future_type future1 = m0->async_call(1);

        // wait until we entered in slot's function
        while(b.m_first_run)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        m0->set_worker(w2);
        future1.wait();
        CPPUNIT_ASSERT(b.m_thread_id == w1->get_thread_id());

        core::com::slot<signature>::shared_future_type future2 = m0->async_call(1);

        future2.wait();

        CPPUNIT_ASSERT(b.m_thread_id == w2->get_thread_id());
        CPPUNIT_ASSERT(future1.get() == std::thread::id());
        CPPUNIT_ASSERT(future2.get() == w1->get_thread_id());
        w1->stop();
        w2->stop();
    }
}

//------------------------------------------------------------------------------

void slot_test::sloppiness_test()
{
    // Tests whether fallback when calling a slot with too many arguments works
    // correctly.
    A a;
    last_sum_result = 0;

    core::com::slot_base::sptr slot1 = core::com::new_slot(&sum);
    core::com::slot_base::sptr slot2 = core::com::new_slot(&A::method0, &a);
    core::com::slot_base::sptr slot3 = core::com::new_slot(&A::method1, &a);

    slot1->run(40, 2, 3);
    slot2->run("Hello world");
    slot3->run(2.1F, 4.2F);

    CPPUNIT_ASSERT_EQUAL(42, last_sum_result);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);

    a.m_method0 = false;
    a.m_method1 = false;

    CPPUNIT_ASSERT_EQUAL(42, slot1->call<int>(40, 2, 3));
    slot2->call<void>("Hello world");
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT_EQUAL(4.2F, slot3->call<float>(2.1F, 4.2F));
    CPPUNIT_ASSERT(a.m_method1);

    last_sum_result = 0;
    a.m_method0     = false;
    a.m_method1     = false;

    core::thread::worker::sptr w = core::thread::worker::make();

    slot1->set_worker(w);
    slot2->set_worker(w);
    slot3->set_worker(w);

    slot1->async_run(40, 2, 3).wait();
    slot2->async_run("Hello world");
    slot3->async_run(2.1F, 4.2F).wait();

    CPPUNIT_ASSERT_EQUAL(42, last_sum_result);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);

    last_sum_result = 0;
    a.m_method0     = false;
    a.m_method1     = false;

    std::shared_future<int> f1   = slot1->async_call<int>(40, 2, 3);
    std::shared_future<void> f2  = slot2->async_call<void>("Hello world");
    std::shared_future<float> f3 = slot3->async_call<float>(2.1F, 4.2F);

    f1.wait();
    CPPUNIT_ASSERT(f1.valid());
    CPPUNIT_ASSERT_EQUAL(42, f1.get());
    CPPUNIT_ASSERT(f1.valid());

    f2.wait();
    CPPUNIT_ASSERT(f2.valid());
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(f2.valid());

    f3.wait();
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT_EQUAL(4.2F, f3.get());
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT(a.m_method1);

    w->stop();
}

//------------------------------------------------------------------------------

void slot_test::no_worker_test()
{
    // Tests whether the good exception is thrown when asynchronously calling a
    // slot without worker
    core::com::slot<int(int, int)>::sptr slot = core::com::new_slot(&sum);

    CPPUNIT_ASSERT_THROW(slot->async_run(5, 6), core::com::exception::no_worker);
    CPPUNIT_ASSERT_THROW(slot->async_call(4, 5), core::com::exception::no_worker);

    core::thread::worker::sptr w = core::thread::worker::make();
    slot->set_worker(w);

    CPPUNIT_ASSERT_THROW(slot->async_run(nullptr, 5, 6), core::com::exception::no_worker);
    CPPUNIT_ASSERT_THROW(slot->async_call(nullptr, 4, 5), core::com::exception::no_worker);

    w->stop();
}

} // namespace sight::core::com::ut
