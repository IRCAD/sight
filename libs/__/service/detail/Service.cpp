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

#include "service/detail/Service.hpp"

#include "service/registry.hpp"

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/com/signal.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/worker.hpp>

namespace sight::service::detail
{

using register_t = core::com::signal<void (data::Object::sptr, const std::string&)>;

static std::mutex s_signalMutex;
/// Signal used to notify that a new output object is available
register_t::sptr s_registerSignal = std::make_shared<register_t>();
/// Signal used to notify that an output object is no longer available
register_t::sptr s_unregisterSignal = std::make_shared<register_t>();

//-----------------------------------------------------------------------------

Service::Service(sight::service::base& service) :
    m_service(service)
{
}

Service::~Service()
{
    m_connections.disconnectStartSlot(m_service);
}

//-----------------------------------------------------------------------------

void Service::setConfiguration(const config_t& config)
{
    m_configuration      = config;
    m_configurationState = base::UNCONFIGURED;
}

//-----------------------------------------------------------------------------

const service::config_t& Service::getConfiguration() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

void Service::configure()
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

                auto config = m_service.getConfiguration();
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

base::SharedFutureType Service::start(bool _async)
{
    SIGHT_FATAL_IF("Service " << m_service.get_id() << " already started", m_globalState != base::STOPPED);

    m_connections.connect(m_service);

    m_globalState = base::STARTING;

    PackagedTaskType task([this](auto&& ...){m_service.starting();});
    base::SharedFutureType future = task.get_future();
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Error while STARTING service '" + m_service.get_id() + "' : " + e.what());
        SIGHT_ERROR("Service '" + m_service.get_id() + "' is still STOPPED.");
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

    this->autoConnect();

    auto sig = m_service.signal<service::signals::started_t>(service::signals::STARTED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

base::SharedFutureType Service::stop(bool _async)
{
    SIGHT_FATAL_IF("Service " << m_service.get_id() << " already stopped", m_globalState != base::STARTED);

    this->autoDisconnect();

    PackagedTaskType task([this](auto&& ...){m_service.stopping();});
    base::SharedFutureType future = task.get_future();

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
        SIGHT_ERROR("Service '" + m_service.get_id() + "' is still STARTED.");
        m_globalState = base::STARTED;
        this->autoConnect();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_globalState = base::STOPPED;

    auto sig = m_service.signal<service::signals::stopped_t>(service::signals::STOPPED);
    sig->async_emit(m_service.get_sptr());

    m_connections.disconnect(m_service);

    // check if the service manage outputs that are not maintained by someone else.
    std::string objectKeys;
    for(const auto& [key, ptr] : m_service.container())
    {
        if(ptr->access() == data::Access::out)
        {
            const data::Object::cwptr output = ptr->get();
            if(output.use_count() == 1)
            {
                if(!objectKeys.empty())
                {
                    objectKeys += ", ";
                }

                objectKeys += "'" + std::string(key.first) + "'(nbRef: " + std::to_string(output.use_count()) + ")";
            }
        }
    }

    SIGHT_WARN_IF(
        "Service " + m_service.get_id() + " still contains registered outputs: " + objectKeys + ". They will no "
                                                                                                "longer be maintained. You should call setOutput(key, nullptr) before stopping the service to inform "
                                                                                                "AppManager and other services that the object will be destroyed.",
        !objectKeys.empty()
    );

    return future;
}

//-----------------------------------------------------------------------------

base::SharedFutureType Service::swapKey(std::string_view _key, data::Object::sptr _obj, bool _async)
{
    SIGHT_FATAL_IF(
        "Service " << m_service.get_id() << " is not STARTED, no swapping with Object "
        << (_obj ? _obj->get_id() : "nullptr"),
        m_globalState != base::STARTED
    );

    auto fn = [this, _key]{m_service.swapping(_key);};
    PackagedTaskType task(fn);
    base::SharedFutureType future = task.get_future();

    this->autoDisconnect();

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

    this->autoConnect();

    auto sig = m_service.signal<service::signals::swapped_t>(service::signals::SWAPPED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

base::SharedFutureType Service::update(bool _async)
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

    PackagedTaskType task([this](auto&& ...){m_service.updating();});
    base::SharedFutureType future = task.get_future();
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

    auto sig = m_service.signal<service::signals::updated_t>(service::signals::UPDATED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

void Service::autoConnect()
{
    service::connections_t connectionMap = m_service.getAutoConnections();

    for(const auto& [key, ptr] : m_service.container())
    {
        data::Object::csptr obj = ptr->get();

        if(ptr->autoConnect() && obj)
        {
            core::com::helper::sig_slot_connection::key_connections_type connections;
            if(!connectionMap.empty())
            {
                if(auto it = connectionMap.find(key.first); it != connectionMap.end())
                {
                    connections = it->second;
                }

                SIGHT_ERROR_IF(
                    "Object '" << key.first << "' of '" << m_service.get_id() << "'(" << m_service.get_classname()
                    << ") set to 'autoConnect=\"true\"' but no matching connection defined in getAutoConnections().",
                    connections.empty() && ptr->autoConnect()
                );
            }
            else
            {
                SIGHT_ERROR(
                    "Object '" << key.first << "' of '" << m_service.get_id() << "'(" << m_service.get_classname()
                    << ") set to 'autoConnect=\"true\"' but no matching connection is defined in getAutoConnections()."
                );
            }

            m_autoConnections.connect(obj, m_service.get_sptr(), connections);
        }
    }
}

//-----------------------------------------------------------------------------

void Service::autoDisconnect()
{
    m_autoConnections.disconnect();
}

//------------------------------------------------------------------------------

core::com::connection Service::connectRegisterOut(const core::com::slot_base::sptr& _slot)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    return s_registerSignal->connect(_slot);
}

//------------------------------------------------------------------------------

core::com::connection Service::connectUnregisterOut(const core::com::slot_base::sptr& _slot)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    return s_unregisterSignal->connect(_slot);
}

//------------------------------------------------------------------------------

void Service::notifyRegisterOut(data::Object::sptr _obj, const std::string& _id)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    s_registerSignal->async_emit(_obj, _id);
}

//------------------------------------------------------------------------------

void Service::notifyUnregisterOut(data::Object::sptr _obj, const std::string& _id)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    s_unregisterSignal->async_emit(_obj, _id);
}

//------------------------------------------------------------------------------

std::pair<bool, bool> Service::getKeyProps(const std::string& _key) const
{
    const auto& container = m_service.container();
    if(auto itData = container.find({_key, {}}); itData != container.end())
    {
        return {itData->second->autoConnect(), itData->second->optional()};
    }

    return {false, false};
}

//-----------------------------------------------------------------------------

} // namespace sight::service::detail
