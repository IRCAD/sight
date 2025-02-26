/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

// cspell:ignore HRESULT PWSTR

#include "core/thread/worker.hpp"

#include "core/lazy_instantiator.hpp"
#include "core/mt/types.hpp"

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>

#include <winerror.h>
#else
#include <pthread.h>
#endif

#include <codecvt>
#include <map>
#include <boost/locale/encoding_utf.hpp>

#include "core/spy_log.hpp"

namespace sight::core::thread
{

//------------------------------------------------------------------------------

thread_id_t get_current_thread_id()
{
    return std::this_thread::get_id();
}

//------------------------------------------------------------------------------

thread_native_id_t get_current_thread_native_id()
{
#ifdef _WIN32
    void* thread_id = GetCurrentThread();
    return thread_id;
#else
    pthread_t thread_id = pthread_self();
    return thread_id;
#endif
}

//------------------------------------------------------------------------------

void set_thread_name(const std::string& _thread_name, std::optional<std::thread::native_handle_type> _thread_id)
{
    SIGHT_WARN_IF(
        "Thread name '" << _thread_name << "' is too long. It must be restricted to "
        << get_max_length_of_thread_name() << " characters including the terminating null byte.",
        _thread_name.size()
        >= get_max_length_of_thread_name()
    );

    const std::string restricted_thread_name = _thread_name.substr(0, get_max_length_of_thread_name() - 1);

#ifdef _WIN32
    const std::wstring wide_thread_name(restricted_thread_name.begin(), restricted_thread_name.end()); // only works for
                                                                                                       // ascii
                                                                                                       // characters
    const auto thread_id_to_use = _thread_id.has_value() ? _thread_id.value() : GetCurrentThread();

    const HRESULT hr = SetThreadDescription(thread_id_to_use, wide_thread_name.c_str());

    SIGHT_WARN_IF("Unable to set the name of the thread. Error code: " << hr << std::endl, FAILED(hr));
#else
    const auto thread_id_to_use = _thread_id.has_value() ? _thread_id.value() : pthread_self();
    const auto success          = pthread_setname_np(thread_id_to_use, restricted_thread_name.c_str());

    SIGHT_WARN_IF(
        "Unable to set the name of the thread. Error code: "
        << "The length of the string specified pointed to by name exceeds the allowed limit." << std::endl,
        success != 0
    );
#endif
}

//------------------------------------------------------------------------------

std::string get_thread_name(std::optional<std::thread::native_handle_type> _thread_id)
{
#ifdef _WIN32
    PWSTR data                  = nullptr;
    const auto thread_id_to_use = _thread_id.has_value() ? _thread_id.value() : GetCurrentThread();

    const HRESULT hr = GetThreadDescription(thread_id_to_use, &data);

    if(FAILED(hr))
    {
        SIGHT_WARN("Failed to get thread name. Error code: " << hr << std::endl);
        return {};
    }

    std::wstring _wide_thread_name_(data);
    std::string thread_name;
    std::transform(
        _wide_thread_name_.begin(),
        _wide_thread_name_.end(),
        std::back_inserter(thread_name),
        [](const wchar_t c)
        {
            return static_cast<char>(c);
        });

    LocalFree(data);
    return thread_name;
#else
    const auto thread_id_to_use = _thread_id.has_value() ? _thread_id.value() : pthread_self();
    std::array<char, get_max_length_of_thread_name()> thread_name {};

    const auto success = pthread_getname_np(thread_id_to_use, thread_name.data(), sizeof(thread_name));
    if(success != 0)
    {
        SIGHT_WARN(
            "Failed to get thread name. Error code: The buffer specified by name and size is too small to hold the thread name."
            << std::endl
        );
        return {};
    }
    return {thread_name.data()};
#endif
}

//------------------------------------------------------------------------------

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

    void add_worker(const worker_key_type& _key, core::thread::worker::sptr _worker)
    {
        core::mt::write_lock lock(m_registry_mutex);
        m_workers.insert(worker_map_type::value_type(_key, _worker));
    }

    //------------------------------------------------------------------------------

    void remove_worker(const worker_key_type& _key)
    {
        core::mt::write_lock lock(m_registry_mutex);

        auto it = m_workers.find(_key);

        if(it != m_workers.end())
        {
            it->second->stop();
            m_workers.erase(_key);
        }
    }

    //------------------------------------------------------------------------------

    void remove_worker(core::thread::worker::sptr _worker)
    {
        core::mt::write_lock lock(m_registry_mutex);
        for(const auto& [key, value] : m_workers)
        {
            if(value == _worker)
            {
                _worker->stop();

                m_workers.erase(key);
                return;
            }
        }

        SIGHT_WARN("A worker was requested to be removed, but it could not be found");
    }

    //------------------------------------------------------------------------------

    core::thread::worker::sptr get_worker(const worker_key_type& _key) const
    {
        core::mt::read_lock lock(m_registry_mutex);

        if(auto it = m_workers.find(_key); it != m_workers.end())
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

    void set_default_worker(const core::thread::worker::sptr& _worker)
    {
        SIGHT_THROW_IF("default worker can not be null", _worker == nullptr);

        SIGHT_THROW_IF(
            "Can not switch the default worker as the initial one is already used in the application",
            m_default_worker.use_count() > 1
        );

        if(m_default_worker)
        {
            m_default_worker->stop();
        }

        m_default_worker = _worker;
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

core::thread::worker::sptr get_worker(const worker_key_type& _key)
{
    return active_workers::get()->get_worker(_key);
}

//-----------------------------------------------------------------------------

void add_worker(const worker_key_type& _key, core::thread::worker::sptr _worker)
{
    active_workers::get()->add_worker(_key, _worker);
}

//-----------------------------------------------------------------------------

void remove_worker(const worker_key_type& _key)
{
    active_workers::get()->remove_worker(_key);
}

//-----------------------------------------------------------------------------

void remove_worker(core::thread::worker::sptr _worker)
{
    active_workers::get()->remove_worker(_worker);
}

//-----------------------------------------------------------------------------

core::thread::worker::sptr get_default_worker()
{
    return active_workers::get()->get_default_worker();
}

//-----------------------------------------------------------------------------

void set_default_worker(core::thread::worker::sptr _worker)
{
    active_workers::get()->set_default_worker(_worker);
}

//------------------------------------------------------------------------------

void reset_default_worker()
{
    active_workers::get()->reset_default_worker();
}

//-----------------------------------------------------------------------------

} //namespace sight::core::thread
