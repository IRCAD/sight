/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "SlotTest.hpp"

#include "fwCom/exception/BadCall.hpp"
#include "fwCom/exception/BadRun.hpp"
#include "fwCom/exception/NoWorker.hpp"
#include "fwCom/util/AutoBind.hpp"
#include "fwCom/util/AutoBind.hxx"
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>

#include <fwCore/mt/types.hpp>

#include <fwThread/Worker.hpp>

#include <boost/bind.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/typeof/typeof.hpp>

#include <functional>
#include <future>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCom::ut::SlotTest );

namespace fwCom
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
        return 2*f;
    }

    bool m_method0;
    bool m_method1;

};

static int lastSumResult = 0;

//------------------------------------------------------------------------------

int sum (int a, int b)
{
    lastSumResult = a+b;
    return lastSumResult;
}

//------------------------------------------------------------------------------

void SlotTest::buildTest ()
{
    A a;

    ::fwCom::Slot< int (int, int) >::sptr slot1 = ::fwCom::newSlot( &sum );
    ::fwCom::Slot< void() >::sptr slot2         = ::fwCom::newSlot( &A::method0, &a );
    ::fwCom::Slot< float (float) >::sptr slot3  = ::fwCom::newSlot( &A::method1, &a  );

    CPPUNIT_ASSERT( slot1 );
    CPPUNIT_ASSERT( slot2 );
    CPPUNIT_ASSERT( slot3 );

    CPPUNIT_ASSERT_EQUAL( (unsigned int) 2, slot1->arity() );
    CPPUNIT_ASSERT_EQUAL( (unsigned int) 0, slot2->arity() );
    CPPUNIT_ASSERT_EQUAL( (unsigned int) 1, slot3->arity() );

    ::fwCom::SlotCall< int (int, int) >::sptr slotcall1
        = std::dynamic_pointer_cast< ::fwCom::SlotCall< int (int, int) > > ( slot1 );
    ::fwCom::SlotCall< void() >::sptr slotcall2
        = std::dynamic_pointer_cast< ::fwCom::SlotCall< void() >        > ( slot2 );
    ::fwCom::SlotCall< float (float) >::sptr slotcall3
        = std::dynamic_pointer_cast< ::fwCom::SlotCall< float (float) >  > ( slot3 );

    CPPUNIT_ASSERT( slotcall1 );
    CPPUNIT_ASSERT( slotcall2 );
    CPPUNIT_ASSERT( slotcall3 );

    ::fwCom::SlotRun< void (int, int) >::sptr slotrun1
        = std::dynamic_pointer_cast< ::fwCom::SlotRun< void (int, int) > > ( slot1 );
    ::fwCom::SlotRun< void() >::sptr slotrun2
        = std::dynamic_pointer_cast< ::fwCom::SlotRun< void() >         > ( slot2 );
    ::fwCom::SlotRun< void (float) >::sptr slotrun3
        = std::dynamic_pointer_cast< ::fwCom::SlotRun< void (float) >    > ( slot3 );

    CPPUNIT_ASSERT( slotrun1 );
    CPPUNIT_ASSERT( slotrun2 );
    CPPUNIT_ASSERT( slotrun3 );

    ::fwCom::SlotBase::sptr slotbase1 = std::dynamic_pointer_cast< ::fwCom::SlotBase > ( slot1 );
    ::fwCom::SlotBase::sptr slotbase2 = std::dynamic_pointer_cast< ::fwCom::SlotBase > ( slot2 );
    ::fwCom::SlotBase::sptr slotbase3 = std::dynamic_pointer_cast< ::fwCom::SlotBase > ( slot3 );

    CPPUNIT_ASSERT( slotbase1 );
    CPPUNIT_ASSERT( slotbase2 );
    CPPUNIT_ASSERT( slotbase3 );

}

//-----------------------------------------------------------------------------

void SlotTest::runTest ()
{
    A a;

    ::fwCom::Slot< int (int, int) >::sptr slot1 = ::fwCom::newSlot( &sum );
    ::fwCom::Slot< void() >::sptr slot2         = ::fwCom::newSlot( &A::method0, &a );
    ::fwCom::Slot< float (float) >::sptr slot3  = ::fwCom::newSlot( &A::method1, &a );

    lastSumResult = 0;
    CPPUNIT_ASSERT( !a.m_method0 );
    CPPUNIT_ASSERT( !a.m_method1 );

    slot1->run(34, 66);
    slot2->run();
    slot3->run(88.1236);

    CPPUNIT_ASSERT_EQUAL( 100, lastSumResult );
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT( a.m_method1 );
}
//-----------------------------------------------------------------------------

