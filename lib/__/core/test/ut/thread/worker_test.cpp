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

#include <core/exception.hpp>
#include <core/thread/timer.hpp>
#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

//-----------------------------------------------------------------------------

namespace
{

struct test_handler
{
    test_handler() :
        m_constructor_thread_id(sight::core::thread::get_current_thread_id())
    {
    }

    //------------------------------------------------------------------------------

    void next_step()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->next_step_no_sleep();
    }

    //------------------------------------------------------------------------------

    void next_step_no_sleep()
    {
        m_thread_check_ok = m_thread_check_ok.load()
                            && (m_constructor_thread_id != sight::core::thread::get_current_thread_id())
                            && (m_worker_thread_id == sight::core::thread::get_current_thread_id());

        ++m_step;
    }

    //------------------------------------------------------------------------------

    void set_worker_id(sight::core::thread::thread_id_t _id)
    {
        m_worker_thread_id = _id;
    }

    std::atomic_int m_step {0};
    std::atomic_bool m_thread_check_ok {true};
    sight::core::thread::thread_id_t m_constructor_thread_id;
    sight::core::thread::thread_id_t m_worker_thread_id;
};

//------------------------------------------------------------------------------

/// Records when a timer fires, and on which thread.
class tick_recorder
{
public:

    using time_point = std::chrono::steady_clock::time_point;

    explicit tick_recorder(sight::core::thread::thread_id_t _worker_thread_id) :
        m_worker_thread_id(_worker_thread_id)
    {
    }

    //------------------------------------------------------------------------------

    void tick()
    {
        const std::scoped_lock lock(m_mutex);
        m_on_worker_thread = m_on_worker_thread && sight::core::thread::get_current_thread_id() == m_worker_thread_id;
        m_times.push_back(std::chrono::steady_clock::now());
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::size_t count() const
    {
        const std::scoped_lock lock(m_mutex);
        return m_times.size();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool on_worker_thread() const
    {
        const std::scoped_lock lock(m_mutex);
        return m_on_worker_thread;
    }

    //------------------------------------------------------------------------------

    /// True when no tick came earlier than its period allows. The timer is rearmed when it fires, so the n-th fire
    /// after _start is never before _start + n x _period: a callback run late shifts its record, never the next fire,
    /// so only this cumulative bound holds under load, not one on the gap between two records. Ticks are counted from
    /// _first, the _late first ones of them being allowed early (a tick of a previous run still in flight); a tenth
    /// of a period per tick is left to the platform timer's resolution.
    [[nodiscard]] bool never_early(
        std::chrono::steady_clock::duration _period,
        time_point _start,
        std::size_t _first = 0,
        std::size_t _late  = 0
    ) const
    {
        const std::scoped_lock lock(m_mutex);
        for(std::size_t index = _first + _late ; index < m_times.size() ; ++index)
        {
            const auto fires = static_cast<std::int64_t>(index - _first - _late + 1);
            if(m_times[index] - _start < _period * fires * 9 / 10)
            {
                return false;
            }
        }

        return true;
    }

private:

    mutable std::mutex m_mutex;
    std::vector<time_point> m_times;
    bool m_on_worker_thread {true};
    const sight::core::thread::thread_id_t m_worker_thread_id;
};

//------------------------------------------------------------------------------

/// Stops its timers still alive, then joins the worker, on every exit of a test, a failed REQUIRE included: a running
/// timer would call into recorders already destroyed, and a worker still running asserts on destruction. Declare the
/// recorders before it, so that they outlive it. It never keeps a timer alive, so a test can destroy one.
class timer_scope
{
public:

    timer_scope() = default;

    timer_scope(const timer_scope&)            = delete;
    timer_scope(timer_scope&&)                 = delete;
    timer_scope& operator=(const timer_scope&) = delete;
    timer_scope& operator=(timer_scope&&)      = delete;

    //------------------------------------------------------------------------------

    ~timer_scope()
    {
        for(const auto& weak_timer : m_timers)
        {
            if(const auto timer = weak_timer.lock(); timer)
            {
                timer->stop();
            }
        }

        m_worker->stop();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] sight::core::thread::thread_id_t thread_id() const
    {
        return m_worker->get_thread_id();
    }

    //------------------------------------------------------------------------------

    /// A periodic timer of this worker that runs _function.
    std::shared_ptr<sight::core::thread::timer> create_timer(
        std::function<void()> _function,
        std::chrono::steady_clock::duration _period
)
    {
        auto timer = m_worker->create_timer();
        timer->set_function(std::move(_function));
        timer->set_duration(std::chrono::duration_cast<std::chrono::milliseconds>(_period));
        m_timers.push_back(timer);
        return timer;
    }

    //------------------------------------------------------------------------------

    /// A periodic timer of this worker that records its ticks in _ticks.
    std::shared_ptr<sight::core::thread::timer> create_timer(
        tick_recorder& _ticks,
        std::chrono::steady_clock::duration _period
)
    {
        return create_timer([&_ticks]{_ticks.tick();}, _period);
    }

private:

    sight::core::thread::worker::sptr m_worker {sight::core::thread::worker::make()};
    std::vector<std::weak_ptr<sight::core::thread::timer> > m_timers;
};

} // namespace

//------------------------------------------------------------------------------

/// Waits for a condition, never for a delay: a loaded machine only makes it longer, until a 30 s deadline.
static bool wait_until(const std::function<bool()>& _condition)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
    while(!_condition())
    {
        if(std::chrono::steady_clock::now() >= deadline)
        {
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return true;
}

TEST_SUITE("sight::core::thread::worker")
{
//-----------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        {
            auto worker = sight::core::thread::worker::make();

            test_handler handler;
            handler.set_worker_id(worker->get_thread_id());
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step();});

