/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#include <thread>

namespace sight::core::thread
{

//------------------------------------------------------------------------------

std::size_t worker_thread(SPTR(boost::asio::io_service)io_service)
{
    std::size_t res = io_service->run();
    return res;
}

/**
 * @brief Private implementation of core::thread::worker using boost::asio.
 */
class worker_asio : public core::thread::worker
{
public:

    using ioservice_type = boost::asio::io_service;
    using work_type      = boost::asio::io_service::work;
    using work_ptr_type  = std::shared_ptr<work_type>;
    using thread_type    = std::thread;

    worker_asio();

    ~worker_asio() override;

    void stop() override;

    void post(task_t handler) override;

    thread_id_t get_thread_id() const override;

    SPTR(core::thread::timer) create_timer() override;

    void process_tasks() override;

    void process_tasks(period_t maxtime) override;

protected:

    /// Copy constructor forbidden
    worker_asio(const worker_asio&);

    /// Copy operator forbidden
    worker_asio& operator=(const worker_asio&);

private:

    /// Class provides functionality to manipulate asynchronous tasks.
    SPTR(ioservice_type) m_io_service;

    /// Class to inform the io_service when it has work to do.
    work_ptr_type m_work;

    /// Thread created and managed by the worker.
    SPTR(thread_type) m_thread;

    /// To avoid race conditions when calling stop()
    std::recursive_mutex m_stop_mutex;
};

//------------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using boost::asio.
 */
class timer_asio : public core::thread::timer
{
public:

    SIGHT_DECLARE_CLASS(timer_asio, core::thread::timer);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Constructs a TimerAsio from given io_service.
     */
    explicit timer_asio(boost::asio::io_service& io_srv);

    ~timer_asio() override;

    /// Starts or restarts the timer.
    void start() override;

    /// Stops the timer and cancel all pending operations.
    void stop() override;

    /// Sets time duration.
    void set_duration(time_duration_t duration) override;

    /// Returns if the timer mode is 'one shot'.
    bool is_one_shot() const override
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_one_shot;
    }

    /// Sets timer mode.
    void set_one_shot(bool one_shot) override
    {
        core::mt::scoped_lock lock(m_mutex);
        m_one_shot = one_shot;
    }

    /// Returns true if the timer is currently running.
    bool is_running() const override
    {
        core::mt::scoped_lock lock(m_mutex);
        return m_running;
    }

protected:

    void cancel_no_lock();
    void rearm_no_lock(time_duration_t duration);

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

worker_asio::worker_asio() :
    m_io_service(std::make_shared<ioservice_type>()),
    m_work(std::make_shared<work_type>(*m_io_service))
{
    std::packaged_task<core::thread::worker::exit_return_type()> task([this](auto&& ...)
            {
                                                                      return worker_thread(m_io_service);
            });
    std::future<core::thread::worker::exit_return_type> future = task.get_future();

    m_thread = std::make_shared<thread_type>(std::move(task));

    m_future = std::move(future);
}

//------------------------------------------------------------------------------

worker_asio::~worker_asio()
{
    std::unique_lock<std::recursive_mutex> lock(m_stop_mutex);

    SIGHT_ASSERT(
        "Worker must be properly stopped. Try to call stop() from the caller thread before.",
        !m_thread->joinable()
    );
}

//------------------------------------------------------------------------------

void worker_asio::stop()
{
    // stop() is also called in the destructor, so we need to put a critical section here
    std::unique_lock<std::recursive_mutex> lock(m_stop_mutex);

    SIGHT_ASSERT("Thread is not joinable", m_thread->joinable());
    SIGHT_ASSERT(
        "Can not destroy a thread while running it. Try to call stop() from the caller thread before.",
        m_thread->get_id() != core::thread::get_current_thread_id()
    );

    m_work.reset();
    m_thread->join();
}

//------------------------------------------------------------------------------

SPTR(core::thread::timer) worker_asio::create_timer()
{
    return std::make_shared<timer_asio>(*m_io_service);
}

//------------------------------------------------------------------------------

void worker_asio::post(task_t handler)
{
    m_io_service->post(handler);
}

//------------------------------------------------------------------------------

thread_id_t worker_asio::get_thread_id() const
{
    return m_thread->get_id();
}

//------------------------------------------------------------------------------

void worker_asio::process_tasks()
{
    m_io_service->poll();
}

//------------------------------------------------------------------------------

void worker_asio::process_tasks(period_t maxtime)
{
    core::time_stamp time_stamp;
    time_stamp.set_life_period(maxtime);
    time_stamp.modified();
    while(time_stamp.period_expired())
    {
        m_io_service->poll_one();
    }
}

// ---------- Worker ----------
SPTR(worker) worker::make()
{
    return std::make_shared<worker_asio>();
}

// ---------- Timer private implementation ----------

timer_asio::timer_asio(boost::asio::io_service& io_srv) :
    m_timer(io_srv),
    m_duration(std::chrono::seconds(1))
{
}

timer_asio::~timer_asio()
= default;

//------------------------------------------------------------------------------

void timer_asio::set_duration(time_duration_t duration)
{
    core::mt::scoped_lock lock(m_mutex);
    m_duration = duration;
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

    static void call(const boost::system::error_code& error, timer_asio::sptr _timer)
    {
        if(!error)
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

void timer_asio::rearm_no_lock(time_duration_t duration)
{
    this->cancel_no_lock();
    boost::posix_time::time_duration d =
        boost::posix_time::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
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
