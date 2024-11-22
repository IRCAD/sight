/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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
#include <core/com/slots.hxx>
#include <core/runtime/helper.hpp>
#include <core/thread/worker.hpp>

#include <ranges>

namespace sight::service::detail
{

//-----------------------------------------------------------------------------

service::service(sight::service::base& _service) :
    m_service(_service)
{
}

//-----------------------------------------------------------------------------

service::~service()
{
    m_connections.disconnect_start_slot(m_service);
}

//-----------------------------------------------------------------------------

void service::set_config(const config_t& _config)
{
    m_configuration       = _config;
    m_configuration_state = base::configuration_status::unconfigured;
}

//-----------------------------------------------------------------------------

const config_t& service::get_config() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

void service::configure()
{
    if(m_configuration_state == base::configuration_status::unconfigured)
    {
        m_configuration_state = base::configuration_status::configuring;
        if(m_global_state == base::global_status::stopped)
        {
            try
            {
                // Collect all input/output configurations
                std::map<std::string, std::string> properties_cfgs;
                if(const auto& properties = m_configuration.get_child_optional("properties"); properties.has_value())
                {
                    if(const auto& attributes = properties->get_child_optional("<xmlattr>"); attributes.has_value())
                    {
                        for(const auto& attribute : *attributes)
                        {
                            properties_cfgs[attribute.first] = attribute.second.get_value<std::string>();
                        }
                    }

                    auto properties_attrs = properties->equal_range("property");
                    for(auto it_prop = properties_attrs.first ; it_prop != properties_attrs.second ; ++it_prop)
                    {
                        if(auto obj_cfg = it_prop->second.get_child_optional("<xmlattr>"); obj_cfg.has_value())
                        {
                            // We take only the first element
                            auto first_element = *obj_cfg->begin();
                            properties_cfgs[first_element.first] = first_element.second.get_value<std::string>();
                        }
                    }
                }

                const auto properties_obj = m_service.m_properties_map.lock();
                const auto properties_map = std::dynamic_pointer_cast<data::map>(properties_obj.get_shared());

                // Look for properties
                auto is_property = [](auto& _p){return dynamic_cast<data::property_base*>(_p.second) != nullptr;};

                auto obj_from_property_map = [&](const std::string& _key) -> sight::data::object::sptr
                                             {
                                                 if(properties_map != nullptr)
                                                 {
                                                     if(const auto& prop_key = properties_map->find(_key);
                                                        prop_key != properties_map->end())
                                                     {
                                                         return prop_key->second;
                                                     }
                                                 }

                                                 return nullptr;
                                             };

                for(const auto& [key, ptr] : m_service.container() | std::views::filter(is_property))
                {
                    auto weak_obj = m_service.inout(key.first);
                    auto obj      = weak_obj.lock();

                    const auto& obj_from_map = obj_from_property_map(std::string(key.first));

                    if(obj == nullptr)
                    {
                        auto* property = dynamic_cast<data::property_base*>(ptr);
                        SIGHT_ASSERT("Data pointer is not convertible to a property", property);

                        if(obj_from_map != nullptr)
                        {
                            // We found a key in the map
                            m_service.set_object(obj_from_map, key.first, {}, ptr->access(), true, false);
                        }
                        else
                        {
                            auto new_obj = property->make_default();
                            m_created_properties.emplace_back(new_obj);

                            if(const auto& prop_cfg = properties_cfgs.find(std::string(key.first));
                               prop_cfg != properties_cfgs.end())
                            {
                                new_obj->from_string(prop_cfg->second);
                            }
                        }
                    }
                    else
                    {
                        SIGHT_ERROR_IF(
                            "Properties " << std::quoted(key.first)
                            << " set with an object while there is already a key in the property map",
                            obj_from_map != nullptr
                        );
                    }
                }

                // Create a slot for each property
                for(const auto& [key, ptr] : m_service.container())
                {
                    const auto& key_str = key.first;

                    if(dynamic_cast<data::property_base*>(ptr) != nullptr)
                    {
                        auto slot = m_service.new_slot(
                            std::string(key_str),
                            [&]()
                            {
                                m_service.on_property_set(key_str);
                            });
                        slot->set_worker(m_service.worker());
                    }
                }

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
        else if(m_global_state == base::global_status::started)
        {
            SIGHT_ERROR(
                "Error trying to configure the service '" + m_service.get_id() + "' whereas it is already started."
            );
        }

        m_configuration_state = base::configuration_status::configured;
    }

    m_connections.connect_start_slot(m_service);
}

//-----------------------------------------------------------------------------

base::shared_future_t service::start(bool _async)
{
    SIGHT_FATAL_IF(
        "service " << m_service.get_id() << " already started",
        m_global_state != base::global_status::stopped
    );

    m_connections.connect(m_service);

    m_global_state = base::global_status::starting;

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
        m_global_state = base::global_status::stopped;
        m_connections.disconnect(m_service);

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_global_state = base::global_status::started;

    this->auto_connect();

    auto sig = m_service.signal<sight::service::signals::started_t>(sight::service::signals::STARTED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

base::shared_future_t service::stop(bool _async)
{
    SIGHT_FATAL_IF(
        "service " << m_service.get_id() << " already stopped",
        m_global_state != base::global_status::started
    );

    this->auto_disconnect();

    packaged_task_t task([this](auto&& ...){m_service.stopping();});
    base::shared_future_t future = task.get_future();

    m_global_state = base::global_status::stopping;
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
        m_global_state = base::global_status::started;
        this->auto_connect();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_global_state = base::global_status::stopped;

    auto sig = m_service.signal<sight::service::signals::stopped_t>(sight::service::signals::STOPPED);
    sig->async_emit(m_service.get_sptr());

    m_connections.disconnect(m_service);

    // Reset all output objects to inform other services they are no longer available
    m_service.reset_all_out();

    return future;
}

//-----------------------------------------------------------------------------

base::shared_future_t service::swap_key(std::string_view _key, data::object::sptr _obj, bool _async)
{
    SIGHT_FATAL_IF(
        "service " << m_service.get_id() << " is not STARTED, no swapping with Object "
        << (_obj ? _obj->get_id() : "nullptr"),
        m_global_state != base::global_status::started
    );

    auto fn = [this, _key]{m_service.swapping(_key);};
    packaged_task_t task(fn);
    base::shared_future_t future = task.get_future();

    this->auto_disconnect();

    m_global_state = base::global_status::swapping;
    task();
    m_global_state = base::global_status::started;

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
    if(m_global_state != base::global_status::started)
    {
        SIGHT_WARN(
            "Update() called while not started: service '" << m_service.get_id() << "' of type '"
            << m_service.get_classname() << "': update is discarded."
        );
        return {};
    }

    SIGHT_ASSERT(
        "Update() called while already updating '" << m_service.get_id()
        << "' of type '" << m_service.get_classname() << "'",
        m_updating_state == base::updating_status::notupdating
    );

    packaged_task_t task([this](auto&& ...){m_service.updating();});
    base::shared_future_t future = task.get_future();
    m_updating_state = base::updating_status::updating;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while UPDATING service '" + m_service.get_id() + "' : " + e.what());

        m_updating_state = base::updating_status::notupdating;
        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_updating_state = base::updating_status::notupdating;

    auto sig = m_service.signal<sight::service::signals::updated_t>(sight::service::signals::UPDATED);
    sig->async_emit(m_service.get_sptr());

    return future;
}

//-----------------------------------------------------------------------------

void service::auto_connect()
{
    sight::service::connections_t connection_map = m_service.auto_connections();
    m_auto_connected = false;

    for(const auto& [key, ptr] : m_service.container())
    {
        const auto& key_str     = key.first;
        data::object::csptr obj = ptr->get();
        const bool auto_connect = !ptr->auto_connect().has_value() || ptr->auto_connect().value();
        if(auto_connect && obj)
        {
            core::com::helper::sig_slot_connection::key_connections_t connections;
            bool connected = false;

            if(auto it = connection_map.find(key_str); it != connection_map.end())
            {
                connections = it->second;
                m_auto_connections.connect(obj, m_service.get_sptr(), connections);
                connected = true;
                if(dynamic_cast<data::property_base*>(ptr) == nullptr)
                {
                    m_auto_connected = true;
                }
            }

            // Connect the properties
            if(!connected && dynamic_cast<data::property_base*>(ptr) != nullptr)
            {
                const auto sig = obj->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);

                auto slot = m_service.slot(std::string(key_str));
                SIGHT_ASSERT("Slot not found for property: " << key_str, slot);
                m_auto_connections.add_connection(sig->connect(slot));
            }
        }
    }
}

//-----------------------------------------------------------------------------

void service::auto_disconnect()
{
    m_auto_connections.disconnect();
    m_auto_connected = false;
}

//------------------------------------------------------------------------------

bool service::is_auto_connected() const
{
    return m_auto_connected;
}

//------------------------------------------------------------------------------

bool service::is_key_optional(const std::string& _key) const
{
    const auto& container = m_service.container();
    if(auto it_data = container.find({_key, {}}); it_data != container.end())
    {
        return it_data->second->optional();
    }

    return false;
}

//-----------------------------------------------------------------------------

} // namespace sight::service::detail