            worker->stop();
            CHECK_EQ(3, handler.m_step.load());
            CHECK_EQ(true, handler.m_thread_check_ok.load());
        }

        {
            auto worker = sight::core::thread::worker::make();

            test_handler handler;
            handler.set_worker_id(worker->get_thread_id());
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});
            worker->post([object_ptr = &handler](auto&& ...){object_ptr->next_step_no_sleep();});

            worker->stop();
            CHECK_EQ(3, handler.m_step.load());
            CHECK_EQ(true, handler.m_thread_check_ok.load());
        }
    }

//-----------------------------------------------------------------------------

    // A barrier is ready only once the tasks posted before it have run.
    TEST_CASE("post_barrier")
    {
        auto worker = sight::core::thread::worker::make();

        // The first task holds the worker until the test releases it.
        std::promise<void> release;
        worker->post([held = release.get_future().share()](auto&& ...){held.wait();});

        std::atomic_bool second_ran {false};
        worker->post([&second_ran](auto&& ...){second_ran = true;});

        auto barrier = worker->post_barrier();
        CHECK(barrier.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout);

        release.set_value();
        barrier.get();
        CHECK(second_ran);

        worker->stop();
    }

//-----------------------------------------------------------------------------

    // wait_barrier() returns only once the tasks posted before it have run.
    TEST_CASE("wait_barrier")
    {
        auto worker = sight::core::thread::worker::make();

        // Slow enough that a wait_barrier() returning at once would find the flag still down.
        std::atomic_bool ran {false};
        worker->post(
            [&ran](auto&& ...)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ran = true;
        });

        worker->wait_barrier();
        CHECK(ran);

        worker->stop();
    }

