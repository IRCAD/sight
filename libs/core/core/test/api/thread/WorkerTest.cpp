/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "WorkerTest.hpp"

#include <core/spyLog.hpp>
#include <core/thread/Timer.hpp>
#include <core/thread/Worker.hpp>

#include <utest/Exception.hpp>

#include <atomic>
#include <exception>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::thread::ut::WorkerTest);

namespace sight::core::thread
{

namespace ut
{

static utest::Exception e(""); // force link with fwTest

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
    TestHandler()
    {
        m_constructorThreadId = core::thread::getCurrentThreadId();
    }

    //------------------------------------------------------------------------------

    void nextStep()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->nextStepNoSleep();
    }

    //------------------------------------------------------------------------------

    void nextStepNoSleep()
    {
        m_threadCheckOk = m_threadCheckOk.load()
                          && (m_constructorThreadId != core::thread::getCurrentThreadId())
                          && (m_workerThreadId == core::thread::getCurrentThreadId());

        ++m_step;
    }

    //------------------------------------------------------------------------------

    void setWorkerId(core::thread::ThreadIdType id)
    {
        m_workerThreadId = id;
    }

    std::atomic_int m_step {0};
    std::atomic_bool m_threadCheckOk {true};
    core::thread::ThreadIdType m_constructorThreadId;
    core::thread::ThreadIdType m_workerThreadId;
};

//-----------------------------------------------------------------------------

void WorkerTest::basicTest()
{
    {
        core::thread::Worker::sptr worker = core::thread::Worker::New();

        TestHandler handler;
        handler.setWorkerId(worker->getThreadId());
        worker->post(std::bind(&TestHandler::nextStep, &handler));
        worker->post(std::bind(&TestHandler::nextStep, &handler));
        worker->post(std::bind(&TestHandler::nextStep, &handler));

        worker->stop();
        CPPUNIT_ASSERT_EQUAL(3, handler.m_step.load());
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk.load());
    }

    {
        core::thread::Worker::sptr worker = core::thread::Worker::New();

        TestHandler handler;
        handler.setWorkerId(worker->getThreadId());
        worker->post(std::bind(&TestHandler::nextStepNoSleep, &handler));
        worker->post(std::bind(&TestHandler::nextStepNoSleep, &handler));
        worker->post(std::bind(&TestHandler::nextStepNoSleep, &handler));

        worker->stop();
        CPPUNIT_ASSERT_EQUAL(3, handler.m_step.load());
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk.load());
    }
}

//-----------------------------------------------------------------------------

void WorkerTest::timerTest()
{
    core::thread::Worker::sptr worker = core::thread::Worker::New();

    TestHandler handler;
    handler.setWorkerId(worker->getThreadId());

    core::thread::Timer::sptr timer = worker->createTimer();

    std::chrono::milliseconds duration = std::chrono::milliseconds(100);

    timer->setFunction(std::bind(&TestHandler::nextStepNoSleep, &handler));
    timer->setDuration(duration);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration / 10.);

    for(int i = 1 ; i < 50 ; ++i)
    {
        std::this_thread::sleep_for(duration);

        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(45, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(51, handler.m_step.load());

    timer->stop();

    int lastStep = handler.m_step.load();
    std::this_thread::sleep_for(duration * 3);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_LESSEQUAL(lastStep + 5, handler.m_step.load());

    // test start after stop
    handler.m_step.store(0);

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration / 10.);

    for(int i = 1 ; i < 50 ; ++i)
    {
        std::this_thread::sleep_for(duration);
        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(40, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(51, handler.m_step.load());

    timer->stop();

    lastStep = handler.m_step.load();
    std::this_thread::sleep_for(duration * 3);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_LESSEQUAL(lastStep + 5, handler.m_step.load());

    // change timer duration on the fly
    // change timer duration
    handler.m_step.store(0);

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration / 10.);

    for(int i = 1 ; i < 25 ; ++i)
    {
        std::this_thread::sleep_for(duration);

        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(20, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(26, handler.m_step.load());

    duration = std::chrono::milliseconds(50);
    timer->setDuration(duration);

    for(int i = 24 ; i < 50 ; ++i)
    {
        std::this_thread::sleep_for(duration);

        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    }

    // Be a bit tolerant, we have no 100% guarantee that timer will perform the requested number of ticks
    CPPUNIT_ASSERT_GREATEREQUAL(45, handler.m_step.load());
    CPPUNIT_ASSERT_LESSEQUAL(51, handler.m_step.load());

    timer->stop();

    lastStep = handler.m_step.load();
    std::this_thread::sleep_for(duration * 3);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_LESSEQUAL(lastStep + 5, handler.m_step.load());

    // one shot test
    handler.m_step.store(0);

    duration = std::chrono::milliseconds(10);
    timer->setDuration(duration);
    timer->setOneShot(true);

    timer->start();

    CPPUNIT_ASSERT(timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step.load());

    std::this_thread::sleep_for(duration * 10);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk.load());
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
        core::thread::Timer::sptr timer2 = worker->createTimer();
        duration = std::chrono::milliseconds(10);
        timer2->setFunction(
            [duration]()
                {
                    std::this_thread::sleep_for(duration * 90);
                });

        timer2->setDuration(duration);

        timer2->start();
        std::this_thread::sleep_for(duration * 2);
        timer2->stop();

        timer2.reset();
    }

    worker->stop();
}

//-----------------------------------------------------------------------------

} //namespace ut

} //namespace sight::core::thread
