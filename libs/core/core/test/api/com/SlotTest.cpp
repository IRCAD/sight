/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SlotTest.hpp"

#include "core/com/exception/BadCall.hpp"
#include "core/com/exception/BadRun.hpp"
#include "core/com/exception/NoWorker.hpp"
#include "core/com/util/AutoBind.hpp"
#include "core/com/util/AutoBind.hxx"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/mt/types.hpp>
#include <core/thread/Worker.hpp>

#include <functional>
#include <future>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::SlotTest);

namespace sight::core::com::ut
{

//------------------------------------------------------------------------------

void SlotTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SlotTest::tearDown()
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

    float method1(float f)
    {
        m_method1 = true;
        return 2 * f;
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

static int lastSumResult      = 0;
static int lastThreeSumResult = 0;

//------------------------------------------------------------------------------

int sum(int a, int b)
{
    lastSumResult = a + b;
    return lastSumResult;
}

//------------------------------------------------------------------------------

int threeSum(int a, int b, int c)
{
    lastThreeSumResult = a + b + c;
    return lastThreeSumResult;
}

//------------------------------------------------------------------------------

void SlotTest::buildTest()
{
    A a;

    auto slot1 = core::com::newSlot(&sum);
    auto slot2 = core::com::newSlot(&A::method0, &a);
    auto slot3 = core::com::newSlot(&A::method1, &a);
    auto slot4 = core::com::newSlot(&threeSum);
    auto slot5 = core::com::newSlot(&A::method2, &a);

    auto fn    = [ObjectPtr = &a]{ObjectPtr->method2(4321);};
    auto slot6 = std::make_shared<Slot<std::function<void(void)> > >(fn);

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

    core::com::SlotCall<int(int, int)>::sptr slotcall1 =
        std::dynamic_pointer_cast<core::com::SlotCall<int(int, int)> >(slot1);
    core::com::SlotCall<void()>::sptr slotcall2 =
        std::dynamic_pointer_cast<core::com::SlotCall<void()> >(slot2);
    core::com::SlotCall<float(float)>::sptr slotcall3 =
        std::dynamic_pointer_cast<core::com::SlotCall<float(float)> >(slot3);
    core::com::SlotCall<int(int, int, int)>::sptr slotcall4 =
        std::dynamic_pointer_cast<core::com::SlotCall<int(int, int, int)> >(slot4);

    CPPUNIT_ASSERT(slotcall1);
    CPPUNIT_ASSERT(slotcall2);
    CPPUNIT_ASSERT(slotcall3);
    CPPUNIT_ASSERT(slotcall4);

    core::com::SlotRun<void(int, int)>::sptr slotrun1 =
        std::dynamic_pointer_cast<core::com::SlotRun<void(int, int)> >(slot1);
    core::com::SlotRun<void()>::sptr slotrun2 =
        std::dynamic_pointer_cast<core::com::SlotRun<void()> >(slot2);
    core::com::SlotRun<void(float)>::sptr slotrun3 =
        std::dynamic_pointer_cast<core::com::SlotRun<void(float)> >(slot3);
    core::com::SlotRun<void(int, int, int)>::sptr slotrun4 =
        std::dynamic_pointer_cast<core::com::SlotRun<void(int, int, int)> >(slot4);

    CPPUNIT_ASSERT(slotrun1);
    CPPUNIT_ASSERT(slotrun2);
    CPPUNIT_ASSERT(slotrun3);
    CPPUNIT_ASSERT(slotrun4);

    core::com::SlotBase::sptr slotbase1 = std::dynamic_pointer_cast<core::com::SlotBase>(slot1);
    core::com::SlotBase::sptr slotbase2 = std::dynamic_pointer_cast<core::com::SlotBase>(slot2);
    core::com::SlotBase::sptr slotbase3 = std::dynamic_pointer_cast<core::com::SlotBase>(slot3);
    core::com::SlotBase::sptr slotbase4 = std::dynamic_pointer_cast<core::com::SlotBase>(slot4);

    CPPUNIT_ASSERT(slotbase1);
    CPPUNIT_ASSERT(slotbase2);
    CPPUNIT_ASSERT(slotbase3);
    CPPUNIT_ASSERT(slotbase4);
}

//------------------------------------------------------------------------------

void SlotTest::runTest()
{
    A a;

    auto slot1 = core::com::newSlot(&sum);
    auto slot2 = core::com::newSlot(&A::method0, &a);
    auto slot3 = core::com::newSlot(&A::method1, &a);
    auto slot4 = core::com::newSlot(&threeSum);
    auto slot5 = core::com::newSlot(&A::method2, &a);
    auto slot6 = core::com::newSlot([&a](){a.method2(4321);});
    auto slot7 = core::com::newSlot([&a](int x){a.method2(x);});

    lastSumResult = 0;
    CPPUNIT_ASSERT(!a.m_method0);
    CPPUNIT_ASSERT(!a.m_method1);
    lastThreeSumResult = 0;

    slot1->run(34, 66);
    slot2->run();
    slot3->run(88.1236F);
    slot4->run(40, 2, 3);
    slot5->run(1234);

    CPPUNIT_ASSERT_EQUAL(100, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, lastThreeSumResult);
    CPPUNIT_ASSERT_EQUAL(1234, a.m_method2);

    slot6->run();
    CPPUNIT_ASSERT_EQUAL(4321, a.m_method2);

    slot7->run(9876);
    CPPUNIT_ASSERT_EQUAL(9876, a.m_method2);
}

//-----------------------------------------------------------------------------

void SlotTest::callTest()
{
    A a;

    auto slot1 = core::com::newSlot(&sum);
    auto slot2 = core::com::newSlot(&A::method0, &a);
    auto slot3 = core::com::newSlot(&A::method1, &a);
    auto slot4 = core::com::newSlot(&threeSum);
    auto slot5 = core::com::newSlot([&a](){a.method2(4321);});

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

void SlotTest::asyncTest()
{
    A a;

    auto slot1 = core::com::newSlot(&sum);
    auto slot2 = core::com::newSlot(&A::method0, &a);
    auto slot3 = core::com::newSlot(&A::method1, &a);
    auto slot4 = core::com::newSlot(&threeSum);
    auto slot5 = core::com::newSlot([&a](){return a.method2(4321);});

    core::thread::Worker::sptr w = core::thread::Worker::New();

    slot1->setWorker(w);
    slot2->setWorker(w);
    slot4->setWorker(w);
    slot5->setWorker(w);

    slot1->asyncRun(40, 2).wait();
    slot2->asyncRun();
    slot3->asyncRun(w, 2.1F).wait();
    slot4->asyncRun(w, 40, 2, 3).wait();
    slot5->asyncRun().wait();

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, lastThreeSumResult);

    lastSumResult      = 0;
    a.m_method0        = false;
    a.m_method1        = false;
    lastThreeSumResult = 0;

    std::shared_future<int> f1   = slot1->asyncCall(40, 2);
    std::shared_future<void> f2  = slot2->asyncCall();
    std::shared_future<float> f3 = slot3->asyncCall(w, 2.1F);
    std::shared_future<int> f4   = slot4->asyncCall(w, 40, 2, 3);
    std::shared_future<int> f5   = slot5->asyncCall();

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

void SlotTest::slotBaseTest()
{
    A a;

    core::com::SlotBase::sptr slot1 = core::com::newSlot(&sum);
    core::com::SlotBase::sptr slot2 = core::com::newSlot(&A::method0, &a);
    core::com::SlotBase::sptr slot3 = core::com::newSlot(&A::method1, &a);
    core::com::SlotBase::sptr slot4 = core::com::newSlot(&threeSum);
    core::com::SlotBase::sptr slot5 = core::com::newSlot([&a](){a.method2(4321);});

    lastSumResult      = 0;
    lastThreeSumResult = 0;

    slot1->run(40, 2);
    slot2->run();
    slot3->run(2.1F);
    slot4->run(40, 2, 3);
    slot5->run();

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, lastThreeSumResult);
    CPPUNIT_ASSERT(a.m_method2);

    lastSumResult      = 0;
    a.m_method0        = false;
    a.m_method1        = false;
    lastThreeSumResult = 0;

    CPPUNIT_ASSERT_EQUAL(42, slot1->call<int>(40, 2));
    slot2->call<void>();
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT_EQUAL(4.2F, slot3->call<float>(2.1F));
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, slot4->call<int>(40, 2, 3));

    lastSumResult      = 0;
    a.m_method0        = false;
    a.m_method1        = false;
    lastThreeSumResult = 0;

    core::thread::Worker::sptr w = core::thread::Worker::New();

    slot1->setWorker(w);
    slot2->setWorker(w);
    slot3->setWorker(w);
    slot4->setWorker(w);

    slot1->asyncRun(40, 2).wait();
    slot2->asyncRun();
    slot3->asyncRun(2.1F).wait();
    slot4->asyncRun(40, 2, 3).wait();

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
    CPPUNIT_ASSERT_EQUAL(45, lastThreeSumResult);

    lastSumResult      = 0;
    a.m_method0        = false;
    a.m_method1        = false;
    lastThreeSumResult = 0;

    std::shared_future<int> f1   = slot1->asyncCall<int>(40, 2);
    std::shared_future<void> f2  = slot2->asyncCall<void>();
    std::shared_future<float> f3 = slot3->asyncCall<float>(2.1F);
    std::shared_future<int> f4   = slot4->asyncCall<int>(40, 2, 3);

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

void SlotTest::exceptionTest()
{
    core::com::SlotBase::sptr slot = core::com::newSlot(&sum);

    CPPUNIT_ASSERT_THROW(slot->run(), core::com::exception::BadRun);
    CPPUNIT_ASSERT_THROW(slot->call<int>(), core::com::exception::BadCall);
    CPPUNIT_ASSERT_THROW(slot->call<void>(3, 4), core::com::exception::BadCall);
    CPPUNIT_ASSERT_THROW(slot->asyncRun(), core::com::exception::BadRun);
    CPPUNIT_ASSERT_THROW(slot->asyncCall<int>(), core::com::exception::BadCall);
    CPPUNIT_ASSERT_THROW(slot->asyncRun(5, 6), core::com::exception::NoWorker);
    CPPUNIT_ASSERT_THROW(slot->asyncCall<int>(4, 5), core::com::exception::NoWorker);
}

//-----------------------------------------------------------------------------

struct B
{
    B()
    = default;

    //------------------------------------------------------------------------------

    std::thread::id waitSeconds(const unsigned int nbSeconds)
    {
        core::mt::WriteLock lock(m_mutex);
        std::thread::id oldId = m_threadId;
        m_threadId = std::this_thread::get_id();
        m_firstRun = false;

        std::this_thread::sleep_for(std::chrono::seconds(nbSeconds));

        return oldId;
    }

    std::thread::id m_threadId;

    bool m_firstRun {true};

    core::mt::ReadWriteMutex m_mutex;
};

//------------------------------------------------------------------------------

void SlotTest::workerSwapTest()
{
    // Tests if weak call gets interrupted when slot worker is changed while
    // calls are pending.
    bool exceptionThrown = false;
    while(!exceptionThrown)
    {
        using Signature = std::thread::id(const unsigned int);

        B b;

        core::thread::Worker::sptr w1 = core::thread::Worker::New();
        core::thread::Worker::sptr w2 = core::thread::Worker::New();

        core::com::Slot<Signature>::sptr m0 = core::com::newSlot(&B::waitSeconds, &b);

        CPPUNIT_ASSERT(b.m_threadId == std::thread::id());

        m0->setWorker(w1);
        core::com::Slot<Signature>::VoidSharedFutureType future1 = m0->asyncRun(1);
        m0->setWorker(w2);

        {
            core::mt::ReadLock lock(b.m_mutex);
            if(b.m_threadId == std::thread::id())
            {
                exceptionThrown = true;
                CPPUNIT_ASSERT_THROW(future1.get(), core::com::exception::WorkerChanged);
            }
            else
            {
                CPPUNIT_ASSERT(b.m_threadId == w1->getThreadId());
            }
        }
        w1->stop();
        w2->stop();
    }

    //Tests weakcalls to hold slot worker while running weakcall (asyncRun test)
    {
        using Signature = std::thread::id(const unsigned int);

        B b;

        core::thread::Worker::sptr w1 = core::thread::Worker::New();
        core::thread::Worker::sptr w2 = core::thread::Worker::New();

        core::com::Slot<Signature>::sptr m0 = core::com::newSlot(&B::waitSeconds, &b);

        CPPUNIT_ASSERT(b.m_threadId == std::thread::id());

        m0->setWorker(w1);
        core::com::Slot<Signature>::VoidSharedFutureType future1 = m0->asyncRun(1);

        // wait until we entered in slot's function
        while(b.m_firstRun)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        m0->setWorker(w2);
        future1.wait();
        CPPUNIT_ASSERT(b.m_threadId == w1->getThreadId());

        core::com::Slot<Signature>::VoidSharedFutureType future2 = m0->asyncRun(1);

        future2.wait();

        CPPUNIT_ASSERT(b.m_threadId == w2->getThreadId());
        w1->stop();
        w2->stop();
    }

    //Tests weakcalls to hold slot worker while running weakcall (asyncCall test)
    {
        using Signature = std::thread::id(const unsigned int);

        B b;

        core::thread::Worker::sptr w1 = core::thread::Worker::New();
        core::thread::Worker::sptr w2 = core::thread::Worker::New();

        core::com::Slot<Signature>::sptr m0 = core::com::newSlot(&B::waitSeconds, &b);

        CPPUNIT_ASSERT(b.m_threadId == std::thread::id());

        m0->setWorker(w1);
        core::com::Slot<Signature>::SharedFutureType future1 = m0->asyncCall(1);

        // wait until we entered in slot's function
        while(b.m_firstRun)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        m0->setWorker(w2);
        future1.wait();
        CPPUNIT_ASSERT(b.m_threadId == w1->getThreadId());

        core::com::Slot<Signature>::SharedFutureType future2 = m0->asyncCall(1);

        future2.wait();

        CPPUNIT_ASSERT(b.m_threadId == w2->getThreadId());
        CPPUNIT_ASSERT(future1.get() == std::thread::id());
        CPPUNIT_ASSERT(future2.get() == w1->getThreadId());
        w1->stop();
        w2->stop();
    }
}

//------------------------------------------------------------------------------

void SlotTest::sloppinessTest()
{
    // Tests whether fallback when calling a slot with too many arguments works
    // correctly.
    A a;
    lastSumResult = 0;

    core::com::SlotBase::sptr slot1 = core::com::newSlot(&sum);
    core::com::SlotBase::sptr slot2 = core::com::newSlot(&A::method0, &a);
    core::com::SlotBase::sptr slot3 = core::com::newSlot(&A::method1, &a);

    slot1->run(40, 2, 3);
    slot2->run("Hello world");
    slot3->run(2.1F, 4.2F);

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);

    a.m_method0 = false;
    a.m_method1 = false;

    CPPUNIT_ASSERT_EQUAL(42, slot1->call<int>(40, 2, 3));
    slot2->call<void>("Hello world");
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT_EQUAL(4.2F, slot3->call<float>(2.1F, 4.2F));
    CPPUNIT_ASSERT(a.m_method1);

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    core::thread::Worker::sptr w = core::thread::Worker::New();

    slot1->setWorker(w);
    slot2->setWorker(w);
    slot3->setWorker(w);

    slot1->asyncRun(40, 2, 3).wait();
    slot2->asyncRun("Hello world");
    slot3->asyncRun(2.1F, 4.2F).wait();

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    std::shared_future<int> f1   = slot1->asyncCall<int>(40, 2, 3);
    std::shared_future<void> f2  = slot2->asyncCall<void>("Hello world");
    std::shared_future<float> f3 = slot3->asyncCall<float>(2.1F, 4.2F);

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

void SlotTest::noWorkerTest()
{
    // Tests whether the good exception is thrown when asynchronously calling a
    // slot without worker
    core::com::Slot<int(int, int)>::sptr slot = core::com::newSlot(&sum);

    CPPUNIT_ASSERT_THROW(slot->asyncRun(5, 6), core::com::exception::NoWorker);
    CPPUNIT_ASSERT_THROW(slot->asyncCall(4, 5), core::com::exception::NoWorker);

    core::thread::Worker::sptr w = core::thread::Worker::New();
    slot->setWorker(w);

    CPPUNIT_ASSERT_THROW(slot->asyncRun(nullptr, 5, 6), core::com::exception::NoWorker);
    CPPUNIT_ASSERT_THROW(slot->asyncCall(nullptr, 4, 5), core::com::exception::NoWorker);

    w->stop();
}

} // namespace sight::core::com::ut