void SlotTest::callTest ()
{
    A a;

    ::fwCom::Slot< int (int, int) >::sptr slot1 = ::fwCom::newSlot( &sum );
    ::fwCom::Slot< void() >::sptr slot2         = ::fwCom::newSlot( &A::method0, &a );
    ::fwCom::Slot< float (float) >::sptr slot3  = ::fwCom::newSlot( &A::method1, &a );

    CPPUNIT_ASSERT_EQUAL( 42, slot1->call(40, 2) );
    slot2->call();
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT_EQUAL( 4.2f, slot3->call(2.1f) );
    CPPUNIT_ASSERT( a.m_method1 );
}

//-----------------------------------------------------------------------------

void SlotTest::asyncTest ()
{
    A a;

    ::fwCom::Slot< int (int, int) >::sptr slot1 = ::fwCom::newSlot( &sum );
    ::fwCom::Slot< void() >::sptr slot2         = ::fwCom::newSlot( &A::method0, &a );
    ::fwCom::Slot< float (float) >::sptr slot3  = ::fwCom::newSlot( &A::method1, &a );

    fwThread::Worker::sptr w = fwThread::Worker::New();

    slot1->setWorker(w);
    slot2->setWorker(w);
    slot3->setWorker(w);

    slot1->asyncRun(40, 2).wait();
    slot2->asyncRun();
    slot3->asyncRun(2.1f).wait();

    CPPUNIT_ASSERT_EQUAL( 42, lastSumResult );
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT( a.m_method1 );

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    std::shared_future< int >   f1 = slot1->asyncCall(40, 2);
    std::shared_future< void >  f2 = slot2->asyncCall();
    std::shared_future< float > f3 = slot3->asyncCall(2.1f);

    f1.wait();
    CPPUNIT_ASSERT( f1.valid() );
    CPPUNIT_ASSERT_EQUAL( 42, f1.get() );
    CPPUNIT_ASSERT( f1.valid() );

    f2.wait();
    CPPUNIT_ASSERT( f2.valid() );
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT( f2.valid() );

    f3.wait();
    CPPUNIT_ASSERT( f3.valid() );
    CPPUNIT_ASSERT_EQUAL( 4.2f, f3.get() );
    CPPUNIT_ASSERT( f3.valid() );
    CPPUNIT_ASSERT( a.m_method1 );
}

//-----------------------------------------------------------------------------

void SlotTest::slotBaseTest ()
{
    A a;

    ::fwCom::SlotBase::sptr slot1 = ::fwCom::newSlot( &sum );
    ::fwCom::SlotBase::sptr slot2 = ::fwCom::newSlot( &A::method0, &a );
    ::fwCom::SlotBase::sptr slot3 = ::fwCom::newSlot( &A::method1, &a );

    lastSumResult = 0;

    slot1->run(40, 2);
    slot2->run();
    slot3->run(2.1f);

    CPPUNIT_ASSERT_EQUAL( 42, lastSumResult );
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT( a.m_method1 );

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    CPPUNIT_ASSERT_EQUAL( 42, slot1->call<int>(40, 2) );
    slot2->call<void>();
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT_EQUAL( 4.2f, slot3->call<float>(2.1f) );
    CPPUNIT_ASSERT( a.m_method1 );

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    fwThread::Worker::sptr w = fwThread::Worker::New();

    slot1->setWorker(w);
    slot2->setWorker(w);
    slot3->setWorker(w);

    slot1->asyncRun(40, 2).wait();
    slot2->asyncRun();
    slot3->asyncRun(2.1f).wait();

    CPPUNIT_ASSERT_EQUAL( 42, lastSumResult );
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT( a.m_method1 );

    lastSumResult = 0;
    a.m_method0   = false;
    a.m_method1   = false;

    std::shared_future< int >   f1 = slot1->asyncCall<int>(40, 2);
    std::shared_future< void >  f2 = slot2->asyncCall<void>();
    std::shared_future< float > f3 = slot3->asyncCall<float>(2.1f);

    f1.wait();
    CPPUNIT_ASSERT( f1.valid() );
    CPPUNIT_ASSERT_EQUAL( 42, f1.get() );
    CPPUNIT_ASSERT( f1.valid() );

    f2.wait();
    CPPUNIT_ASSERT( f2.valid() );
    CPPUNIT_ASSERT( a.m_method0 );
    CPPUNIT_ASSERT( f2.valid() );

    f3.wait();
    CPPUNIT_ASSERT( f3.valid() );
    CPPUNIT_ASSERT_EQUAL( 4.2f, f3.get() );
    CPPUNIT_ASSERT( f3.valid() );
    CPPUNIT_ASSERT( a.m_method1 );
}

//-----------------------------------------------------------------------------

