/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "WorkerQtTest.hpp"

#include <fwQt/WorkerQt.hpp>

#include <fwGuiQt/App.hpp>
#include <fwGuiQt/config.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwThread/Timer.hpp>
#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>

#include <cppunit/Exception.h>

#include <QApplication>
#include <QSharedPointer>
#include <QTimer>

#include <functional>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGuiQt::ut::WorkerQtTest );

namespace fwGuiQt
{

// Defined in WorkerQt.cpp
class WorkerQt;

namespace ut
{

struct TestHandler
{
    TestHandler() :
        m_step(0),
        m_threadCheckOk(true)
    {
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
    ::fwThread::ThreadIdType m_workerThreadId;
};

//-----------------------------------------------------------------------------

void WorkerQtTest::setUp()
{
    // Set up context before running a test.
    char arg1[] = "fwGuiQtTest";
#if defined(__linux)
    char arg2[]         = "-platform";
    char arg3[]         = "offscreen";
    static char* argv[] = {arg1, arg2, arg3, nullptr};
    static int argc     = 3;
#else
    static char* argv[] = {arg1, 0};
    static int argc     = 1;
#endif

    CPPUNIT_ASSERT(qApp == NULL);
    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback
        = [](int& argc, char** argv)
          {
#if QT_VERSION < 0x050000
              bool guiEnabled = false;
#else
              bool guiEnabled = true;
#endif
              return QSharedPointer< QApplication > (new ::fwGuiQt::App(argc, argv, guiEnabled));
          };
    m_worker = ::fwQt::getQtWorker(argc, argv, callback, "", "");
}

//-----------------------------------------------------------------------------

void WorkerQtTest::tearDown()
{
    // Clean up after the test run.
    m_worker.reset();
    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    CPPUNIT_ASSERT(qApp == NULL);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::initTest()
{
    CPPUNIT_ASSERT(qApp != NULL);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::twiceInitTest()
{
    CPPUNIT_ASSERT(qApp != NULL);
}

//-----------------------------------------------------------------------------

void runBasicTest(TestHandler& handler, ::fwThread::Worker::sptr worker)
{
    handler.setWorkerId(worker->getThreadId());
    worker->post( std::bind( &TestHandler::nextStep, std::ref(handler)) );
    worker->post( std::bind( &TestHandler::nextStep, std::ref(handler)) );
    worker->post( std::bind( &TestHandler::nextStep, std::ref(handler)) );

    worker->post( std::bind( &QApplication::quit ) );
}

//-----------------------------------------------------------------------------

#define RUN_BASIC_TEST_CHECKS(handler) \
    CPPUNIT_ASSERT_EQUAL(3, handler.m_step); \
    CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk)

//-----------------------------------------------------------------------------

void WorkerQtTest::basicTest()
{
    TestHandler handler;

    runBasicTest(handler, m_worker);

    m_worker->getFuture().wait();

    RUN_BASIC_TEST_CHECKS(handler);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::postFromInsideTest()
{
    TestHandler handler;

    m_worker->post( std::bind(runBasicTest, std::ref(handler), m_worker) );

    m_worker->getFuture().wait();

    RUN_BASIC_TEST_CHECKS(handler);
}

//-----------------------------------------------------------------------------

void doNothing()
{
}

//-----------------------------------------------------------------------------

void runFromOutsideTest(TestHandler& handler, ::fwThread::Worker::sptr worker)
{
    //waiting for WorkerQt to start
    worker->postTask<void>( std::bind( doNothing ) ).wait();

    runBasicTest(handler, worker);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::postFromOutsideTest()
{
    TestHandler handler;

    std::thread testThread(std::bind(runFromOutsideTest, std::ref(handler), m_worker));

    m_worker->getFuture().wait();

    RUN_BASIC_TEST_CHECKS(handler);

    testThread.join();
}

//-----------------------------------------------------------------------------

#define QT_TEST_START \
    try

#define QT_TEST_END \
    catch(CppUnit::Exception& e) \
    { \
        std::cerr << e.what() << std::endl; \
        exception = e; \
        QApplication::exit(1); \
        return; \
    }

static CppUnit::Exception exception;

//-----------------------------------------------------------------------------

void runBasicTimerTest( TestHandler& handler,
                        const ::fwThread::Timer::sptr& timer,
                        ::fwThread::Timer::TimeDurationType duration )
{
    timer->start();

    QT_TEST_START
    {
        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(0, handler.m_step);
    }
    QT_TEST_END
}

//-----------------------------------------------------------------------------

void oneShotBasicTimerTest(int& i,
                           TestHandler& handler,
                           const ::fwThread::Timer::sptr& timer,
                           ::fwThread::Timer::TimeDurationType duration,
                           const ::fwThread::Worker::sptr& worker )
{
    handler.nextStepNoSleep();

    QT_TEST_START
    {
        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(i, handler.m_step);
    }
    QT_TEST_END;

    if(++i == 50)
    {
        timer->stop();

        QT_TEST_START
        {
            CPPUNIT_ASSERT(!timer->isRunning());
            CPPUNIT_ASSERT(handler.m_threadCheckOk);
            CPPUNIT_ASSERT_EQUAL(49, handler.m_step);
        }
        QT_TEST_END;
        worker->post( std::bind( &QApplication::quit ) );
    }
}

//-----------------------------------------------------------------------------

void WorkerQtTest::basicTimerTest()
{
    TestHandler handler;
    handler.setWorkerId(m_worker->getThreadId());

    ::fwThread::Timer::sptr timer = m_worker->createTimer();

    ::fwThread::Timer::TimeDurationType duration = std::chrono::milliseconds(10);

    int i = 1;
    timer->setFunction(
        std::bind(
            &oneShotBasicTimerTest,
            std::ref(i), handler, std::ref(timer), duration, std::ref(m_worker) )
        );
    timer->setDuration(duration);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    m_worker->post( std::bind(&runBasicTimerTest, std::ref(handler), std::ref(timer), duration) );

    ::fwThread::Worker::FutureType future = m_worker->getFuture();
    future.wait();

    CPPUNIT_ASSERT_EQUAL( 0, std::any_cast<int>( future.get() ) );
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwGuiQt
