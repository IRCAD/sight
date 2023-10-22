/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "worker_qt_test.hpp"

#include <core/thread/timer.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <ui/qt/app.hpp>
#include <ui/qt/worker_qt.hpp>

#include <cppunit/Exception.h>

#include <QApplication>
#include <QSharedPointer>
#include <QTimer>

#include <array>
#include <functional>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::qt::ut::worker_qt_test);

namespace sight::ui::qt
{

// Defined in worker_qt.cpp
class worker_qt;

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
        m_threadCheckOk &= (m_workerThreadId == core::thread::get_current_thread_id());
        ++m_step;
    }

    //------------------------------------------------------------------------------

    void set_workerId(core::thread::thread_id_t _id)
    {
        m_workerThreadId = _id;
    }

    int m_step {0};
    bool m_threadCheckOk {true};
    core::thread::thread_id_t m_workerThreadId;
};

//-----------------------------------------------------------------------------

void worker_qt_test::setUp()
{
    // Set up context before running a test.
    static std::string arg1 = "worker_qt_test";
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
        [](int& _argc, char** _argv)
        {
#if QT_VERSION < 0x050000
            bool guiEnabled = false;
#else
            bool gui_enabled = true;
#endif
            return QSharedPointer<QApplication>(new ui::qt::app(_argc, _argv, gui_enabled));
        };
    m_worker = ui::qt::get_qt_worker(argc, argv.data(), callback, "", "");
}

//-----------------------------------------------------------------------------

void worker_qt_test::tearDown()
{
    // Clean up after the test run.
    m_worker.reset();
    CPPUNIT_ASSERT(qApp == nullptr);
}

//-----------------------------------------------------------------------------

void worker_qt_test::initTest()
{
    CPPUNIT_ASSERT(qApp != nullptr);
}

//-----------------------------------------------------------------------------

void worker_qt_test::twiceInitTest()
{
    CPPUNIT_ASSERT(qApp != nullptr);
}

//-----------------------------------------------------------------------------

void run_basic_test(TestHandler& _handler, core::thread::worker::sptr _worker)
{
    _handler.set_workerId(_worker->get_thread_id());
    _worker->post([&_handler]{_handler.nextStep();});
    _worker->post([&_handler]{_handler.nextStep();});
    _worker->post([&_handler]{_handler.nextStep();});

    _worker->post([]{return QApplication::quit();});
}

//-----------------------------------------------------------------------------

void run_basic_test_checks(const TestHandler& _handler)
{
    CPPUNIT_ASSERT_EQUAL(3, _handler.m_step);
    CPPUNIT_ASSERT_EQUAL(true, _handler.m_threadCheckOk);
}

//-----------------------------------------------------------------------------

void worker_qt_test::basicTest()
{
    TestHandler handler;

    run_basic_test(handler, m_worker);

    m_worker->get_future().wait();

    run_basic_test_checks(handler);
}

//-----------------------------------------------------------------------------

void worker_qt_test::postFromInsideTest()
{
    TestHandler handler;

    m_worker->post([&handler, this]{return run_basic_test(handler, m_worker);});

    m_worker->get_future().wait();

    run_basic_test_checks(handler);
}

//-----------------------------------------------------------------------------

void do_nothing()
{
}

//-----------------------------------------------------------------------------

void run_from_outside_test(TestHandler& _handler, core::thread::worker::sptr _worker)
{
    //waiting for worker_qt to start
    _worker->post_task<void>([]{return do_nothing();}).wait();

    run_basic_test(_handler, _worker);
}

//-----------------------------------------------------------------------------

void worker_qt_test::postFromOutsideTest()
{
    TestHandler handler;

    std::thread test_thread([&handler, this]{return run_from_outside_test(handler, m_worker);});

    m_worker->get_future().wait();

    run_basic_test_checks(handler);

    test_thread.join();
}

//-----------------------------------------------------------------------------

static CppUnit::Exception exception;

//------------------------------------------------------------------------------

static inline void qt_test(const std::function<void()>& _f)
{
    try
    {
        _f();
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

void run_basic_timer_test(
    const TestHandler& _handler,
    const core::thread::timer::sptr& _timer,
    core::thread::timer::time_duration_t
    /*unused*/
)
{
    _timer->start();

    qt_test(
        [&]
            {
                CPPUNIT_ASSERT(_timer->is_running());
                CPPUNIT_ASSERT(_handler.m_threadCheckOk);
                CPPUNIT_ASSERT_EQUAL(0, _handler.m_step);
            });
}

//-----------------------------------------------------------------------------

void one_shot_basic_timer_test(
    int& _i,
    TestHandler& _handler,
    const core::thread::timer::sptr& _timer,
    core::thread::timer::time_duration_t /*unused*/,
    const core::thread::worker::sptr& _worker
)
{
    _handler.nextStepNoSleep();

    qt_test(
        [&]
            {
                CPPUNIT_ASSERT(_timer->is_running());
                CPPUNIT_ASSERT(_handler.m_threadCheckOk);
                CPPUNIT_ASSERT_EQUAL(_i, _handler.m_step);
            });

    if(++_i == 50)
    {
        _timer->stop();

        qt_test(
            [&]
                {
                    CPPUNIT_ASSERT(!_timer->is_running());
                    CPPUNIT_ASSERT(_handler.m_threadCheckOk);
                    CPPUNIT_ASSERT_EQUAL(49, _handler.m_step);
                });
        _worker->post([]{return QApplication::quit();});
    }
}

//-----------------------------------------------------------------------------

void worker_qt_test::basicTimerTest()
{
    TestHandler handler;
    handler.set_workerId(m_worker->get_thread_id());

    core::thread::timer::sptr timer = m_worker->create_timer();

    core::thread::timer::time_duration_t duration = std::chrono::milliseconds(10);

    int i = 1;
    timer->set_function(
        [&i, &handler, &timer, duration, this]
            {
                return one_shot_basic_timer_test(i, handler, timer, duration, m_worker);
            });
    timer->set_duration(duration);

    CPPUNIT_ASSERT(!timer->is_running());
    CPPUNIT_ASSERT(handler.m_threadCheckOk);
    CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

    m_worker->post([&handler, &timer, duration]{return run_basic_timer_test(handler, timer, duration);});

    core::thread::worker::future_t future = m_worker->get_future();
    future.wait();

    CPPUNIT_ASSERT_EQUAL(0, std::any_cast<int>(future.get()));
}

//-----------------------------------------------------------------------------

} //namespace ut

} //namespace sight::ui::qt
