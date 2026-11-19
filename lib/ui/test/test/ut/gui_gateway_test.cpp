/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <ui/test/detail/gui_gateway.hpp>
#include <ui/test/qt_application.hpp>

#include <doctest/doctest.h>

#include <QTimer>

#include <array>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

namespace sight::ui::test::detail
{

namespace
{

struct fixture
{
    sight::ui::test::qt_arguments arguments {"ui_test_ut", sight::ui::test::qt_platform::system};
    QCoreApplication application {arguments.argc(), arguments.argv()};
    gui_gateway gateway {application};
};

} // namespace

//------------------------------------------------------------------------------

template<typename F>
static void run_scenario(fixture& _fixture, F&& _scenario)
{
    std::exception_ptr exception;
    std::thread scenario(
        [&]
            {
        try
        {
            std::forward<F>(_scenario)();
        }
        catch(...)
        {
            exception = std::current_exception();
        }

        // Posted: quit() does nothing when the loop has not started yet, and exec() would then never return.
        QMetaObject::invokeMethod(
            &_fixture.application,
            []{QCoreApplication::quit();},
            Qt::QueuedConnection
        );
            });

    QCoreApplication::exec();
    scenario.join();

    if(exception)
    {
        std::rethrow_exception(exception);
    }
}

//------------------------------------------------------------------------------

static gui_deadline deadline_after(std::chrono::milliseconds _duration)
{
    return std::chrono::steady_clock::now() + _duration;
}

//------------------------------------------------------------------------------

/// The deadline of one operation: it only bounds a hang, which a loaded machine never reaches by accident, and a
/// passing test does not wait for it.
static gui_deadline operation_deadline()
{
    return deadline_after(std::chrono::seconds(30));
}

//------------------------------------------------------------------------------

/// Runs _attempt with longer and longer deadlines until its call starts in time: a loaded machine can start a call
/// after a short deadline, and a call that never started says nothing about a running one. The next call waits for
/// the attempt's call, which has then finished or been cancelled.
template<typename F>
static void until_the_call_starts(const gui_gateway& _gateway, const bool& _call_started, F _attempt)
{
    static constexpr std::array<std::chrono::milliseconds, 4> s_DURATIONS {
        std::chrono::milliseconds(200),
        std::chrono::seconds(1),
        std::chrono::seconds(5),
        std::chrono::seconds(30),
    };
    for(const std::chrono::milliseconds duration : s_DURATIONS)
    {
        _attempt(deadline_after(duration));
        _gateway.call_until(operation_deadline(), []{});
        if(_call_started)
        {
            return;
        }
    }

    throw std::runtime_error("the GUI call never started before its deadline");
}

TEST_SUITE("sight::ui::test::gui_gateway")
{
    TEST_CASE_FIXTURE(fixture, "runs_calls_and_returns_values")
    {
        int result         = 0;
        bool void_call_ran = false;

        run_scenario(
            *this,
            [&]
            {
                result = gateway.call_until(operation_deadline(), []{return 42;});
                gateway.call_until(operation_deadline(), [&]{void_call_ran = true;});
            });

        CHECK(result == 42);
        CHECK(void_call_ran);
    }

    TEST_CASE_FIXTURE(fixture, "rejects_calls_from_the_application_thread")
    {
        CHECK_THROWS_AS(
            gateway.call_until(operation_deadline(), []{}),
            std::logic_error
        );
    }

    TEST_CASE_FIXTURE(fixture, "transports_exceptions_from_the_application_thread")
    {
        bool exception_received = false;

        run_scenario(
            *this,
            [&]
            {
                try
                {
                    gateway.call_until(
                        operation_deadline(),
                        []{throw std::runtime_error("GUI failure");});
                }
                catch(const std::runtime_error& error)
                {
                    exception_received = std::string(error.what()) == "GUI failure";
                }
            });

        CHECK(exception_received);
    }

    TEST_CASE_FIXTURE(fixture, "cancels_calls_that_have_not_started")
    {
        std::mutex mutex;
        std::condition_variable changed;
        bool release_blocking_call = false;
        bool action_ran            = false;
        bool timed_out             = false;

        QMetaObject::invokeMethod(
            &application,
            [&]
            {
                std::unique_lock lock(mutex);
                changed.wait(lock, [&]{return release_blocking_call;});
            },
            Qt::QueuedConnection
        );

        run_scenario(
            *this,
            [&]
            {
                try
                {
                    gateway.call_until(deadline_after(std::chrono::milliseconds(150)), [&]{action_ran = true;});
                }
                catch(const std::runtime_error&)
                {
                    timed_out = true;
                }

                {
                    std::scoped_lock lock(mutex);
                    release_blocking_call = true;
                }

                changed.notify_all();
            });

        CHECK(timed_out);
        CHECK_FALSE(action_ran);
    }

    TEST_CASE_FIXTURE(fixture, "retains_running_calls_after_timeout")
    {
        std::mutex mutex;
        std::condition_variable changed;
        bool call_started   = false;
        bool release_call   = false;
        bool call_timed_out = false;
        bool call_finished  = false;

        run_scenario(
            *this,
            [&]
            {
                until_the_call_starts(
                    gateway,
                    call_started,
                    [&](gui_deadline _deadline)
                {
                    call_started   = false;
                    release_call   = false;
                    call_timed_out = false;
                    call_finished  = false;
                    try
                    {
                        // Released only once the caller has returned: the call is still running at the timeout.
                        gateway.call_until(
                            _deadline,
                            [&]
                        {
                            std::unique_lock lock(mutex);
                            call_started = true;
                            changed.wait(lock, [&]{return release_call;});
                            call_finished = true;
                        });
                    }
                    catch(const std::runtime_error&)
                    {
                        call_timed_out = true;
                    }

                    {
                        std::scoped_lock lock(mutex);
                        release_call = true;
                    }

                    changed.notify_all();
                });
            });

        CHECK(call_started);
        CHECK(call_timed_out);
        CHECK(call_finished);
    }

    TEST_CASE_FIXTURE(fixture, "times_out_when_running_calls_finish_after_deadline")
    {
        bool call_started   = false;
        bool call_timed_out = false;
        bool call_finished  = false;

        run_scenario(
            *this,
            [&]
            {
                until_the_call_starts(
                    gateway,
                    call_started,
                    [&](gui_deadline _deadline)
                {
                    call_started   = false;
                    call_timed_out = false;
                    call_finished  = false;
                    try
                    {
                        gateway.call_until(
                            _deadline,
                            [&, _deadline]
                        {
                            call_started = true;
                            std::this_thread::sleep_until(_deadline + std::chrono::milliseconds(100));
                            call_finished = true;
                        });
                    }
                    catch(const std::runtime_error&)
                    {
                        call_timed_out = true;
                    }
                });
            });

        CHECK(call_started);
        CHECK(call_timed_out);
        CHECK(call_finished);
    }

    TEST_CASE_FIXTURE(fixture, "fails_pending_calls_when_the_application_event_loop_stops")
    {
        std::mutex mutex;
        std::condition_variable changed;
        bool loop_blocked   = false;
        bool scenario_calls = false;
        bool stopped_error  = false;
        bool action_ran     = false;

        // The scenario queues its call only once the loop runs the blocking call, which stops the loop when the
        // scenario is about to call. Qt does not deliver an event posted during the pass that runs the blocking call
        // in that pass (sendPostedEvents), and quit() ends the loop after it: the call is never run, whether it was
        // queued before the loop stopped or after. Either way it must fail fast, not at its deadline.
        QMetaObject::invokeMethod(
            &application,
            [&]
            {
                std::unique_lock lock(mutex);
                loop_blocked = true;
                changed.notify_all();
                changed.wait(lock, [&]{return scenario_calls;});
                QCoreApplication::quit();
            },
            Qt::QueuedConnection
        );

        const gui_deadline deadline = deadline_after(std::chrono::minutes(10));
        std::thread scenario(
            [&]
                {
            {
                std::unique_lock lock(mutex);
                changed.wait(lock, [&]{return loop_blocked;});
                scenario_calls = true;
            }

            changed.notify_all();
            try
            {
                gateway.call_until(deadline, [&]{action_ran = true;});
            }
            catch(const std::runtime_error& error)
            {
                stopped_error = std::string(error.what()) == "the application event loop has stopped";
            }
                });

        QCoreApplication::exec();
        scenario.join();

        CHECK(stopped_error);
        CHECK_FALSE(action_ran);
        // A loaded machine does not reach a deadline 10 minutes away by accident.
        CHECK(std::chrono::steady_clock::now() < deadline - std::chrono::minutes(5));
    }

    TEST_CASE_FIXTURE(fixture, "rejects_calls_after_the_application_event_loop_stops")
    {
        bool stopped_error = false;
        bool action_ran    = false;

        QTimer::singleShot(0, &application, []{QCoreApplication::quit();});
        QCoreApplication::exec();

        std::thread scenario(
            [&]
                {
            try
            {
                gateway.call_until(operation_deadline(), [&]{action_ran = true;});
            }
            catch(const std::runtime_error& error)
            {
                stopped_error = std::string(error.what()) == "the application event loop has stopped";
            }
                });
        scenario.join();

        CHECK(stopped_error);
        CHECK_FALSE(action_ran);
    }
} // TEST_SUITE

} // namespace sight::ui::test::detail
