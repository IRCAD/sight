/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/thread/Timer.hpp>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

#include <ui/qt/App.hpp>
#include <ui/qt/WorkerQt.hpp>

#include <cppunit/Exception.h>

#include <QApplication>
#include <QSharedPointer>
#include <QTimer>

#include <array>
#include <functional>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::qt::ut::WorkerQtTest);

namespace sight::ui::qt
{

// Defined in WorkerQt.cpp
class WorkerQt;

namespace ut
{

struct TestHandler
{
    TestHandler()
    = default;

    //------------------------------------------------------------------------------

    void nextStep()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->nextStepNoSleep();
    }

    //------------------------------------------------------------------------------

    void nextStepNoSleep()
    {
        m_threadCheckOk &= (m_workerThreadId == core::thread::getCurrentThreadId());
        ++m_step;
    }

    //------------------------------------------------------------------------------

    void setWorkerId(core::thread::ThreadIdType id)
    {
        m_workerThreadId = id;
    }

    int m_step {0};
    bool m_threadCheckOk {true};
    core::thread::ThreadIdType m_workerThreadId;
};

//-----------------------------------------------------------------------------

void WorkerQtTest::setUp()
{
    // Set up context before running a test.
    static std::string arg1 = "WorkerQtTest";
#if defined(__linux)
    static std::string arg2 = "-platform";
    static std::string arg3 = "offscreen";
    std::array argv         = {arg1.data(), arg2.data(), arg3.data(), static_cast<char*>(nullptr)};
#else
    std::array argv = {arg1.data(), static_cast<char*>(nullptr)};
#endif
    int argc = int(argv.size() - 1);

    CPPUNIT_ASSERT(qApp == nullptr);
    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& argc, char** argv)
        {
#if QT_VERSION < 0x050000
            bool guiEnabled = false;
#else
            bool guiEnabled = true;
#endif
            return QSharedPointer<QApplication>(new ui::qt::App(argc, argv, guiEnabled));
        };
    m_worker = ui::qt::getQtWorker(argc, argv.data(), callback, "", "");
}

//-----------------------------------------------------------------------------

void WorkerQtTest::tearDown()
{
    // Clean up after the test run.
    m_worker.reset();
    CPPUNIT_ASSERT(qApp == nullptr);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::initTest()
{
    CPPUNIT_ASSERT(qApp != nullptr);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::twiceInitTest()
{
    CPPUNIT_ASSERT(qApp != nullptr);
}

//-----------------------------------------------------------------------------

void runBasicTest(TestHandler& handler, core::thread::Worker::sptr worker)
{
    handler.setWorkerId(worker->getThreadId());
    worker->post([&handler]{handler.nextStep();});
    worker->post([&handler]{handler.nextStep();});
    worker->post([&handler]{handler.nextStep();});

    worker->post([]{return QApplication::quit();});
}

//-----------------------------------------------------------------------------

void runBasicTestChecks(const TestHandler& handler)
{
    CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
    CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::basicTest()
{
    TestHandler handler;

    runBasicTest(handler, m_worker);

    m_worker->getFuture().wait();

    runBasicTestChecks(handler);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::postFromInsideTest()
{
    TestHandler handler;

    m_worker->post([&handler, this]{return runBasicTest(handler, m_worker);});

    m_worker->getFuture().wait();

    runBasicTestChecks(handler);
}

//-----------------------------------------------------------------------------

void doNothing()
{
}

//-----------------------------------------------------------------------------

void runFromOutsideTest(TestHandler& handler, core::thread::Worker::sptr worker)
{
    //waiting for WorkerQt to start
    worker->postTask<void>([]{return doNothing();}).wait();

    runBasicTest(handler, worker);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::postFromOutsideTest()
{
    TestHandler handler;

    std::thread testThread([&handler, this]{return runFromOutsideTest(handler, m_worker);});

    m_worker->getFuture().wait();

    runBasicTestChecks(handler);

    testThread.join();
}

//-----------------------------------------------------------------------------

static CppUnit::Exception exception;

//------------------------------------------------------------------------------

static inline void qtTest(const std::function<void()>& f)
{
    try
    {
        f();
    }
    catch(CppUnit::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exception = e;
        QApplication::exit(1);
        return;
    }
}

//-----------------------------------------------------------------------------

void runBasicTimerTest(
    const TestHandler& handler,
    const core::thread::Timer::sptr& timer,
    core::thread::Timer::TimeDurationType
    /*unused*/
)
{
    timer->start();

    qtTest(
        [&]
            {
                CPPUNIT_ASSERT(timer->isRunning());
                CPPUNIT_ASSERT(handler.m_threadCheckOk);
                CPPUNIT_ASSERT_EQUAL(0, handler.m_step);
            });
}

//-----------------------------------------------------------------------------

void oneShotBasicTimerTest(
    int& i,
    TestHandler& handler,
    const core::thread::Timer::sptr& timer,
    core::thread::Timer::TimeDurationType /*unused*/,
    const core::thread::Worker::sptr& worker
)
{
    handler.nextStepNoSleep();

    qtTest(
        [&]
            {
                CPPUNIT_ASSERT(timer->isRunning());
                CPPUNIT_ASSERT(handler.m_threadCheckOk);
                CPPUNIT_ASSERT_EQUAL(i, handler.m_step);
            });

    if(++i == 50)
    {
        timer->stop();

        qtTest(
            [&]
                {
                    CPPUNIT_ASSERT(!timer->isRunning());
                    CPPUNIT_ASSERT(handler.m_threadCheckOk);
                    CPPUNIT_ASSERT_EQUAL(49, handler.m_step);
                });
        worker->post([]{return QApplication::quit();});
    }
}

//-----------------------------------------------------------------------------

void WorkerQtTest::basicTimerTest()
{
    TestHandler handler;
    handler.setWorkerId(m_worker->getThreadId());

    core::thread::Timer::sptr timer = m_worker->createTimer();

    core::thread::Timer::TimeDurationType duration = std::chrono::milliseconds(10);

    int i = 1;
    timer->setFunction(
        [&i, &handler, &timer, duration, this]{return oneShotBasicTimerTest(i, handler, timer, duration, m_worker);});
    timer->setDuration(duration);

    CPPUNIT_ASSERT(!timer->isRunning());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    m_worker->post([&handler, &timer, duration]{return runBasicTimerTest(handler, timer, duration);});

    core::thread::Worker::FutureType future = m_worker->getFuture();
    future.wait();

    CPPUNIT_ASSERT_EQUAL(0, std::any_cast<int>(future.get()));
}

//-----------------------------------------------------------------------------

} //namespace ut

} //namespace sight::ui::qt
