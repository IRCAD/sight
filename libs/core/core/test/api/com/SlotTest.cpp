/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::SlotTest);

namespace sight::core::com
{

namespace ut
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
    A() :
        m_method0(false),
        m_method1(false)
    {
    }

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

    bool m_method0;
    bool m_method1;
};

static int lastSumResult = 0;

//------------------------------------------------------------------------------

int sum(int a, int b)
{
    lastSumResult = a + b;
    return lastSumResult;
}

//------------------------------------------------------------------------------

void SlotTest::buildTest()
{
    A a;

    core::com::Slot<int(int, int)>::sptr slot1 = core::com::newSlot(&sum);
    core::com::Slot<void()>::sptr slot2        = core::com::newSlot(&A::method0, &a);
    core::com::Slot<float(float)>::sptr slot3  = core::com::newSlot(&A::method1, &a);

    CPPUNIT_ASSERT(slot1);
    CPPUNIT_ASSERT(slot2);
    CPPUNIT_ASSERT(slot3);

    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, slot1->arity());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 0, slot2->arity());
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, slot3->arity());

    core::com::SlotCall<int(int, int)>::sptr slotcall1 =
        std::dynamic_pointer_cast<core::com::SlotCall<int(int, int)> >(slot1);
    core::com::SlotCall<void()>::sptr slotcall2 =
        std::dynamic_pointer_cast<core::com::SlotCall<void()> >(slot2);
    core::com::SlotCall<float(float)>::sptr slotcall3 =
        std::dynamic_pointer_cast<core::com::SlotCall<float(float)> >(slot3);

    CPPUNIT_ASSERT(slotcall1);
    CPPUNIT_ASSERT(slotcall2);
    CPPUNIT_ASSERT(slotcall3);

    core::com::SlotRun<void(int, int)>::sptr slotrun1 =
        std::dynamic_pointer_cast<core::com::SlotRun<void(int, int)> >(slot1);
    core::com::SlotRun<void()>::sptr slotrun2 =
        std::dynamic_pointer_cast<core::com::SlotRun<void()> >(slot2);
    core::com::SlotRun<void(float)>::sptr slotrun3 =
        std::dynamic_pointer_cast<core::com::SlotRun<void(float)> >(slot3);

    CPPUNIT_ASSERT(slotrun1);
    CPPUNIT_ASSERT(slotrun2);
    CPPUNIT_ASSERT(slotrun3);

    core::com::SlotBase::sptr slotbase1 = std::dynamic_pointer_cast<core::com::SlotBase>(slot1);
    core::com::SlotBase::sptr slotbase2 = std::dynamic_pointer_cast<core::com::SlotBase>(slot2);
    core::com::SlotBase::sptr slotbase3 = std::dynamic_pointer_cast<core::com::SlotBase>(slot3);

    CPPUNIT_ASSERT(slotbase1);
    CPPUNIT_ASSERT(slotbase2);
    CPPUNIT_ASSERT(slotbase3);
}

//-----------------------------------------------------------------------------

void SlotTest::runTest()
{
    A a;

    core::com::Slot<int(int, int)>::sptr slot1 = core::com::newSlot(&sum);
    core::com::Slot<void()>::sptr slot2        = core::com::newSlot(&A::method0, &a);
    core::com::Slot<float(float)>::sptr slot3  = core::com::newSlot(&A::method1, &a);

    lastSumResult = 0;
    CPPUNIT_ASSERT(!a.m_method0);
    CPPUNIT_ASSERT(!a.m_method1);

    slot1->run(34, 66);
    slot2->run();
    slot3->run(88.1236F);

    CPPUNIT_ASSERT_EQUAL(100, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);
}

//-----------------------------------------------------------------------------

void SlotTest::callTest()
{
    A a;

    core::com::Slot<int(int, int)>::sptr slot1 = core::com::newSlot(&sum);
    core::com::Slot<void()>::sptr slot2        = core::com::newSlot(&A::method0, &a);
    core::com::Slot<float(float)>::sptr slot3  = core::com::newSlot(&A::method1, &a);

    CPPUNIT_ASSERT_EQUAL(42, slot1->call(40, 2));
    slot2->call();
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT_EQUAL(4.2f, slot3->call(2.1f));
    CPPUNIT_ASSERT(a.m_method1);
}

//-----------------------------------------------------------------------------

void SlotTest::asyncTest()
{
    A a;

    core::com::Slot<int(int, int)>::sptr slot1 = core::com::newSlot(&sum);
    core::com::Slot<void()>::sptr slot2        = core::com::newSlot(&A::method0, &a);
    core::com::Slot<float(float)>::sptr slot3  = core::com::newSlot(&A::method1, &a);

    core::thread::Worker::sptr w = core::thread::Worker::New();

    slot1->setWorker(w);
    slot2->setWorker(w);
    slot3->setWorker(w);

    slot1->asyncRun(40, 2).wait();
    slot2->asyncRun();
    slot3->asyncRun(2.1f).wait();

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    std::shared_future<int> f1   = slot1->asyncCall(40, 2);
    std::shared_future<void> f2  = slot2->asyncCall();
    std::shared_future<float> f3 = slot3->asyncCall(2.1f);

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
    CPPUNIT_ASSERT_EQUAL(4.2f, f3.get());
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT(a.m_method1);
    w->stop();
}

//-----------------------------------------------------------------------------

void SlotTest::slotBaseTest()
{
    A a;

    core::com::SlotBase::sptr slot1 = core::com::newSlot(&sum);
    core::com::SlotBase::sptr slot2 = core::com::newSlot(&A::method0, &a);
    core::com::SlotBase::sptr slot3 = core::com::newSlot(&A::method1, &a);

    lastSumResult = 0;

    slot1->run(40, 2);
    slot2->run();
    slot3->run(2.1f);

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    CPPUNIT_ASSERT_EQUAL(42, slot1->call<int>(40, 2));
    slot2->call<void>();
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT_EQUAL(4.2f, slot3->call<float>(2.1f));
    CPPUNIT_ASSERT(a.m_method1);

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    core::thread::Worker::sptr w = core::thread::Worker::New();

    slot1->setWorker(w);
    slot2->setWorker(w);
    slot3->setWorker(w);

    slot1->asyncRun(40, 2).wait();
    slot2->asyncRun();
    slot3->asyncRun(2.1f).wait();

    CPPUNIT_ASSERT_EQUAL(42, lastSumResult);
    CPPUNIT_ASSERT(a.m_method0);
    CPPUNIT_ASSERT(a.m_method1);

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    std::shared_future<int> f1   = slot1->asyncCall<int>(40, 2);
    std::shared_future<void> f2  = slot2->asyncCall<void>();
    std::shared_future<float> f3 = slot3->asyncCall<float>(2.1f);

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
    CPPUNIT_ASSERT_EQUAL(4.2f, f3.get());
    CPPUNIT_ASSERT(f3.valid());
    CPPUNIT_ASSERT(a.m_method1);
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
    B() :
        m_threadId(),
        m_firstRun(true)
    {
    }

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

    bool m_firstRun;

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
        typedef std::thread::id Signature(const unsigned int);

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
        typedef std::thread::id Signature(const unsigned int);

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
        typedef std::thread::id Signature(const unsigned int);

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

} //namespace ut

} //namespace sight::core::com
