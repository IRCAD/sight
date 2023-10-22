/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "service/detail/service.hpp"

#include "service/registry.hpp"

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/com/signal.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/worker.hpp>

namespace sight::service::detail
{

//-----------------------------------------------------------------------------

service::service(sight::service::base& _service) :
    m_service(_service)
{
}

service::~service()
{
    m_connections.disconnectStartSlot(m_service);
}

//-----------------------------------------------------------------------------

void service::set_config(const config_t& _config)
{
    m_configuration      = _config;
    m_configurationState = base::UNCONFIGURED;
}

//-----------------------------------------------------------------------------

const config_t& service::get_config() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

void service::configure()
{
    if(m_configurationState == base::UNCONFIGURED)
    {
        m_configurationState = base::CONFIGURING;
        if(m_globalState == base::STOPPED)
        {
            try
            {
                m_service.configuring();
                m_service.configuring(m_configuration);
            }
            catch(const boost::property_tree::ptree_bad_path& e)
            {
                SIGHT_ERROR("Error while configuring the service '" + m_service.get_id() + "' : " + e.what());

                auto config = m_service.get_config();
                SIGHT_ERROR("With the given configuration:\n" + core::runtime::property_tree::to_string(config));
            }
            catch(std::exception& e)
            {
                SIGHT_ERROR("Error while configuring service '" + m_service.get_id() + "' : " + e.what());
                throw; // Rethrow the error for unit tests
            }
        }
        else if(m_globalState == base::STARTED)
        {
            SIGHT_ERROR(
                "Error trying to configure the service '" + m_service.get_id() + "' whereas it is already started."
            );
        }

        m_configurationState = base::CONFIGURED;
    }

    m_connections.connectStartSlot(m_service);
}

//-----------------------------------------------------------------------------

base::shared_future_t service::start(bool _async)
{
    SIGHT_FATAL_IF("service " << m_service.get_id() << " already started", m_globalState != base::STOPPED);

    m_connections.connect(m_service);

    m_globalState = base::STARTING;

    packaged_task_t task([this](auto&& ...){m_service.starting();});
    base::shared_future_t future = task.get_future();
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Error while STARTING service '" + m_service.get_id() + "' : " + e.what());
        SIGHT_ERROR("service '" + m_service.get_id() + "' is still STOPPED.");
        m_globalState = base::STOPPED;
        m_connections.disconnect(m_service);

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_globalState = base::STARTED;

    this->auto_connect();

    auto sig = m_service.signal<sight::service::signals::started_t>(sight::service::signals::STARTED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

base::shared_future_t service::stop(bool _async)
{
    SIGHT_FATAL_IF("service " << m_service.get_id() << " already stopped", m_globalState != base::STARTED);

    this->auto_disconnect();

    packaged_task_t task([this](auto&& ...){m_service.stopping();});
    base::shared_future_t future = task.get_future();

    m_globalState = base::STOPPING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while STOPPING service '" + m_service.get_id() + "' : " + e.what());
        SIGHT_ERROR("service '" + m_service.get_id() + "' is still STARTED.");
        m_globalState = base::STARTED;
        this->auto_connect();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_globalState = base::STOPPED;

    auto sig = m_service.signal<sight::service::signals::stopped_t>(sight::service::signals::STOPPED);
    sig->async_emit(m_service.get_sptr());

    m_connections.disconnect(m_service);

    // check if the service manage outputs that are not maintained by someone else.
    std::string object_keys;
    for(const auto& [key, ptr] : m_service.container())
    {
        if(ptr->access() == data::Access::out)
        {
            const data::object::cwptr output = ptr->get();
            if(output.use_count() == 1)
            {
                if(!object_keys.empty())
                {
                    object_keys += ", ";
                }

                object_keys += "'" + std::string(key.first) + "'(nbRef: " + std::to_string(output.use_count()) + ")";
            }
        }
    }

    SIGHT_WARN_IF(
        "service " + m_service.get_id() + " still contains registered outputs: " + object_keys + ". They will no "
                                                                                                 "longer be maintained. You should call set_output(key, nullptr) before stopping the service to inform "
                                                                                                 "AppManager and other services that the object will be destroyed.",
        !object_keys.empty()
    );

    return future;
}

//-----------------------------------------------------------------------------

base::shared_future_t service::swap_key(std::string_view _key, data::object::sptr _obj, bool _async)
{
    SIGHT_FATAL_IF(
        "service " << m_service.get_id() << " is not STARTED, no swapping with Object "
        << (_obj ? _obj->get_id() : "nullptr"),
        m_globalState != base::STARTED
    );

    auto fn = [this, _key]{m_service.swapping(_key);};
    packaged_task_t task(fn);
    base::shared_future_t future = task.get_future();

    this->auto_disconnect();

    m_globalState = base::SWAPPING;
    task();
    m_globalState = base::STARTED;

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while SWAPPING service '" + m_service.get_id() + "' : " + e.what());

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }

    this->auto_connect();

    auto sig = m_service.signal<sight::service::signals::swapped_t>(sight::service::signals::SWAPPED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

base::shared_future_t service::update(bool _async)
{
    if(m_globalState != base::STARTED)
    {
        SIGHT_WARN(
            "INVOKING update WHILE STOPPED (" << m_globalState << ") on service '" << m_service.get_id()
            << "' of type '" << m_service.get_classname() << "': update is discarded."
        );
        return {};
    }

    SIGHT_ASSERT(
        "INVOKING update WHILE NOT IDLE (" << m_updatingState << ") on service '" << m_service.get_id()
        << "' of type '" << m_service.get_classname() << "'",
        m_updatingState == base::NOTUPDATING
    );

    packaged_task_t task([this](auto&& ...){m_service.updating();});
    base::shared_future_t future = task.get_future();
    m_updatingState = base::UPDATING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while UPDATING service '" + m_service.get_id() + "' : " + e.what());

        m_updatingState = base::NOTUPDATING;
        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_updatingState = base::NOTUPDATING;

    auto sig = m_service.signal<sight::service::signals::updated_t>(sight::service::signals::UPDATED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

void service::auto_connect()
{
    sight::service::connections_t connection_map = m_service.auto_connections();

    for(const auto& [key, ptr] : m_service.container())
    {
        data::object::csptr obj = ptr->get();

        if(ptr->auto_connect() && obj)
        {
            core::com::helper::sig_slot_connection::key_connections_t connections;
            if(!connection_map.empty())
            {
                if(auto it = connection_map.find(key.first); it != connection_map.end())
                {
                    connections = it->second;
                }

                SIGHT_ERROR_IF(
                    "Object '" << key.first << "' of '" << m_service.get_id() << "'(" << m_service.get_classname()
                    << ") set to 'auto_connect=\"true\"' but no matching connection defined in auto_connections().",
                    connections.empty() && ptr->auto_connect()
                );
            }
            else
            {
                SIGHT_ERROR(
                    "Object '" << key.first << "' of '" << m_service.get_id() << "'(" << m_service.get_classname()
                    << ") set to 'auto_connect=\"true\"' but no matching connection is defined in auto_connections()."
                );
            }

            m_autoConnections.connect(obj, m_service.get_sptr(), connections);
        }
    }
}

//-----------------------------------------------------------------------------

void service::auto_disconnect()
{
    m_autoConnections.disconnect();
}

//------------------------------------------------------------------------------

std::pair<bool, bool> service::get_object_key_attrs(const std::string& _key) const
{
    const auto& container = m_service.container();
    if(auto it_data = container.find({_key, {}}); it_data != container.end())
    {
        return {it_data->second->auto_connect(), it_data->second->optional()};
    }

    return {false, false};
}

//-----------------------------------------------------------------------------

} // namespace sight::service::detail
