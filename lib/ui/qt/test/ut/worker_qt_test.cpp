/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/thread/timer.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <ui/qt/app.hpp>
#include <ui/qt/worker_qt.hpp>

#include <doctest/doctest.h>

#include <QApplication>
#include <QSharedPointer>
#include <QTimer>

#include <array>
#include <functional>
#include <thread>

namespace sight::ui::qt
{

// Defined in worker_qt.cpp
class worker_qt;

struct test_handler
{
    test_handler()
    = default;

    //------------------------------------------------------------------------------

    void next_step()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->next_step_no_sleep();
    }

    //------------------------------------------------------------------------------

    void next_step_no_sleep()
    {
        m_thread_check_ok &= (m_worker_thread_id == core::thread::get_current_thread_id());
        ++m_step;
    }

    //------------------------------------------------------------------------------

    void set_worker_id(core::thread::thread_id_t _id)
    {
        m_worker_thread_id = _id;
    }

    int m_step {0};
    bool m_thread_check_ok {true};
    core::thread::thread_id_t m_worker_thread_id;
};

namespace
{

struct fixture
{
    fixture()
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

        CHECK(qApp == nullptr);
        std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
            [](int& _argc, char** _argv)
            {
#if QT_VERSION < 0x050000
                bool guiEnabled = false;
#else
                bool gui_enabled = true;
#endif
                return QSharedPointer<sight::ui::qt::app>(new sight::ui::qt::app(_argc, _argv, gui_enabled));
            };
        m_worker = sight::ui::qt::get_qt_worker(argc, argv.data(), callback, "", "");

        CHECK(qApp != nullptr);
    }

    ~fixture()
    {
        m_worker.reset();
        CHECK(qApp == nullptr);
    }

    sight::core::thread::worker::sptr m_worker;
};

} // namespace

//-----------------------------------------------------------------------------

TEST_SUITE("sight::ui::qt::worker")
{
//-----------------------------------------------------------------------------

    static void run_basic_test(test_handler& _handler, sight::core::thread::worker::sptr _worker)
    {
        _handler.set_worker_id(_worker->get_thread_id());
        _worker->post([&_handler]{_handler.next_step();});
        _worker->post([&_handler]{_handler.next_step();});
        _worker->post([&_handler]{_handler.next_step();});

        _worker->post([]{QApplication::quit();});
    }

    //------------------------------------------------------------------------------

    static void run_basic_test_checks(const test_handler& _handler)
    {
        CHECK_EQ(3, _handler.m_step);
        CHECK_EQ(true, _handler.m_thread_check_ok);
    }

    TEST_CASE_FIXTURE(fixture, "basic")
    {
        test_handler handler;

        run_basic_test(handler, m_worker);

        m_worker->get_future().wait();

        run_basic_test_checks(handler);
    }

    TEST_CASE_FIXTURE(fixture, "post_from_inside")
    {
        test_handler handler;

        m_worker->post([&handler, this]{run_basic_test(handler, m_worker);});

        m_worker->get_future().wait();

        run_basic_test_checks(handler);
    }

    //------------------------------------------------------------------------------

    static void do_nothing()
    {
    }

    //------------------------------------------------------------------------------

    static void run_from_outside_test(test_handler& _handler, sight::core::thread::worker::sptr _worker)
    {
        //waiting for worker_qt to start
        _worker->post_task<void>([]{do_nothing();}).wait();

        run_basic_test(_handler, _worker);
    }

    TEST_CASE_FIXTURE(fixture, "post_from_outside")
    {
        test_handler handler;

        std::thread test_thread([&handler, this]{run_from_outside_test(handler, m_worker);});

        m_worker->get_future().wait();

        run_basic_test_checks(handler);

        test_thread.join();
    }

//-----------------------------------------------------------------------------

    static inline void qt_test(const std::function<void()>& _f)
    {
        try
        {
            _f();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            QApplication::exit(1);
            return;
        }
    }

    //------------------------------------------------------------------------------

    static void run_basic_timer_test(
        const test_handler& _handler,
        const sight::core::thread::timer::sptr& _timer,
        sight::core::thread::timer::time_duration_t
        /*unused*/
)
    {
        _timer->start();

        qt_test(
            [&]
            {
                CHECK(_timer->is_running());
                CHECK(_handler.m_thread_check_ok);
                CHECK_EQ(0, _handler.m_step);
            });
    }

    //------------------------------------------------------------------------------

    static void one_shot_basic_timer_test(
        int& _i,
        test_handler& _handler,
        const sight::core::thread::timer::sptr& _timer,
        sight::core::thread::timer::time_duration_t /*unused*/,
        const sight::core::thread::worker::sptr& _worker
)
    {
        _handler.next_step_no_sleep();

        qt_test(
            [&]
            {
                CHECK(_timer->is_running());
                CHECK(_handler.m_thread_check_ok);
                CHECK_EQ(_i, _handler.m_step);
            });

        if(++_i == 50)
        {
            _timer->stop();

            qt_test(
                [&]
                {
                    CHECK(!_timer->is_running());
                    CHECK(_handler.m_thread_check_ok);
                    CHECK_EQ(49, _handler.m_step);
                });
            _worker->post([]{QApplication::quit();});
        }
    }

    TEST_CASE_FIXTURE(fixture, "basic_timer")
    {
        test_handler handler;
        handler.set_worker_id(m_worker->get_thread_id());

        sight::core::thread::timer::sptr timer = m_worker->create_timer();

        sight::core::thread::timer::time_duration_t duration = std::chrono::milliseconds(10);

        int i = 1;
        timer->set_function(
            [&i, &handler, &timer, duration, this]
            {
                one_shot_basic_timer_test(i, handler, timer, duration, m_worker);
            });
        timer->set_duration(duration);

        CHECK(!timer->is_running());
        CHECK(handler.m_thread_check_ok);
        CHECK_EQ(0, handler.m_step);

        m_worker->post([&handler, &timer, duration]{run_basic_timer_test(handler, timer, duration);});

        sight::core::thread::worker::future_t future = m_worker->get_future();
        future.wait();

        CHECK_EQ(0, std::any_cast<int>(future.get()));
    }
} // TEST_SUITE

} // namespace sight::ui::qt
