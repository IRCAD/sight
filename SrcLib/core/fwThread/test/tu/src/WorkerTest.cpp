/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "WorkerTest.hpp"

#include <fwThread/Timer.hpp>
#include <fwThread/Worker.hpp>

#include <fwCore/spyLog.hpp>

#include <fwTest/Exception.hpp>

#include <exception>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwThread::ut::WorkerTest );

namespace fwThread
{
namespace ut
{

static ::fwTest::Exception e(""); // force link with fwTest

//------------------------------------------------------------------------------

void WorkerTest::setUp()
{
    // Set up context before running a test.
}
//------------------------------------------------------------------------------

void WorkerTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

struct TestHandler
{
    TestHandler() :
        m_step(0),
        m_threadCheckOk(true)
    {
        m_constructorThreadId = ::fwThread::getCurrentThreadId();
    }

    //------------------------------------------------------------------------------

    void nextStep()
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(50));
        this->nextStepNoSleep();
    }

    //------------------------------------------------------------------------------

    void nextStepNoSleep()
    {

        m_threadCheckOk &= (m_constructorThreadId != ::fwThread::getCurrentThreadId());
        m_threadCheckOk &= (m_workerThreadId == ::fwThread::getCurrentThreadId());
        ++m_step;
    }

    //------------------------------------------------------------------------------

    void setWorkerId(::fwThread::ThreadIdType id)
    {
        m_workerThreadId = id;
    }

    int m_step;
    bool m_threadCheckOk;
    ::fwThread::ThreadIdType m_constructorThreadId;
    ::fwThread::ThreadIdType m_workerThreadId;
};

//-----------------------------------------------------------------------------

void WorkerTest::basicTest()
{
    {
        ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

        TestHandler handler;
        handler.setWorkerId(worker->getThreadId());
        worker->post( std::bind( &TestHandler::nextStep, &handler) );
        worker->post( std::bind( &TestHandler::nextStep, &handler) );
        worker->post( std::bind( &TestHandler::nextStep, &handler) );

        worker->stop();
        CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
    }

    {
        ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

        TestHandler handler;
        handler.setWorkerId(worker->getThreadId());
        worker->post( std::bind( &TestHandler::nextStepNoSleep, &handler) );
        worker->post( std::bind( &TestHandler::nextStepNoSleep, &handler) );
        worker->post( std::bind( &TestHandler::nextStepNoSleep, &handler) );

        worker->stop();
        CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
    }
}

//-----------------------------------------------------------------------------

void WorkerTest::timerTest()
{
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    TestHandler handler;
    handler.setWorkerId(worker->getThreadId());

    ::fwThread::Timer::sptr timer = worker->createTimer();

    std::chrono::milliseconds duration = std::chrono::milliseconds(100);

    timer->setFunction( std::bind( &TestHandler::nextStepNoSleep, &handler) );
    timer->setDuration(duration);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    std::this_thread::sleep_for( duration/10. );

    for (int i = 1; i < 50; ++i)
    {
        std::this_thread::sleep_for( duration );

        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(i, handler.m_step);

    }

    timer->stop();

    std::this_thread::sleep_for( duration*3 );

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(49, handler.m_step);

    // test start after stop
    handler.m_step = 0;

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    std::this_thread::sleep_for( duration/10. );

    for (int i = 1; i < 50; ++i)
    {
        std::this_thread::sleep_for( duration );

        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(i, handler.m_step);

    }

    timer->stop();

    std::this_thread::sleep_for( duration*3 );

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(49, handler.m_step);

    // change timer duration on the fly
    // change timer duration
    handler.m_step = 0;

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    std::this_thread::sleep_for( duration/10. );

    for (int i = 1; i < 25; ++i)
    {
        std::this_thread::sleep_for( duration );

        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(i, handler.m_step);

    }

    duration = std::chrono::milliseconds(50);
    timer->setDuration(duration);

    for (int i = 24; i < 50; ++i)
    {
        std::this_thread::sleep_for( duration );

        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(i, handler.m_step);

    }

    timer->stop();

    std::this_thread::sleep_for( duration*3 );

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(49, handler.m_step);

    // one shot test
    handler.m_step = 0;

    duration = std::chrono::milliseconds(10);
    timer->setDuration(duration);
    timer->setOneShot(true);

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    std::this_thread::sleep_for( duration*10 );

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(1, handler.m_step);

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
        ::fwThread::Timer::sptr timer = worker->createTimer();
        duration                      = std::chrono::milliseconds(10);
        timer->setFunction( [duration]()
                {
                    std::this_thread::sleep_for( duration*90 );
                } );

        timer->setDuration(duration);

        timer->start();
        std::this_thread::sleep_for( duration * 2 );
        timer->stop();

        timer.reset();
    }

    worker->stop();
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwThread
