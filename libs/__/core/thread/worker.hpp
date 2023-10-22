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

// cspell:ignore NOLINT

#pragma once

#include <future>
#include <any>

#include <core/base.hpp>
#include <core/hires_clock.hpp>

#include "core/config.hpp"

namespace sight::core::thread
{

typedef std::thread::id thread_id_t;

/// Returns the current thread id
CORE_API thread_id_t get_current_thread_id();

class timer; // NOLINT(bugprone-forward-declaration-namespace)

/**
 * @brief   This class creates and manages a task loop.
 * The default implementation create a loop in a new thread.
 */
class CORE_CLASS_API worker : public core::base_object
{
public:

    typedef core::hires_clock::type period_t;
    typedef std::function<void ()> task_t;
    typedef std::any exit_return_type;

    typedef std::shared_future<exit_return_type> future_t;

    SIGHT_DECLARE_CLASS(worker, core::base_object);

    worker()
    = default;

    /// Waits for the last task to be processed and stops the loop
    CORE_API virtual void stop() = 0;

    /// Requests invocation of the given task handler and returns immediately.
    virtual void post(task_t _handler) = 0;

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
    std::shared_future<R> post_task(CALLABLE _f);

    /// Returns the worker's thread id
    CORE_API virtual thread_id_t get_thread_id() const = 0;

    /// Creates and returns a core::thread::timer running in this Worker
    CORE_API virtual SPTR(core::thread::timer) create_timer() = 0;

    /**
     * @brief Returns a std::shared_future associated with the execution of Worker's loop
     * @warning Calling get_future() may be blocking if it is required by a specific implementation (for example, the Qt
     * implementation).
     */
    virtual future_t get_future()
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
    CORE_API virtual void process_tasks(period_t _maxtime) = 0;

    /**
     * @brief Processes all worker pending tasks for the calling thread
     * until there are no more tasks to process.
     * You can call this function occasionally when your program is busy performing a long operation.
     * @warning WxWidgets version is not yet implemented.
     */
    CORE_API virtual void process_tasks() = 0;

    /// Creates and returns a new instance of Worker default implementation
    /// (boost::Asio).
    CORE_API static SPTR(worker) make();

protected:

    /// Copy constructor forbidden
    worker(const worker&);

    /// Copy operator forbidden
    worker& operator=(const worker&);

    /// Worker's loop future
    future_t m_future;
};

typedef std::string worker_key_type;

/**
 * @brief Get a registered worker given a key, if the key does not exist a null pointer is returned
 * @param key name of the worker thread
 * @note This method is thread safe.
 */
CORE_API core::thread::worker::sptr get_worker(const worker_key_type& _key);

/**
 * @brief Registers a worker.
 * The caller is still responsible of the lifetime of the worker and should call remove_worker() symmetrically.
 * @param key name of the worker thread
 * @param worker pointer to the worker thread
 * @note This method is thread safe.
 */
CORE_API void add_worker(const worker_key_type& _key, core::thread::worker::sptr _worker);

/**
 * @brief Stops and unregisters a worker
 * @param key name of the worker thread
 * @note This method is thread safe.
 */
CORE_API void remove_worker(const worker_key_type& _key);

/**
 * @brief Stops and unregisters a worker
 * @param worker pointer to the worker thread
 * @note This method is thread safe.
 */
CORE_API void remove_worker(core::thread::worker::sptr _worker);

/**
 * @brief Get the default registered worker
 * @note This method is thread safe.
 */
CORE_API core::thread::worker::sptr get_default_worker();

/**
 * @brief Set the default registered worker.
 *
 * This is only needed in rare cases and should be used at the start of an application. This replaces and destroys
 * the default WorkerAsio, which should not be already in use.
 * If you use this method, you need to call reset_default_worker() to unregister the worker thread.
 *
 * @param worker pointer to the worker thread
 * @note This method is thread safe.
 * @throw sight::core::exception if the worker is null or if the previous default worker is already in use.
 */
CORE_API void set_default_worker(core::thread::worker::sptr _worker);

/**
 * @brief Reset the default registered worker.
 *
 * Beware that there would be no default thread registered after that call. So this is really intended to be called
 * at the end of the run of an application.
 *
 * @note This method is thread safe.
 */
CORE_API void reset_default_worker();

} // namespace sight::core::thread

#include "core/thread/worker.hxx"
