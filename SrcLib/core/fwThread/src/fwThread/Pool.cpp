/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwThread/Pool.hpp"

#include <fwCore/spyLog.hpp>
#include <fwCore/util/LazyInstantiator.hpp>

namespace fwThread
{

//-----------------------------------------------------------------------------

Pool::Pool() :
    Pool(std::thread::hardware_concurrency())
{
}

//-----------------------------------------------------------------------------

Pool::Pool(size_t _threads) :
    m_stop(false)
{
    const auto availableCores = std::thread::hardware_concurrency();
    OSLM_WARN_IF( _threads << " threads were allocated in this thread pool, but you only have " <<
                  availableCores << " physical cores on this CPU",
                  _threads > std::thread::hardware_concurrency());

    for(size_t i = 0; i < _threads; ++i)
    {
        m_workers.emplace_back(
            [this]
            {
                for(;; )
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(m_queueMutex);
                        m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
                        if(m_stop && m_tasks.empty())
                        {
                            return;
                        }
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }

                    task();
                }
            }
            );
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
    for(std::thread& worker: m_workers)
    {
        worker.join();
    }
}

//-----------------------------------------------------------------------------

Pool& getDefaultPool()
{
    auto poolInstance = ::fwCore::util::LazyInstantiator< Pool >::getInstance();
    return *poolInstance;
}

//-----------------------------------------------------------------------------

} //namespace fwThread
