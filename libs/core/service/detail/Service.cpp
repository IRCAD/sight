/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Signal.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/Worker.hpp>

namespace sight::service::detail
{

using register_t = core::com::Signal<void (data::Object::sptr, const std::string&)>;

static std::mutex s_signalMutex;
/// Signal used to notify that a new output object is available
register_t::sptr s_registerSignal = std::make_shared<register_t>();
/// Signal used to notify that an output object is no longer available
register_t::sptr s_unregisterSignal = std::make_shared<register_t>();

//-----------------------------------------------------------------------------

Service::Service(sight::service::IService& service) :
    m_service(service)
{
}

//-----------------------------------------------------------------------------

void Service::setConfiguration(const config_t& config)
{
    m_configuration      = config;
    m_configurationState = IService::UNCONFIGURED;
}

//-----------------------------------------------------------------------------

const IService::config_t& Service::getConfiguration() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

void Service::configure()
{
    if(m_configurationState == IService::UNCONFIGURED)
    {
        m_configurationState = IService::CONFIGURING;
        if(m_globalState == IService::STOPPED)
        {
            try
            {
                m_service.configuring();
                m_service.configuring(m_configuration);
            }
            catch(const boost::property_tree::ptree_bad_path& e)
            {
                SIGHT_ERROR("Error while configuring the service '" + m_service.getID() + "' : " + e.what());

                auto config = m_service.getConfiguration();
                SIGHT_ERROR("With the given configuration:\n" + core::runtime::property_tree::toString(config));
            }
            catch(std::exception& e)
            {
                SIGHT_ERROR("Error while configuring service '" + m_service.getID() + "' : " + e.what());
                throw; // Rethrow the error for unit tests
            }
        }
        else if(m_globalState == IService::STARTED)
        {
            SIGHT_ERROR(
                "Error trying to configure the service '" + m_service.getID() + "' whereas it is already started."
            );
        }

        m_configurationState = IService::CONFIGURED;
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType Service::start(bool _async)
{
    SIGHT_FATAL_IF("Service " << m_service.getID() << " already started", m_globalState != IService::STOPPED);

    m_connections.connect(m_service);

    m_globalState = IService::STARTING;

    PackagedTaskType task([this](auto&& ...){m_service.starting();});
    IService::SharedFutureType future = task.get_future();
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Error while STARTING service '" + m_service.getID() + "' : " + e.what());
        SIGHT_ERROR("Service '" + m_service.getID() + "' is still STOPPED.");
        m_globalState = IService::STOPPED;
        m_connections.disconnect(m_service);

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_globalState = IService::STARTED;

    this->autoConnect();

    auto sig = m_service.signal<IService::signals::started_t>(IService::signals::s_STARTED);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType Service::stop(bool _async)
{
    SIGHT_FATAL_IF("Service " << m_service.getID() << " already stopped", m_globalState != IService::STARTED);

    this->autoDisconnect();

    PackagedTaskType task([this](auto&& ...){m_service.stopping();});
    IService::SharedFutureType future = task.get_future();

    m_globalState = IService::STOPPING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while STOPPING service '" + m_service.getID() + "' : " + e.what());
        SIGHT_ERROR("Service '" + m_service.getID() + "' is still STARTED.");
        m_globalState = IService::STARTED;
        this->autoConnect();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_globalState = IService::STOPPED;

    auto sig = m_service.signal<IService::signals::stopped_t>(IService::signals::s_STOPPED);
    sig->asyncEmit();

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
        "Service " + m_service.getID() + " still contains registered outputs: " + objectKeys + ". They will no "
                                                                                               "longer be maintained. You should call setOutput(key, nullptr) before stopping the service to inform "
                                                                                               "AppManager and other services that the object will be destroyed.",
        !objectKeys.empty()
    );

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType Service::swapKey(std::string_view _key, data::Object::sptr _obj, bool _async)
{
    SIGHT_FATAL_IF(
        "Service " << m_service.getID() << " is not STARTED, no swapping with Object "
        << (_obj ? _obj->getID() : "nullptr"),
        m_globalState != IService::STARTED
    );

    auto fn = [this, _key]{m_service.swapping(_key);};
    PackagedTaskType task(fn);
    IService::SharedFutureType future = task.get_future();

    this->autoDisconnect();

    m_globalState = IService::SWAPPING;
    task();
    m_globalState = IService::STARTED;

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while SWAPPING service '" + m_service.getID() + "' : " + e.what());

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }

    this->autoConnect();

    auto sig = m_service.signal<IService::signals::swapped_t>(IService::signals::s_SWAPPED);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType Service::update(bool _async)
{
    if(m_globalState != IService::STARTED)
    {
        SIGHT_WARN(
            "INVOKING update WHILE STOPPED (" << m_globalState << ") on service '" << m_service.getID()
            << "' of type '" << m_service.getClassname() << "': update is discarded."
        );
        return {};
    }

    SIGHT_ASSERT(
        "INVOKING update WHILE NOT IDLE (" << m_updatingState << ") on service '" << m_service.getID()
        << "' of type '" << m_service.getClassname() << "'",
        m_updatingState == IService::NOTUPDATING
    );

    PackagedTaskType task([this](auto&& ...){m_service.updating();});
    IService::SharedFutureType future = task.get_future();
    m_updatingState = IService::UPDATING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while UPDATING service '" + m_service.getID() + "' : " + e.what());

        m_updatingState = IService::NOTUPDATING;
        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_updatingState = IService::NOTUPDATING;

    auto sig = m_service.signal<IService::signals::updated_t>(IService::signals::s_UPDATED);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

void Service::autoConnect()
{
    service::IService::KeyConnectionsMap connectionMap = m_service.getAutoConnections();

    for(const auto& [key, ptr] : m_service.container())
    {
        data::Object::csptr obj = ptr->get();

        if(ptr->autoConnect() && obj)
        {
            core::com::helper::SigSlotConnection::KeyConnectionsType connections;
            if(!connectionMap.empty())
            {
                if(auto it = connectionMap.find(key.first); it != connectionMap.end())
                {
                    connections = it->second;
                }

                SIGHT_ERROR_IF(
                    "Object '" << key.first << "' of '" << m_service.getID() << "'(" << m_service.getClassname()
                    << ") set to 'autoConnect=\"true\"' but no matching connection defined in getAutoConnections().",
                    connections.empty() && ptr->autoConnect()
                );
            }
            else
            {
                SIGHT_ERROR(
                    "Object '" << key.first << "' of '" << m_service.getID() << "'(" << m_service.getClassname()
                    << ") set to 'autoConnect=\"true\"' but no matching connection is defined in getAutoConnections()."
                );
            }

            m_autoConnections.connect(obj, m_service.getSptr(), connections);
        }
    }
}

//-----------------------------------------------------------------------------

void Service::autoDisconnect()
{
    m_autoConnections.disconnect();
}

//------------------------------------------------------------------------------

core::com::Connection Service::connectRegisterOut(const core::com::SlotBase::sptr& _slot)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    return s_registerSignal->connect(_slot);
}

//------------------------------------------------------------------------------

core::com::Connection Service::connectUnregisterOut(const core::com::SlotBase::sptr& _slot)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    return s_unregisterSignal->connect(_slot);
}

//------------------------------------------------------------------------------

void Service::notifyRegisterOut(data::Object::sptr _obj, const std::string& _id)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    s_registerSignal->asyncEmit(_obj, _id);
}

//------------------------------------------------------------------------------

void Service::notifyUnregisterOut(data::Object::sptr _obj, const std::string& _id)
{
    std::lock_guard<std::mutex> lock(s_signalMutex);
    s_unregisterSignal->asyncEmit(_obj, _id);
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