//-----------------------------------------------------------------------------

    // The default worker has the same barriers, without looking it up.
    TEST_CASE("default_barrier")
    {
        std::atomic_bool ran {false};
        sight::core::thread::get_default_worker()->post([&ran](auto&& ...){ran = true;});
        sight::core::thread::wait_default_barrier();
        CHECK(ran);

        std::atomic_bool second_ran {false};
        sight::core::thread::get_default_worker()->post([&second_ran](auto&& ...){second_ran = true;});
        sight::core::thread::post_default_barrier().get();
        CHECK(second_ran);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("timer")
    {
        using namespace std::chrono_literals;
        constexpr auto period = 20ms;

        // One recorder per phase, declared before the scope that stops their timers: a tick still in flight from a
        // phase never lands in the next one.
        std::optional<tick_recorder> periodic;
        std::optional<tick_recorder> restarted;
        std::optional<tick_recorder> one_shot;
        timer_scope timers;
        periodic.emplace(timers.thread_id());
        restarted.emplace(timers.thread_id());
        one_shot.emplace(timers.thread_id());

        // Periodic: a loaded machine delays ticks but never brings one early.
        const auto timer = timers.create_timer(*periodic, period);
        CHECK_FALSE(timer->is_running());
        const auto started = std::chrono::steady_clock::now();
        timer->start();
        CHECK(timer->is_running());
        REQUIRE(wait_until([&periodic]{return periodic->count() >= 10;}));
        CHECK(periodic->on_worker_thread());
        CHECK(periodic->never_early(period, started));

        // Stopped: a tick already due may still run, never more.
        timer->stop();
        CHECK_FALSE(timer->is_running());
        const std::size_t at_stop = periodic->count();
        std::this_thread::sleep_for(period * 5);
        CHECK(periodic->count() <= at_stop + 1);

        // Started again after a stop; a tick of the stopped run may still come first.
        timer->set_function([&restarted]{restarted->tick();});
        const auto restarted_at = std::chrono::steady_clock::now();
        timer->start();
        REQUIRE(wait_until([&restarted]{return restarted->count() >= 5;}));
        CHECK(restarted->never_early(period, restarted_at, 0, 1));

        // A longer duration applies from the next rearm. The duration is read before it was set by at most the call
        // being run when it was set: counted once set, the tick of that call and the one it armed may keep the old
        // duration, and every later one comes at least one new duration after the one before, from when it was set.
        constexpr auto longer_period = 100ms;
        const auto changed           = std::chrono::steady_clock::now();
        timer->set_duration(longer_period);
        const std::size_t at_change = restarted->count();
        REQUIRE(wait_until([&restarted, at_change]{return restarted->count() >= at_change + 5;}));
        CHECK(restarted->never_early(longer_period, changed, at_change + 2));

        // One shot: a single tick, then the timer stops by itself.
        const auto shot = timers.create_timer(*one_shot, 10ms);
        shot->set_one_shot(true);
        const auto shot_at = std::chrono::steady_clock::now();
        shot->start();
        REQUIRE(wait_until([&shot]{return !shot->is_running();}));
        std::this_thread::sleep_for(10ms * 5);
        CHECK(one_shot->count() == 1);
        CHECK(one_shot->never_early(10ms, shot_at));

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
            const auto duration = 10ms;
            auto timer2         = timers.create_timer(
                [duration]
            {
                std::this_thread::sleep_for(duration * 90);
            },
                duration
            );

            timer2->start();
            std::this_thread::sleep_for(duration * 2);
            timer2->stop();

            timer2.reset();
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("registry")
    {
        {
            CHECK_EQ(sight::core::thread::worker::sptr(), sight::core::thread::get_worker("worker1"));

            auto worker1 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker1", worker1);

            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));

            auto worker2 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker2", worker2);

            auto worker3 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker3", worker3);

            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));
            CHECK_EQ(worker2, sight::core::thread::get_worker("worker2"));
            CHECK_EQ(worker3, sight::core::thread::get_worker("worker3"));

            sight::core::thread::remove_worker("worker2");
            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));
            CHECK_EQ(sight::core::thread::worker::sptr(), sight::core::thread::get_worker("worker2"));
            CHECK_EQ(worker3, sight::core::thread::get_worker("worker3"));

            auto worker4 = sight::core::thread::worker::make();
            sight::core::thread::add_worker("worker2", worker4);
            CHECK_EQ(worker1, sight::core::thread::get_worker("worker1"));
            CHECK_EQ(worker4, sight::core::thread::get_worker("worker2"));
            CHECK_EQ(worker3, sight::core::thread::get_worker("worker3"));

            sight::core::thread::remove_worker("worker1");
            sight::core::thread::remove_worker("worker2");
            sight::core::thread::remove_worker("worker3");
        }

        {
            auto init_worker = sight::core::thread::get_default_worker();
            CHECK(init_worker != nullptr);
        }
        {
            auto worker1 = sight::core::thread::worker::make();
            sight::core::thread::set_default_worker(worker1);
            CHECK_EQ(worker1, sight::core::thread::get_default_worker());

            auto worker2 = sight::core::thread::worker::make();

            // simulates that worker1 is still in use because of its sptr above
            CHECK_THROWS_AS(sight::core::thread::set_default_worker(worker2), sight::core::exception);
            worker2->stop();
        }
        {
            auto default_worker = sight::core::thread::worker::make();
            sight::core::thread::set_default_worker(default_worker);
            CHECK_EQ(default_worker, sight::core::thread::get_default_worker());
        }

        CHECK_THROWS_AS(
            sight::core::thread::set_default_worker(sight::core::thread::worker::sptr()),
            sight::core::exception
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("thread_name")
    {
        // Current thread (implicit api)
        {
            const auto thread_name     = sight::core::thread::get_thread_name();
            const auto new_thread_name = thread_name + "_pf0";
            sight::core::thread::set_thread_name(new_thread_name);
            const auto modified_thread_name = sight::core::thread::get_thread_name();
            CHECK_EQ(new_thread_name, modified_thread_name);

            sight::core::thread::set_thread_name(thread_name);
        }

        // Current thread (implicit api) and thread name too long
        {
            const auto thread_name = sight::core::thread::get_thread_name();

            const auto desired_thread_name  = std::string("desired_thread_name_too_long");
            const auto expected_thread_name = std::string("desired_thread_");

            sight::core::thread::set_thread_name(desired_thread_name);
            const auto current_thread_name = sight::core::thread::get_thread_name();
            CHECK_EQ(expected_thread_name, current_thread_name);

            sight::core::thread::set_thread_name(thread_name);
        }

        // Current thread (explicit api)
        {
            const auto current_native_id = sight::core::thread::get_current_thread_native_id();

            const auto thread_name     = sight::core::thread::get_thread_name(current_native_id);
            const auto new_thread_name = thread_name + "_pf1";
            sight::core::thread::set_thread_name(new_thread_name, current_native_id);
            const auto modified_thread_name = sight::core::thread::get_thread_name(current_native_id);
            CHECK_EQ(new_thread_name, modified_thread_name);
        }
    }
} // TEST_SUITE
