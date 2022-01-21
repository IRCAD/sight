/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "core/thread/Pool.hpp"

#include <core/LazyInstantiator.hpp>
#include <core/spyLog.hpp>

namespace sight::core::thread
{

//-----------------------------------------------------------------------------

Pool::Pool() :
    Pool(std::thread::hardware_concurrency())
{
}

//-----------------------------------------------------------------------------

Pool::Pool(std::size_t _threads) :
    m_stop(false)
{
    SIGHT_WARN_IF(
        _threads << " threads were allocated in this thread pool, but you only have "
        << std::thread::hardware_concurrency() << " physical cores on this CPU",
        _threads > std::thread::hardware_concurrency()
    );

    for(std::size_t i = 0 ; i < _threads ; ++i)
    {
        m_workers.emplace_back(
            [this]
            {
                for( ; ; )
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(m_queueMutex);
                        m_condition.wait(lock, [this]{return m_stop || !m_tasks.empty();});
                        if(m_stop && m_tasks.empty())
                        {
                            return;
                        }

                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }

                    task();
                }
            });
    }
}

//-----------------------------------------------------------------------------

Pool::~Pool()
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }

    m_condition.notify_all();
    for(std::thread& worker : m_workers)
    {
        worker.join();
    }
}

//-----------------------------------------------------------------------------

Pool& getDefaultPool()
{
    auto poolInstance = core::LazyInstantiator<Pool>::getInstance();
    return *poolInstance;
}

//-----------------------------------------------------------------------------

} //namespace sight::core::thread
