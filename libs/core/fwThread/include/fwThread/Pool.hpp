/************************************************************************
 *
 * Copyright (C) 2015-2018 IRCAD France
 * Copyright (C) 2015-2018 IHU Strasbourg
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

#include "fwThread/config.hpp"

#include <fwCore/base.hpp>

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace fwThread
{

/**
 * @brief   This class creates and manages a pool of threads which process tasks.
 *
 * The purpose of this class is to provide a set of threads that can be used to process tasks asynchronously.
 *
 */
class FWTHREAD_CLASS_API Pool
{
public:
    typedef std::shared_ptr<Pool> sptr;

    /// this constructor launches as much as possible workers
    FWTHREAD_API Pool();
    /// this constructor launches some amount of workers
    FWTHREAD_API Pool(size_t);
    /// the destructor joins all threads
    FWTHREAD_API ~Pool();

    /// add new work item to the pool
    template<class F, class ... Args>
    auto post(F&& f, Args&& ... args)
    ->std::shared_future<typename std::result_of<F(Args ...)>::type>;

private:
    /// need to keep track of threads so we can join them
    std::vector< std::thread > m_workers;

    /// the task queue
    std::queue< std::function<void()> > m_tasks;

    /// synchronization
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    bool m_stop;
};

//-----------------------------------------------------------------------------

template<class F, class ... Args>
auto Pool::post(F&& f, Args&& ... args)
->std::shared_future<typename std::result_of<F(Args ...)>::type>
{
    using return_type = typename std::result_of<F(Args ...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args) ...)
        );

    std::shared_future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);

        // don't allow enqueueing after stopping the pool
        if(m_stop)
        {
            throw std::runtime_error("enqueue on stopped Pool");
        }

        m_tasks.emplace([task]()
            {
                (*task)();
            });
    }
    m_condition.notify_one();
    return res;
}

/// Get the default pool
FWTHREAD_API Pool& getDefaultPool();

//-----------------------------------------------------------------------------

} //namespace fwThread
