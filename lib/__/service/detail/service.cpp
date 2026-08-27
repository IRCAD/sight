/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <core/com/helper/sig_slot_connection.hpp>

#include <core/ptree.hpp>

#include <service/manager.hpp>
#include <service/value_parameters.hpp>

#include <algorithm>
#include <ranges>
#include <set>

namespace sight::service::detail
{

// To avoid any conflict with other slots
const auto MAKE_PROPERTY_SLOT_NAME = [](const std::string& _property){return core::id::join(_property, "property");};

//-----------------------------------------------------------------------------

service::service(sight::service::base& _service) :
    m_service(_service)
{
}

//-----------------------------------------------------------------------------

service::~service()
{
    SIGHT_ASSERT(
        "service " << m_id_copy << " not stopped upon destruction, call stop() beforehand",
        m_global_state == base::global_status::stopped
    );

    {
        const auto started = m_service.m_start_property.lock();
        if(started)
        {
            auto sig = started->signal(sight::data::signals::MODIFIED);
            if(auto conn = sig->get_connection(m_service.slot(sight::service::slots::START_ON_PROPERTY));
               not conn.expired())
            {
                conn.disconnect();
            }
        }
    }
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

                // Literal values given with the hierarchical syntax, i.e. <config value="10"/> for "config.value".
                // Entries that do not match a declared property are simply never looked up.
                for(const auto& entry : core::ptree::flatten(m_configuration, sight::service::manager::RESERVED_TAGS))
                {
                    properties_cfgs.emplace(entry.key, entry.value);
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
                            m_created_objects.emplace_back(new_obj);

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

                // Create a slot for each property - avoid recreating them if configured multiple times
                for(const auto& [key, ptr] : m_service.container())
                {
                    const auto& key_str = key.first;

                    if(dynamic_cast<data::property_base*>(ptr) != nullptr)
                    {
                        const auto slot_name = MAKE_PROPERTY_SLOT_NAME(std::string(key_str));
                        const auto& slots    = dynamic_cast<sight::core::com::has_slots&>(m_service).slots();
                        if(not slots.contains(slot_name))
                        {
                            auto slot = m_service.new_slot(
                                MAKE_PROPERTY_SLOT_NAME(std::string(key_str)),
                                [&]()
                                {
                                    m_service.on_property_set(key_str);
                                });
                            slot->set_worker(m_service.worker());
                        }
                    }
                }

                this->create_value_objects();

                m_service.configuring();
                m_service.configuring(m_configuration);
            }
            catch(const boost::property_tree::ptree_bad_path& e)
            {
                SIGHT_ERROR("Error while configuring the service '" + m_service.get_id() + "' : " + e.what());

                auto config = m_service.get_config();
                SIGHT_ERROR("With the given configuration:\n" + core::ptree::to_string(config));
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

        {
            const auto started = m_service.m_start_property.lock();
            auto sig           = started->signal(sight::data::signals::MODIFIED);
            if(auto conn = sig->get_connection(m_service.slot(sight::service::slots::START_ON_PROPERTY));
               conn.expired())
            {
                sig->connect(m_service.slot(sight::service::slots::START_ON_PROPERTY));
            }
        }
        m_configuration_state = base::configuration_status::configured;
    }

    m_connections.connect_start_slot(m_service);
}

//-----------------------------------------------------------------------------

void service::create_value_objects()
{
    // Collect the keys that are not bound to an object uid. Those are left empty on purpose by the application
    // configuration, so that we can fill them here, either from the literal value or from the declared default value.
    std::map<std::pair<std::string, std::optional<std::size_t> >, std::optional<std::string> > value_cfgs;

    // All the keys mentioned in the configuration, whether they declare a uid or not.
    std::set<std::string> declared_keys;

    for(const auto* data_keyword : {"in", "inout"})
    {
        const auto obj_cfgs = m_configuration.equal_range(data_keyword);
        for(auto obj_cfg = obj_cfgs.first ; obj_cfg != obj_cfgs.second ; ++obj_cfg)
        {
            if(const auto group = obj_cfg->second.get_optional<std::string>("<xmlattr>.group"); group.has_value())
            {
                declared_keys.insert(*group);

                // The index must be computed exactly like in app::helper::config::parse_service, i.e. every key
                // consumes one index, whether it declares a uid or a value.
                std::size_t index   = 0;
                const auto key_cfgs = obj_cfg->second.equal_range("key");
                for(auto key_cfg = key_cfgs.first ; key_cfg != key_cfgs.second ; ++key_cfg)
                {
                    const auto uid   = key_cfg->second.get_optional<std::string>("<xmlattr>.uid");
                    const auto value = key_cfg->second.get_optional<std::string>("<xmlattr>.value");
                    if(!uid.has_value() && value.has_value())
                    {
                        value_cfgs[{*group, index}] = *value;
                    }

                    ++index;
                }
            }
            else
            {
                const auto key = obj_cfg->second.get<std::string>("<xmlattr>.key", "");
                if(key.empty())
                {
                    // Not a data declaration, this is left to the application configuration parser.
                    continue;
                }

                declared_keys.insert(key);

                if(const auto uid = obj_cfg->second.get_optional<std::string>("<xmlattr>.uid"); !uid.has_value())
                {
                    const auto value = obj_cfg->second.get_optional<std::string>("<xmlattr>.value");
                    value_cfgs[{key, std::nullopt}] =
                        value.has_value() ? std::make_optional(*value) : std::nullopt;
                }
            }
        }
    }

    const auto& container = m_service.container();

    // Same thing with the hierarchical syntax, i.e. <config threshold="1.5"/> for the key "config.threshold". Here
    // there is no explicit distinction between a uid and a literal value, so a key already bound to an object by the
    // application configuration is left alone. Objects previously created by this service are updated instead.
    for(const auto& entry : core::ptree::flatten(m_configuration, sight::service::manager::RESERVED_TAGS))
    {
        const auto declaration = container.find({entry.key, {}});
        if(declaration == container.end())
        {
            continue;
        }

        declared_keys.insert(entry.key);

        const std::optional<std::size_t> index = declaration->second->is_group()
                                                 ? std::optional {entry.index} : std::nullopt;

        if(const auto bound = container.find({entry.key, index}); bound != container.end())
        {
            const auto current_obj        = std::const_pointer_cast<data::object>(bound->second->get());
            const bool created_by_service = current_obj
                                            && std::ranges::find(m_created_objects, current_obj)
                                            != m_created_objects.end();
            if(!created_by_service && (current_obj || !bound->second->deferred_id().empty()))
            {
                continue;
            }
        }

        value_cfgs[{entry.key, index}] = entry.value;
    }

    // Keys that are not mentioned at all in the configuration, but that declare a default value, are built as well.
    for(const auto& [id, ptr] : container)
    {
        const auto& [key, index] = id;
        if(!index.has_value() && !declared_keys.contains(std::string(key)) && ptr->make_default_object() != nullptr)
        {
            value_cfgs[{std::string(key), std::nullopt}] = std::nullopt;
        }
    }

    for(const auto& [id, value] : value_cfgs)
    {
        const auto& [key, index] = id;

        const auto declaration = container.find({key, {}});
        SIGHT_THROW_IF(
            "No data::ptr declared with key '" << key << "' in service '" << m_service.get_id() << "'.",
            declaration == container.end()
        );

        auto* const ptr = declaration->second;
        SIGHT_THROW_IF(
            "Key '" << key << "' of service '" << m_service.get_id()
            << "' is an output, it can not be declared with a value.",
            ptr->access() == data::access::out
        );

        const auto current_obj = std::const_pointer_cast<data::object>(ptr->get());
        if(value.has_value()
           && current_obj
           && std::ranges::find(m_created_objects, current_obj) != m_created_objects.end())
        {
            // Reuse an object previously created from a literal value. This keeps its identity stable while applying
            // the new configuration, and avoids retaining an obsolete object in m_created_objects.
            const auto serializable = std::dynamic_pointer_cast<data::string_serializable>(current_obj);
            SIGHT_ASSERT("Object created from a literal value is not string serializable", serializable);
            serializable->from_string(*value);
            serializable->set_default_value();
            continue;
        }

        if(!value.has_value() && current_obj)
        {
            // Already assigned, typically set programmatically before the service was configured.
            continue;
        }

        sight::data::object::sptr new_obj;

        if(value.has_value())
        {
            const auto object_type = m_service.resolve_object_type(key, index);
            if(!object_type.has_value())
            {
                // The service builds the object itself at a later stage, typically when it knows the configuration
                // the value will be forwarded to.
                continue;
            }

            SIGHT_THROW_IF(
                "Could not resolve the type of the object to build for key '" << key << "' of service '"
                << m_service.get_id() << "'. Either use a 'uid', or override resolve_object_type() in the service.",
                object_type->empty()
            );

            try
            {
                new_obj = sight::service::make_object_from_value(*object_type, *value);
            }
            catch(const std::exception& e)
            {
                SIGHT_THROW(
                    "Key '" << key << "' of service '" << m_service.get_id() << "' can not be built. " << e.what()
                );
            }
        }
        else
        {
            // No literal value, fall back on the default value declared with the data::ptr, if any. When there is
            // none, the key is simply left unassigned, like it was before, and the object may be set programmatically
            // or reported as missing when the service starts.
            new_obj = ptr->make_default_object();
            if(!new_obj)
            {
                continue;
            }
        }

        if(!new_obj->has_id())
        {
            // Give a deterministic identifier, so that the object can be referenced like any other one, typically
            // when it is forwarded to a sub-configuration.
            new_obj->set_id(
                core::id::join(m_service.get_id(), key, index.has_value() ? std::to_string(*index) : "value")
            );
        }

        m_created_objects.emplace_back(new_obj);
        // We do not connect created objects since they are not accessible from outside the service, and thus can not
        // be modified by other services.
        m_service.set_object(new_obj, key, index, ptr->access(), false, ptr->optional());
    }
}

//-----------------------------------------------------------------------------

base::shared_future_t service::start(bool _async)
{
    m_id_copy = m_service.get_id();
    if(m_configuration_state == base::configuration_status::unconfigured)
    {
        // Well we could be stricter and require this to be done before, but a lot of legacy code would need
        // to be fixed and I don't think this bring so much value
        this->configure();
    }

    SIGHT_ASSERT(
        "service " << m_service.get_id() << " requested to start, but it is not configured",
        m_configuration_state == base::configuration_status::configured
    );
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

    {
        const auto started = m_service.m_start_property.lock();
        *started = true;
        started->async_emit(&m_service, sight::data::signals::MODIFIED);
    }

    m_service.async_emit(sight::service::signals::STARTED, sight::service::base::wptr(m_service.get_sptr()));

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

    m_service.async_emit(sight::service::signals::STOPPED, sight::service::base::wptr(m_service.get_sptr()));

    {
        const auto started = m_service.m_start_property.lock();
        *started = false;
        started->async_emit(&m_service, sight::data::signals::MODIFIED);
    }

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

    m_service.async_emit(sight::service::signals::SWAPPED, sight::service::base::wptr(m_service.get_sptr()));

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

    m_service.async_emit(sight::service::signals::UPDATED, sight::service::base::wptr(m_service.get_sptr()));

    return future;
}

//-----------------------------------------------------------------------------

void service::auto_connect()
{
    sight::service::connections_t connection_map = m_service.auto_connections();
    m_auto_connected = false;

    if(m_service.get_id().ends_with("swap_target_tool_srv"))
    {
        SIGHT_WARN(
            "Service " << std::quoted(m_service.get_id())
            << " is auto-connected, this is likely to introduce timing issues."
        );
    }

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
                const auto sig = obj->signal<data::signals::modified_t>(data::signals::MODIFIED);

                auto slot = m_service.slot(MAKE_PROPERTY_SLOT_NAME(std::string(key_str)));
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
