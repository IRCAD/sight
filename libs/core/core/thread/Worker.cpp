/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "core/thread/Worker.hpp"

#include "core/LazyInstantiator.hpp"
#include "core/mt/types.hpp"

#include <map>
namespace sight::core::thread
{

//------------------------------------------------------------------------------

ThreadIdType getCurrentThreadId()
{
    return std::this_thread::get_id();
}

/**
 * @brief This internal class registers worker threads in the system. It creates a default worker.
 * The life cycle of registered workers should be handled by the creator of the workers, but to avoid unneeded crashes,
 * we do stop and destroy them if this not done outside. A non breaking error message is sent.
 */
class ActiveWorkers
{
public:

    /// Constructor, creates the default worker
    ActiveWorkers()
    = default;

    /// Destructor, destroys the default worker and the registered ones if necessary (this sends an error in this case)
    virtual ~ActiveWorkers()
    {
        core::mt::WriteLock lock(m_registryMutex);

        // Avoid double stop
        if(m_defaultWorker)
        {
            m_defaultWorker->stop();
        }

        SIGHT_ERROR_IF(
            "Workers are still registered, this is abnormal unless the application crashed before.",
            !m_workers.empty()
        );

        for(const auto& elt : m_workers)
        {
            elt.second->stop();
        }

        m_workers.clear();
    }

    //------------------------------------------------------------------------------

    void addWorker(const WorkerKeyType& key, core::thread::Worker::sptr worker)
    {
        core::mt::WriteLock lock(m_registryMutex);
        m_workers.insert(WorkerMapType::value_type(key, worker));
    }

    //------------------------------------------------------------------------------

    void removeWorker(const WorkerKeyType& key)
    {
        core::mt::WriteLock lock(m_registryMutex);

        auto it = m_workers.find(key);

        if(it != m_workers.end())
        {
            it->second->stop();
            m_workers.erase(key);
        }
    }

    //------------------------------------------------------------------------------

    void removeWorker(core::thread::Worker::sptr worker)
    {
        core::mt::WriteLock lock(m_registryMutex);
        for(const auto& [key, value] : m_workers)
        {
            if(value == worker)
            {
                worker->stop();

                m_workers.erase(key);
                return;
            }
        }

        SIGHT_WARN("A worker was requested to be removed, but it could not be found");
    }

    //------------------------------------------------------------------------------

    core::thread::Worker::sptr getWorker(const WorkerKeyType& key) const
    {
        core::mt::ReadLock lock(m_registryMutex);

        if(auto it = m_workers.find(key); it != m_workers.end())
        {
            return it->second;
        }

        return {};
    }

    //------------------------------------------------------------------------------

    static std::shared_ptr<ActiveWorkers> get()
    {
        return core::LazyInstantiator<ActiveWorkers>::getInstance();
    }

    //------------------------------------------------------------------------------

    core::thread::Worker::sptr getDefaultWorker() const
    {
        return m_defaultWorker;
    }

    //------------------------------------------------------------------------------

    void setDefaultWorker(const core::thread::Worker::sptr& worker)
    {
        SIGHT_THROW_IF("default worker can not be null", worker == nullptr);

        SIGHT_THROW_IF(
            "Can not switch the default worker as the initial one is already used in the application",
            m_defaultWorker.use_count() > 1
        );

        m_defaultWorker->stop();
        m_defaultWorker = worker;
    }

    //------------------------------------------------------------------------------

    void resetDefaultWorker()
    {
        m_defaultWorker->stop();
        m_defaultWorker.reset();
    }

private:

    /// Specific pointer for the default worker
    core::thread::Worker::sptr m_defaultWorker {core::thread::Worker::New()};

    using WorkerMapType = std::map<WorkerKeyType, core::thread::Worker::sptr>;

    /// Association key <=> worker
    WorkerMapType m_workers;

    /// Used to protect the registry access.
    mutable core::mt::ReadWriteMutex m_registryMutex;
};

static auto activeWorkers = core::thread::ActiveWorkers::get();

//-----------------------------------------------------------------------------

core::thread::Worker::sptr getWorker(const WorkerKeyType& key)
{
    return ActiveWorkers::get()->getWorker(key);
}

//-----------------------------------------------------------------------------

void addWorker(const WorkerKeyType& key, core::thread::Worker::sptr worker)
{
    ActiveWorkers::get()->addWorker(key, worker);
}

//-----------------------------------------------------------------------------

void removeWorker(const WorkerKeyType& key)
{
    ActiveWorkers::get()->removeWorker(key);
}

//-----------------------------------------------------------------------------

void removeWorker(core::thread::Worker::sptr worker)
{
    ActiveWorkers::get()->removeWorker(worker);
}

//-----------------------------------------------------------------------------

core::thread::Worker::sptr getDefaultWorker()
{
    return ActiveWorkers::get()->getDefaultWorker();
}

//-----------------------------------------------------------------------------

void setDefaultWorker(core::thread::Worker::sptr worker)
{
    ActiveWorkers::get()->setDefaultWorker(worker);
}

//------------------------------------------------------------------------------

void resetDefaultWorker()
{
    ActiveWorkers::get()->resetDefaultWorker();
}

//-----------------------------------------------------------------------------

} //namespace sight::core::thread