void SlotTest::exceptionTest ()
{

    ::fwCom::SlotBase::sptr slot = ::fwCom::newSlot( &sum );

    CPPUNIT_ASSERT_THROW(slot->run(), fwCom::exception::BadRun);
    CPPUNIT_ASSERT_THROW(slot->call<int>(), fwCom::exception::BadCall);
    CPPUNIT_ASSERT_THROW(slot->call<void>(3, 4), fwCom::exception::BadCall);
    CPPUNIT_ASSERT_THROW(slot->asyncRun(), fwCom::exception::BadRun);
    CPPUNIT_ASSERT_THROW(slot->asyncCall<int>(), fwCom::exception::BadCall);
    CPPUNIT_ASSERT_THROW(slot->asyncRun(5, 6), fwCom::exception::NoWorker);
    CPPUNIT_ASSERT_THROW(slot->asyncCall<int>(4, 5), fwCom::exception::NoWorker);
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

    ::boost::thread::id waitSeconds(const unsigned int nbSeconds)
    {
        ::fwCore::mt::WriteLock lock(m_mutex);
        ::boost::thread::id oldId = m_threadId;
        m_threadId = ::boost::this_thread::get_id();
        m_firstRun = false;

        ::boost::this_thread::sleep_for( ::boost::chrono::seconds(nbSeconds));

        return oldId;
    }

    bool m_firstRun;

    ::boost::thread::id m_threadId;

    ::fwCore::mt::ReadWriteMutex m_mutex;
};

//------------------------------------------------------------------------------

void SlotTest::workerSwapTest()
{

    // Tests if weak call gets interrupted when slot worker is changed while
    // calls are pending.
    bool exceptionThrown = false;
    while(!exceptionThrown)
    {
        typedef ::boost::thread::id Signature (const unsigned int);

        B b;

        ::fwThread::Worker::sptr w1 = ::fwThread::Worker::New();
        ::fwThread::Worker::sptr w2 = ::fwThread::Worker::New();

        ::fwCom::Slot< Signature >::sptr m0 = ::fwCom::newSlot( &B::waitSeconds, &b );

        CPPUNIT_ASSERT(b.m_threadId == ::boost::thread::id());

        m0->setWorker(w1);
        ::fwCom::Slot< Signature >::VoidSharedFutureType future1 = m0->asyncRun(1);
        m0->setWorker(w2);

        {
            ::fwCore::mt::ReadLock lock(b.m_mutex);
            if(b.m_threadId == ::boost::thread::id())
            {
                exceptionThrown = true;
                CPPUNIT_ASSERT_THROW( future1.get(), fwCom::exception::WorkerChanged );
            }
            else
            {
                CPPUNIT_ASSERT(b.m_threadId == w1->getThreadId());
            }
        }
    }

    //Tests weakcalls to hold slot worker while running weakcall (asyncRun test)
    {
        typedef ::boost::thread::id Signature (const unsigned int);

        B b;

        ::fwThread::Worker::sptr w1 = ::fwThread::Worker::New();
        ::fwThread::Worker::sptr w2 = ::fwThread::Worker::New();

        ::fwCom::Slot< Signature >::sptr m0 = ::fwCom::newSlot( &B::waitSeconds, &b );

        CPPUNIT_ASSERT(b.m_threadId == ::boost::thread::id());

        m0->setWorker(w1);
        ::fwCom::Slot< Signature >::VoidSharedFutureType future1 = m0->asyncRun(1);

        // wait until we entered in slot's function
        while(b.m_firstRun)
        {
        }

        m0->setWorker(w2);
        future1.wait();
        CPPUNIT_ASSERT(b.m_threadId == w1->getThreadId());

        ::fwCom::Slot< Signature >::VoidSharedFutureType future2 = m0->asyncRun(1);

        future2.wait();

        CPPUNIT_ASSERT(b.m_threadId == w2->getThreadId());
    }

    //Tests weakcalls to hold slot worker while running weakcall (asyncCall test)
    {
        typedef ::boost::thread::id Signature (const unsigned int);

        B b;

        ::fwThread::Worker::sptr w1 = ::fwThread::Worker::New();
        ::fwThread::Worker::sptr w2 = ::fwThread::Worker::New();

        ::fwCom::Slot< Signature >::sptr m0 = ::fwCom::newSlot( &B::waitSeconds, &b );

        CPPUNIT_ASSERT(b.m_threadId == ::boost::thread::id());

        m0->setWorker(w1);
        ::fwCom::Slot< Signature >::SharedFutureType future1 = m0->asyncCall(1);

        // wait until we entered in slot's function
        while(b.m_firstRun)
        {
        }

        m0->setWorker(w2);
        future1.wait();
        CPPUNIT_ASSERT(b.m_threadId == w1->getThreadId());

        ::fwCom::Slot< Signature >::SharedFutureType future2 = m0->asyncCall(1);

        future2.wait();

        CPPUNIT_ASSERT(b.m_threadId == w2->getThreadId());
        CPPUNIT_ASSERT(future1.get() == ::boost::thread::id());
        CPPUNIT_ASSERT(future2.get() == w1->getThreadId());

    }

}

} //namespace ut

} //namespace fwCom

