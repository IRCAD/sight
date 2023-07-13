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

#include "core/thread/Timer.hpp"
#include "core/thread/Worker.hpp"

#include <core/TimeStamp.hpp>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#include <thread>

namespace sight::core::thread
{

//------------------------------------------------------------------------------

std::size_t workerThread(SPTR(boost::asio::io_service)io_service)
{
    std::size_t res = io_service->run();
    return res;
}

/**
 * @brief Private implementation of core::thread::Worker using boost::asio.
 */
class WorkerAsio : public core::thread::Worker
{
public:

    using IOServiceType = boost::asio::io_service;
    using WorkType      = boost::asio::io_service::work;
    using WorkPtrType   = std::shared_ptr<WorkType>;
    using ThreadType    = std::thread;

    WorkerAsio();

    ~WorkerAsio() override;

    void stop() override;

    void post(TaskType handler) override;

    ThreadIdType getThreadId() const override;

    SPTR(core::thread::Timer) createTimer() override;

    void processTasks() override;

    void processTasks(PeriodType maxtime) override;

protected:

    /// Copy constructor forbidden
    WorkerAsio(const WorkerAsio&);

    /// Copy operator forbidden
    WorkerAsio& operator=(const WorkerAsio&);

private:

    /// Class provides functionality to manipulate asynchronous tasks.
    SPTR(IOServiceType) m_ioService;

    /// Class to inform the io_service when it has work to do.
    WorkPtrType m_work;

    /// Thread created and managed by the worker.
    SPTR(ThreadType) m_thread;

    /// To avoid race conditions when calling stop()
    std::recursive_mutex m_stopMutex;
};

//------------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using boost::asio.
 */
class TimerAsio : public core::thread::Timer
{
public:

    SIGHT_DECLARE_CLASS(TimerAsio, core::thread::Timer);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Constructs a TimerAsio from given io_service.
     */
    explicit TimerAsio(boost::asio::io_service& ioSrv);

    ~TimerAsio() override;

    /// Starts or restarts the timer.
    void start() override;

    /// Stops the timer and cancel all pending operations.
    void stop() override;

    /// Sets time duration.
    void setDuration(TimeDurationType duration) override;

    /// Returns if the timer mode is 'one shot'.
    bool isOneShot() const override
    {
        core::mt::ScopedLock lock(m_mutex);
        return m_oneShot;
    }

    /// Sets timer mode.
    void setOneShot(bool oneShot) override
    {
        core::mt::ScopedLock lock(m_mutex);
        m_oneShot = oneShot;
    }

    /// Returns true if the timer is currently running.
    bool isRunning() const override
    {
        core::mt::ScopedLock lock(m_mutex);
        return m_running;
    }

protected:

    void cancelNoLock();
    void rearmNoLock(TimeDurationType duration);

    /// Copy constructor forbidden.
    TimerAsio(const TimerAsio&);

    /// Copy operator forbidden.
    TimerAsio& operator=(const TimerAsio&);

private:

    friend struct TimerCallback;

    /// Timer object.
    boost::asio::deadline_timer m_timer;

    /// Time to wait until timer's expiration.
    TimeDurationType m_duration;

    /// Timer's mode.
    bool m_oneShot {false};

