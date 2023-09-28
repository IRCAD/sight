/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/thread/worker.hpp"

#include "core/lazy_instantiator.hpp"
#include "core/mt/types.hpp"

#include <map>
namespace sight::core::thread
{

//------------------------------------------------------------------------------

thread_id_t get_current_thread_id()
{
    return std::this_thread::get_id();
}

/**
 * @brief This internal class registers worker threads in the system. It creates a default worker.
 * The life cycle of registered workers should be handled by the creator of the workers, but to avoid unneeded crashes,
 * we do stop and destroy them if this not done outside. A non breaking error message is sent.
 */
class active_workers
{
public:

    /// Constructor, creates the default worker
    active_workers()
    = default;

    /// Destructor, destroys the default worker and the registered ones if necessary (this sends an error in this case)
    virtual ~active_workers()
    {
        core::mt::write_lock lock(m_registry_mutex);

        // Avoid double stop
        if(m_default_worker)
        {
            m_default_worker->stop();
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

    void add_worker(const worker_key_type& key, core::thread::worker::sptr worker)
    {
        core::mt::write_lock lock(m_registry_mutex);
        m_workers.insert(worker_map_type::value_type(key, worker));
    }

    //------------------------------------------------------------------------------

    void remove_worker(const worker_key_type& key)
    {
        core::mt::write_lock lock(m_registry_mutex);

        auto it = m_workers.find(key);

        if(it != m_workers.end())
        {
            it->second->stop();
            m_workers.erase(key);
        }
    }

    //------------------------------------------------------------------------------

    void remove_worker(core::thread::worker::sptr worker)
    {
        core::mt::write_lock lock(m_registry_mutex);
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

    core::thread::worker::sptr get_worker(const worker_key_type& key) const
    {
        core::mt::read_lock lock(m_registry_mutex);

        if(auto it = m_workers.find(key); it != m_workers.end())
        {
            return it->second;
        }

        return {};
    }

    //------------------------------------------------------------------------------

    static std::shared_ptr<active_workers> get()
    {
        return core::lazy_instantiator<active_workers>::get_instance();
    }

    //------------------------------------------------------------------------------

    core::thread::worker::sptr get_default_worker() const
    {
        return m_default_worker;
    }

    //------------------------------------------------------------------------------

    void set_default_worker(const core::thread::worker::sptr& worker)
    {
        SIGHT_THROW_IF("default worker can not be null", worker == nullptr);

        SIGHT_THROW_IF(
            "Can not switch the default worker as the initial one is already used in the application",
            m_default_worker.use_count() > 1
        );

        m_default_worker->stop();
        m_default_worker = worker;
    }

    //------------------------------------------------------------------------------

    void reset_default_worker()
    {
        m_default_worker->stop();
        m_default_worker.reset();
    }

private:

    /// Specific pointer for the default worker
    core::thread::worker::sptr m_default_worker {core::thread::worker::make()};

    using worker_map_type = std::map<worker_key_type, core::thread::worker::sptr>;

    /// Association key <=> worker
    worker_map_type m_workers;

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registry_mutex;
};

static auto active_workers = core::thread::active_workers::get();

//-----------------------------------------------------------------------------

core::thread::worker::sptr get_worker(const worker_key_type& key)
{
    return active_workers::get()->get_worker(key);
}

//-----------------------------------------------------------------------------

void add_worker(const worker_key_type& key, core::thread::worker::sptr worker)
{
    active_workers::get()->add_worker(key, worker);
}

//-----------------------------------------------------------------------------

void remove_worker(const worker_key_type& key)
{
    active_workers::get()->remove_worker(key);
}

//-----------------------------------------------------------------------------

void remove_worker(core::thread::worker::sptr worker)
{
    active_workers::get()->remove_worker(worker);
}

//-----------------------------------------------------------------------------

core::thread::worker::sptr get_default_worker()
{
    return active_workers::get()->get_default_worker();
}

//-----------------------------------------------------------------------------

void set_default_worker(core::thread::worker::sptr worker)
{
    active_workers::get()->set_default_worker(worker);
}

//------------------------------------------------------------------------------

void reset_default_worker()
{
    active_workers::get()->reset_default_worker();
}

//-----------------------------------------------------------------------------

} //namespace sight::core::thread
