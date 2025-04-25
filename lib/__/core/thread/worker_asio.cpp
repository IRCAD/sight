/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "core/thread/timer.hpp"
#include "core/thread/worker.hpp"

#include <core/time_stamp.hpp>

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#include <thread>

namespace sight::core::thread
{

/**
 * @brief Private implementation of core::thread::worker using boost::asio.
 */
class worker_asio final : public core::thread::worker
{
public:

    worker_asio();

    ~worker_asio() final;

    void stop() final;

    void post(task_t _handler) final;

    [[nodiscard]] thread_id_t get_thread_id() const final;

    void set_thread_name(const std::string& _thread_name) final;

    SPTR(core::thread::timer) create_timer() final;

    void process_tasks() final;

    void process_tasks(period_t _maxtime) final;

protected:

    /// Copy constructor forbidden
    worker_asio(const worker_asio&);

    /// Copy operator forbidden
    worker_asio& operator=(const worker_asio&);

private:

    struct context
    {
        /// Class provides functionality to manipulate asynchronous tasks.
        boost::asio::io_context m_io_context;

        /// Class to inform the io_context when it has work to do.
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work_guard {
            m_io_context.get_executor()
        };

        /// Thread created and managed by the worker.
        std::thread m_thread;
    };

    std::shared_ptr<context> m_context {std::make_shared<context>()};
};

//------------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using boost::asio.
 */
class timer_asio final : public core::thread::timer
{
public:

    SIGHT_DECLARE_CLASS(timer_asio, core::thread::timer);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Constructs a TimerAsio from given io_context.
     */
    explicit timer_asio(boost::asio::io_context& _io_srv);

    ~timer_asio() final;

    /// Starts or restarts the timer.
    void start() final;

    /// Stops the timer and cancel all pending operations.
    void stop() final;

    /// Sets time duration.
    void set_duration(time_duration_t _duration) final;

    /// Returns if the timer mode is 'one shot'.
    bool is_one_shot() const final
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_one_shot;
    }

    /// Sets timer mode.
    void set_one_shot(bool _one_shot) final
    {
        core::mt::scoped_lock lock(m_mutex);
        m_one_shot = _one_shot;
    }

    /// Returns true if the timer is currently running.
    bool is_running() const final
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_running;
    }

protected:

    void cancel_no_lock();
    void rearm_no_lock(time_duration_t _duration);

    /// Copy constructor forbidden.
    timer_asio(const timer_asio&);

    /// Copy operator forbidden.
    timer_asio& operator=(const timer_asio&);

private:

    friend struct timer_callback;

    /// Timer object.
    boost::asio::deadline_timer m_timer;

    /// Time to wait until timer's expiration.
    time_duration_t m_duration;

    /// Timer's mode.
    bool m_one_shot {false};

    /// Timer's state.
    bool m_running {false};
};

//------------------------------------------------------------------------------

// ---------- WorkerAsio private implementation ----------

worker_asio::worker_asio()
{
    // Explicitly copy the shared context to keep it alive.
    auto work =
        [context = this->m_context](auto&& ...)
        {
            // run() is blocking while work_guard is alive.
            return context->m_io_context.run();
        };

    std::packaged_task<core::thread::worker::exit_return_type()> task(work);

    m_future            = task.get_future();
    m_context->m_thread = std::thread(std::move(task));
}

//------------------------------------------------------------------------------

worker_asio::~worker_asio()
{
    SIGHT_ASSERT(
        "Worker must be properly stopped. Try to call stop() from the caller thread before.",
        !m_context->m_thread.joinable()
    );
}

//------------------------------------------------------------------------------

void worker_asio::stop()
{
    SIGHT_ASSERT("Thread is not joinable", m_context->m_thread.joinable());
    SIGHT_ASSERT(
        "Can not destroy a thread while running it. Try to call stop() from the caller thread before.",
        m_context->m_thread.get_id() != core::thread::get_current_thread_id()
    );

    m_context->m_work_guard.reset();
    m_context->m_thread.join();
}

//------------------------------------------------------------------------------

SPTR(core::thread::timer) worker_asio::create_timer()
{
    return std::make_shared<timer_asio>(m_context->m_io_context);
}

//------------------------------------------------------------------------------

void worker_asio::post(task_t _handler)
{
    boost::asio::post(m_context->m_io_context, _handler);
}

//------------------------------------------------------------------------------

thread_id_t worker_asio::get_thread_id() const
{
    return m_context->m_thread.get_id();
}

//------------------------------------------------------------------------------

void worker_asio::set_thread_name(const std::string& _thread_name)
{
    core::thread::set_thread_name(_thread_name, m_context->m_thread.native_handle());
}

//------------------------------------------------------------------------------

void worker_asio::process_tasks()
{
    m_context->m_io_context.poll();
}

//------------------------------------------------------------------------------

void worker_asio::process_tasks(period_t _maxtime)
{
    core::time_stamp time_stamp;
    time_stamp.set_life_period(_maxtime);
    time_stamp.modified();
    while(time_stamp.period_expired())
    {
        m_context->m_io_context.poll_one();
    }
}

// ---------- Worker ----------
SPTR(worker) worker::make()
{
    return std::make_shared<worker_asio>();
}

// ---------- Timer private implementation ----------

timer_asio::timer_asio(boost::asio::io_context& _io_srv) :
    m_timer(_io_srv),
    m_duration(std::chrono::seconds(1))
{
}

timer_asio::~timer_asio() = default;

//------------------------------------------------------------------------------

void timer_asio::set_duration(time_duration_t _duration)
{
    core::mt::scoped_lock lock(m_mutex);
    m_duration = _duration;
}

//------------------------------------------------------------------------------

void timer_asio::start()
{
    core::mt::scoped_lock lock(m_mutex);
    this->rearm_no_lock(m_duration);
    m_running = true;
}

//------------------------------------------------------------------------------

void timer_asio::stop()
{
    core::mt::scoped_lock lock(m_mutex);
    if(m_running)
    {
        m_running = false;
        this->cancel_no_lock();
    }
}

//------------------------------------------------------------------------------

struct timer_callback
{
    //------------------------------------------------------------------------------

    static void call(const boost::system::error_code& _error, timer_asio::sptr _timer)
    {
        if(!_error)
        {
            timer_asio::time_duration_t duration;
            bool one_shot = false;
            {
                core::mt::scoped_lock lock(_timer->m_mutex);
                one_shot = _timer->m_one_shot;
                duration = _timer->m_duration;
            }

            if(!one_shot)
            {
                {
                    core::mt::scoped_lock lock(_timer->m_mutex);
                    if(_timer->m_running)
                    {
                        _timer->rearm_no_lock(duration);
                    }
                }
                _timer->m_function();
            }
            else
            {
                _timer->m_function();
                core::mt::scoped_lock lock(_timer->m_mutex);
                _timer->m_running = false;
            }
        }
    }
};

//------------------------------------------------------------------------------

void timer_asio::rearm_no_lock(time_duration_t _duration)
{
    this->cancel_no_lock();
    boost::posix_time::time_duration d =
        boost::posix_time::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(_duration).count());
    m_timer.expires_from_now(d);
    // NOLINTNEXTLINE(modernize-avoid-bind)
    m_timer.async_wait(boost::bind(timer_callback::call, boost::asio::placeholders::error, this->get_sptr()));
}

//------------------------------------------------------------------------------

void timer_asio::cancel_no_lock()
{
    m_timer.cancel();
}

} //namespace sight::core::thread