    /// Timer's state.
    bool m_running {false};
};

//------------------------------------------------------------------------------

// ---------- WorkerAsio private implementation ----------

WorkerAsio::WorkerAsio() :
    m_ioService(std::make_shared<IOServiceType>()),
    m_work(std::make_shared<WorkType>(*m_ioService))
{
    std::packaged_task<core::thread::Worker::ExitReturnType()> task([this](auto&& ...)
            {
                                                                    return workerThread(m_ioService);
            });
    std::future<core::thread::Worker::ExitReturnType> future = task.get_future();

    m_thread = std::make_shared<ThreadType>(std::move(task));

    m_future = std::move(future);
}

//------------------------------------------------------------------------------

WorkerAsio::~WorkerAsio()
{
    std::unique_lock<std::recursive_mutex> lock(m_stopMutex);

    SIGHT_ASSERT(
        "Worker must be properly stopped. Try to call stop() from the caller thread before.",
        !m_thread->joinable()
    );
}

//------------------------------------------------------------------------------

void WorkerAsio::stop()
{
    // stop() is also called in the destructor, so we need to put a critical section here
    std::unique_lock<std::recursive_mutex> lock(m_stopMutex);

    SIGHT_ASSERT("Thread is not joinable", m_thread->joinable());
    SIGHT_ASSERT(
        "Can not destroy a thread while running it. Try to call stop() from the caller thread before.",
        m_thread->get_id() != core::thread::getCurrentThreadId()
    );

    m_work.reset();
    m_thread->join();
}

//------------------------------------------------------------------------------

SPTR(core::thread::Timer) WorkerAsio::createTimer()
{
    return std::make_shared<TimerAsio>(*m_ioService);
}

//------------------------------------------------------------------------------

void WorkerAsio::post(TaskType handler)
{
    m_ioService->post(handler);
}

//------------------------------------------------------------------------------

ThreadIdType WorkerAsio::getThreadId() const
{
    return m_thread->get_id();
}

//------------------------------------------------------------------------------

void WorkerAsio::processTasks()
{
    m_ioService->poll();
}

//------------------------------------------------------------------------------

void WorkerAsio::processTasks(PeriodType maxtime)
{
    core::TimeStamp timeStamp;
    timeStamp.setLifePeriod(maxtime);
    timeStamp.modified();
    while(timeStamp.periodExpired())
    {
        m_ioService->poll_one();
    }
}

// ---------- Worker ----------
SPTR(Worker) Worker::defaultFactory()
{
    return std::make_shared<WorkerAsio>();
}

// ---------- Timer private implementation ----------

TimerAsio::TimerAsio(boost::asio::io_service& ioSrv) :
    m_timer(ioSrv),
    m_duration(std::chrono::seconds(1))
{
}

TimerAsio::~TimerAsio()
= default;

//------------------------------------------------------------------------------

void TimerAsio::setDuration(TimeDurationType duration)
{
    core::mt::ScopedLock lock(m_mutex);
    m_duration = duration;
}

//------------------------------------------------------------------------------

void TimerAsio::start()
{
    core::mt::ScopedLock lock(m_mutex);
    this->rearmNoLock(m_duration);
    m_running = true;
}

//------------------------------------------------------------------------------

void TimerAsio::stop()
{
    core::mt::ScopedLock lock(m_mutex);
    if(m_running)
    {
        m_running = false;
        this->cancelNoLock();
    }
}

//------------------------------------------------------------------------------

struct TimerCallback
{
    //------------------------------------------------------------------------------

    static void call(const boost::system::error_code& error, TimerAsio::sptr _timer)
    {
        if(!error)
        {
            TimerAsio::TimeDurationType duration;
            bool oneShot = false;
            {
                core::mt::ScopedLock lock(_timer->m_mutex);
                oneShot  = _timer->m_oneShot;
                duration = _timer->m_duration;
            }

            if(!oneShot)
            {
                {
                    core::mt::ScopedLock lock(_timer->m_mutex);
                    if(_timer->m_running)
                    {
                        _timer->rearmNoLock(duration);
                    }
                }
                _timer->m_function();
            }
            else
            {
                _timer->m_function();
                core::mt::ScopedLock lock(_timer->m_mutex);
                _timer->m_running = false;
            }
        }
    }
};

//------------------------------------------------------------------------------

void TimerAsio::rearmNoLock(TimeDurationType duration)
{
    this->cancelNoLock();
    boost::posix_time::time_duration d =
        boost::posix_time::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
    m_timer.expires_from_now(d);
    // NOLINTNEXTLINE(modernize-avoid-bind)
    m_timer.async_wait(boost::bind(TimerCallback::call, boost::asio::placeholders::error, this->getSptr()));
}

//------------------------------------------------------------------------------

void TimerAsio::cancelNoLock()
{
    m_timer.cancel();
}

} //namespace sight::core::thread
