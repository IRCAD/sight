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

#pragma once

#include <future>
#include <any>

#include <core/base.hpp>
#include <core/HiResClock.hpp>

#include "core/config.hpp"

namespace sight::core::thread
{

typedef std::thread::id ThreadIdType;

/// Returns the current thread id
CORE_API ThreadIdType getCurrentThreadId();

class Timer;

/**
 * @brief   This class creates and manages a task loop.
 * The default implementation create a loop in a new thread.
 */
class CORE_CLASS_API Worker : public core::BaseObject
{
public:

    typedef core::HiResClock::HiResClockType PeriodType;
    typedef std::function<void ()> TaskType;
    typedef std::any ExitReturnType;

    typedef std::shared_future<ExitReturnType> FutureType;

    SIGHT_DECLARE_CLASS(Worker, core::BaseObject, defaultFactory);

    Worker()
    {
    }

    /// Waits for the last task to be processed and stops the loop
    CORE_API virtual void stop() = 0;

    /// Requests invocation of the given task handler and returns immediately.
    virtual void post(TaskType handler) = 0;

    /**
     * @brief Requests invocation of the given callable and returns a shared future.
     *
     * Returns immediately if called from another thread. Otherwise, the given callable is called
     * immediately.
     *
     * @tparam R future's value type
     * @tparam CALLABLE Any type wrappable with a std::function< void() >
     *
     * @returns a std::shared_future associated with the result of the given callable
     */
    template<typename R, typename CALLABLE>
    std::shared_future<R> postTask(CALLABLE f);

    /// Returns the worker's thread id
    CORE_API virtual ThreadIdType getThreadId() const = 0;

    /// Creates and returns a core::thread::Timer running in this Worker
    CORE_API virtual SPTR(core::thread::Timer) createTimer() = 0;

    /**
     * @brief Returns a std::shared_future associated with the execution of Worker's loop
     * @warning Calling getFuture() may be blocking if it is required by a specific implementation (for example, the Qt
     * implementation).
     */
    virtual FutureType getFuture()
    {
        return m_future;
    }

    /**
     * @brief Processes worker pending tasks for the calling thread
     * for maxtime milliseconds or until there are no more tasks to process.
     * You can call this function occasionally when your program is busy performing a long operation.
     *
     * @param maxtime milliseconds to process worker pending tasks.
     * @warning Qt implementation processes all Qt and Worker pending events, be careful.
     * @warning WxWidgets version is not yet implemented.
     */
    CORE_API virtual void processTasks(PeriodType maxtime) = 0;

    /**
     * @brief Processes all worker pending tasks for the calling thread
     * until there are no more tasks to process.
     * You can call this function occasionally when your program is busy performing a long operation.
     * @warning WxWidgets version is not yet implemented.
     */
    CORE_API virtual void processTasks() = 0;

protected:

    /// Creates and returns a new instance of Worker default implementation
    /// (boost::Asio).
    CORE_API static SPTR(Worker) defaultFactory();

    /// Copy constructor forbidden
    Worker(const Worker&);

    /// Copy operator forbidden
    Worker& operator=(const Worker&);

    /// Worker's loop future
    FutureType m_future;
};

} //namespace sight::core::thread

#include "core/thread/Worker.hxx"
