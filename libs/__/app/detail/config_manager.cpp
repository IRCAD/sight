/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "app/detail/config_manager.hpp"

#include "app/extension/config.hpp"
#include "service/extension/factory.hpp"
#include "app/helper/config.hpp"
#include "service/op.hpp"
#include "service/manager.hpp"
#include "service/registry.hpp"

#include <core/com/proxy.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/exit_exception.hpp>
#include <core/runtime/runtime.hpp>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <data/object.hpp>

#include <boost/range/iterator_range_core.hpp>
#include <boost/thread/futures/wait_for_all.hpp>
#include <ranges>

namespace sight::app::detail
{

// ------------------------------------------------------------------------

static const core::com::slots::key_t ADD_OBJECTS_SLOT        = "addObject";
static const core::com::slots::key_t REMOVE_OBJECTS_SLOT     = "removeObjects";
static const core::com::slots::key_t ADD_STARTED_SRV_SLOT    = "addStartedService";
static const core::com::slots::key_t REMOVE_STARTED_SRV_SLOT = "removeStartedService";

// ------------------------------------------------------------------------

config_manager::config_manager()
{
    new_slot(ADD_OBJECTS_SLOT, &config_manager::add_objects, this);
    new_slot(REMOVE_OBJECTS_SLOT, &config_manager::remove_objects, this);
    new_slot(
        ADD_STARTED_SRV_SLOT,
        [this](service::base::wptr _srv)
        {
            core::mt::scoped_lock lock(m_mutex);
            if(auto srv = _srv.lock(); srv)
            {
                m_started_srv.push_back(srv);
            }
        });
    new_slot(
        REMOVE_STARTED_SRV_SLOT,
        [this](service::base::wptr _srv)
        {
            core::mt::scoped_lock lock(m_mutex);
            if(auto srv = _srv.lock(); srv)
            {
                std::erase_if(
                    m_started_srv,
                    [&srv](auto& _x)
                {
                    return srv == _x.lock();
                });
            }
        });
}

// ------------------------------------------------------------------------

config_manager::~config_manager()
{
    SIGHT_ASSERT("Manager must be stopped before destruction.", m_state == state_destroyed);
}

// ------------------------------------------------------------------------

void config_manager::set_config(
    const std::string& _config_id,
    const field_adaptor_t& _replace_fields,
    bool _auto_prefix_id
)
{
    m_config_id = _config_id;
    m_cfg_elem  =
        extension::config::get_default()->get_adapted_template_config(_config_id, _replace_fields, _auto_prefix_id);
}

// ------------------------------------------------------------------------

void config_manager::launch()
{
    FW_PROFILE("launch");

    try
    {
        this->start_module();

        {
            core::mt::scoped_lock lock(m_mutex);
            this->create();
            this->start();
        }
        this->update();
    }
    catch(const core::runtime::exit_exception& e)
    {
        SIGHT_DEBUG("Exit exception caught. Exit code:" << e.what());

        // To ensure proper destruction of the manager
        m_state = state_started;
    }
}

// ------------------------------------------------------------------------

void config_manager::stop_and_destroy()
{
    this->stop();
    this->destroy();
}

// ------------------------------------------------------------------------

void config_manager::start_module()
{
    SIGHT_ERROR_IF("Module is not specified, it can not be started.", m_config_id.empty());
    if(!m_config_id.empty())
    {
        std::shared_ptr<core::runtime::module> module = extension::config::get_default()->get_module(m_config_id);
        module->start();
    }
}

// ------------------------------------------------------------------------

void config_manager::create()
{
    SIGHT_ASSERT("Manager already running.", m_state == state_destroyed);

    m_add_object_connection    = connect_register_out(this->slot(ADD_OBJECTS_SLOT));
    m_remove_object_connection = connect_unregister_out(this->slot(REMOVE_OBJECTS_SLOT));

    this->create_objects(m_cfg_elem);
    this->create_connections();
    this->create_services(m_cfg_elem);

    m_state = state_created;
}

// ------------------------------------------------------------------------

void config_manager::start()
{
    SIGHT_ASSERT("Manager must be created first.", m_state == state_created || m_state == state_stopped);

    core::com::has_slots::m_slots.set_worker(core::thread::get_default_worker());

    this->process_start_items();
    for(auto& created_object : m_created_objects)
    {
        if(created_object.second.second)
        {
            created_object.second.second->start_config();
        }
    }

    m_state = state_started;
}

// ------------------------------------------------------------------------

void config_manager::update()
{
    this->process_update_items();
    for(auto& created_object : m_created_objects)
    {
        if(created_object.second.second)
        {
            created_object.second.second->update_config();
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::stop()
{
    SIGHT_ASSERT("Manager is not started, cannot stop.", m_state == state_started);

    m_add_object_connection.disconnect();
    m_remove_object_connection.disconnect();

    std::vector<service::base::shared_future_t> futures;
    {
        core::mt::scoped_lock lock(m_mutex);

        // Disconnect configuration connections
        this->destroy_proxies();

        for(auto& created_object : m_created_objects)
        {
            if(created_object.second.second)
            {
                created_object.second.second->stop_config();
            }
        }

        for(auto& w_srv : std::views::reverse(m_started_srv))
        {
            const service::base::sptr srv = w_srv.lock();
            SIGHT_ASSERT("Service expired.", srv);

            // The service can have been stopped just before...
            // This is a rare case, but nothing can really prevent that. So we just warn the developer, because if it
            // was not expected, at least he has a notice in the log
            if(srv->stopped())
            {
                SIGHT_WARN("Service " << srv->get_id() << " already stopped.");
            }
            else
            {
                futures.emplace_back(srv->stop());
            }
        }

        m_started_srv.clear();
        m_state = state_stopped;
    }
    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));

    app::helper::config::clear_props();
}

// ------------------------------------------------------------------------

void config_manager::destroy()
{
    SIGHT_ASSERT("Manager is not stopped, cannot destroy.", m_state == state_stopped || m_state == state_created);

    for(auto& created_object : m_created_objects)
    {
        if(created_object.second.second)
        {
            created_object.second.second->destroy_config();
        }
    }

    this->destroy_created_services();

    SIGHT_DEBUG(
        "Parsing OSR after destroying the config :" << std::endl
        << service::get_registry_information()
    );

    m_cfg_elem.clear();
    m_created_objects.clear();
    m_deferred_objects.clear();
    m_deferred_services.clear();
    m_deferred_start_srv.clear();
    m_deferred_update_srv.clear();
    m_services_proxies.clear();

    m_state = state_destroyed;
}

// ------------------------------------------------------------------------

void config_manager::add_existing_deferred_object(const data::object::sptr& _obj, const std::string& _uid)
{
    SIGHT_ASSERT(
        "Existing deferred objects must be added before starting the configuration, it's useless to do it later",
        m_state == state_destroyed
    );
    deferred_object_t deferred_object;
    deferred_object.m_object = _obj;
    m_deferred_objects.insert(std::make_pair(_uid, deferred_object));
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::get_config_root() const
{
    if(m_created_objects.empty())
    {
        return nullptr;
    }

    return m_created_objects.begin()->second.first;
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::find_object(const std::string& _uid, std::string_view _err_msg_tail) const
{
#ifndef _DEBUG
    SIGHT_NOT_USED(_err_msg_tail);
#endif
    data::object::sptr obj;

    // Look first in objects created in this appConfig
    auto it = m_created_objects.find(_uid);
    if(it != m_created_objects.end())
    {
        obj = it->second.first;
    }
    else
    {
        // Not found, now look in the objects that were marked as "deferred"
        auto it_deferred_obj = m_deferred_objects.find(_uid);

        SIGHT_ASSERT(
            this->msg_head() + "Object '" << _uid << "' has not been found" << _err_msg_tail,
            it_deferred_obj != m_deferred_objects.end()
        );
        obj = it_deferred_obj->second.m_object;
    }

    return obj;
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::get_new_object(config_attribute_t _type, config_attribute_t _uid)
{
    // Building object structure
    SPTR(core::runtime::extension) ext = core::runtime::find_extension(_type.first);
    if(ext)
    {
        const std::string class_name = core::get_classname<data::object>();
        SIGHT_ASSERT(
            "Extension and classname are different.",
            ext->point() == class_name
        );

        // Start dll to retrieve proxy and register object
        ext->get_module()->start();
    }

    data::object::sptr obj = data::factory::make(_type.first);
    SIGHT_ASSERT("factory failed to build object : " + _type.first, obj);

    if(_uid.second)
    {
        SIGHT_ASSERT("Object already has an UID.", !obj->has_id());
        SIGHT_ASSERT("UID " << _uid.first << " already exists.", !core::id::exist(_uid.first));
        obj->set_id(_uid.first);
    }

    return obj;
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::get_new_object(config_attribute_t _type, const std::string& _uid) const
{
    return this->get_new_object(_type, config_attribute_t(_uid, true));
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::get_object(config_attribute_t _type, const std::string& _uid) const
{
    SIGHT_ASSERT(this->msg_head() + "Object with UID \"" + _uid + "\" doesn't exist.", core::id::exist(_uid));
    auto obj = std::dynamic_pointer_cast<data::object>(core::id::get_object(_uid));

    SIGHT_ASSERT(this->msg_head() + "The UID '" + _uid + "' does not reference any object.", obj);

    if(_type.second)
    {
        SIGHT_ASSERT(
            this->msg_head() + "Object with UID \"" + _uid
            + "\" has a different type (\"" + obj->get_classname() + "\" != \"" + _type.first + "\").",
            obj->is_a(_type.first)
        );
    }

    return obj;
}

// ------------------------------------------------------------------------

service::base::sptr config_manager::get_new_service(const std::string& _uid, const std::string& _impl_type) const
{
    auto srv_factory = service::extension::factory::get();

    service::base::sptr srv = srv_factory->create(_impl_type);

    SIGHT_ASSERT("factory could not create service of type <" + _impl_type + ">.", srv);
    SIGHT_ASSERT("Service already has an UID.", !srv->has_id());

    SIGHT_ASSERT(this->msg_head() + "UID " + _uid + " already exists.", !core::id::exist(_uid));
    if(!_uid.empty())
    {
        srv->set_id(_uid);
    }

    return srv;
}

// ------------------------------------------------------------------------

void config_manager::destroy_created_services()
{
    for(auto& w_srv : std::views::reverse(m_created_srv))
    {
        const service::base::sptr srv = w_srv.lock();
        SIGHT_ASSERT("Service expired.", srv);

        // Force stopping services. It can happen for deferred services and when the associated data is not yet deleted.
        // Anyway, we must stop them before destroying them in all cases.
        if(!srv->stopped())
        {
            SIGHT_WARN("Service " << srv->get_id() << " is still running and will be stopped.")
            srv->stop().wait();
        }

        service::unregister_service(srv);
    }

    m_created_srv.clear();

    std::ranges::for_each(m_created_workers, [](auto& _x){core::thread::remove_worker(_x);});
    m_created_workers.clear();
}

// ------------------------------------------------------------------------

void config_manager::process_start_items()
{
    std::vector<service::base::shared_future_t> futures;

    for(const auto& elem : m_cfg_elem)
    {
        if(elem.first == "start")
        {
            const auto uid = elem.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!core::id::exist(uid))
            {
                if(m_deferred_services.find(uid) != m_deferred_services.end())
                {
                    m_deferred_start_srv.push_back(uid);
                    SIGHT_DEBUG(
                        this->msg_head() + "Start for service '" + uid + "' will be deferred since at least one "
                                                                         "of its data is missing. With DEBUG log level, you can know which are the "
                                                                         "missing objects."
                    );
                }
                else
                {
                    SIGHT_FATAL(
                        this->msg_head() + "Start is requested for service '" + uid
                        + "', but it does not exist."
                    );
                }
            }
            else
            {
                const service::base::sptr srv = service::get(uid);
                SIGHT_FATAL_IF(
                    this->msg_head() + "Start is requested for service '" + uid + "', though this identifier "
                                                                                  "exists, this is not a service.",
                    !srv
                );

                futures.emplace_back(srv->start());

                m_started_srv.push_back(srv);
            }
        }
    }

    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));
}

// ------------------------------------------------------------------------

void config_manager::process_update_items()
{
    std::vector<service::base::shared_future_t> futures;

    for(const auto& elem : m_cfg_elem)
    {
        if(elem.first == "update")
        {
            const auto uid = elem.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!core::id::exist(uid))
            {
                if(m_deferred_services.find(uid) != m_deferred_services.end())
                {
                    m_deferred_update_srv.push_back(uid);
                    SIGHT_DEBUG(
                        this->msg_head() + "Update for service '" + uid + "'will be deferred since at least one "
                                                                          "of its data is missing. With DEBUG log level, you can know which are the "
                                                                          "missing objects."
                    );
                }
                else
                {
                    SIGHT_FATAL(
                        this->msg_head() + "Update is requested for service '" + uid
                        + "', but it does not exist."
                    );
                }
            }
            else
            {
                const service::base::sptr srv = service::get(uid);
                SIGHT_FATAL_IF(
                    this->msg_head() + "Update is requested for service '" + uid
                    + "', though this identifier exists, this is not a service.",
                    !srv
                );

                futures.emplace_back(srv->update());
            }
        }
    }

    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));
}

// ------------------------------------------------------------------------

void config_manager::create_objects(const core::runtime::config_t& _cfg_elem)
{
    for(const auto& elem : _cfg_elem)
    {
        if(elem.first == "object")
        {
            // Get attributes

            // Id
            config_attribute_t id("", false);
            if(const auto uid = elem.second.get_optional<std::string>("<xmlattr>.uid"); uid.has_value())
            {
                id.first = uid.value();
                SIGHT_ASSERT(this->msg_head() + "\"uid\" attribute is empty.", !id.first.empty());
                id.second = true;
            }

            // Type
            config_attribute_t type("", false);
            if(const auto type_cfg = elem.second.get_optional<std::string>("<xmlattr>.type"); type_cfg.has_value())
            {
                type.first = type_cfg.value();
                SIGHT_ASSERT(this->msg_head() + "\"type\" attribute is empty.", !type.first.empty());
                type.second = true;
            }

            // Build mode
            config_attribute_t build_mode("", false);
            if(const auto build_mode_cfg =
                   elem.second.get_optional<std::string>("<xmlattr>.src"); build_mode_cfg.has_value())
            {
                build_mode.first = build_mode_cfg.value();
                SIGHT_ASSERT("this->msgHead() + \"src\" attribute is empty.", !build_mode.first.empty());

                SIGHT_ASSERT(
                    "Unhandled build mode (bad \"src\" attribute). Must be \"new\", \"deferred\" or \"ref\".",
                    build_mode.first == "ref" || build_mode.first == "src" || build_mode.first == "deferred"
                );
                build_mode.second = true;
            }

            if(build_mode.first == "deferred")
            {
                SIGHT_ASSERT(this->msg_head() + "Missing attribute \"id\".", id.second);
                const auto ret = m_deferred_objects.insert(std::make_pair(id.first, deferred_object_t()));
                SIGHT_NOT_USED(ret);
                SIGHT_DEBUG_IF(
                    this->msg_head() + "Object '" + id.first + "' already exists in this config.",
                    !ret.second
                );
            }
            else
            {
                // Creation of a new object
                data::object::sptr obj;
                service::object_parser::sptr obj_parser;

                // Create new or get the referenced object
                if(build_mode.second && build_mode.first == "ref")
                {
                    SIGHT_ASSERT(this->msg_head() + "Missing attribute \"id\".", id.second);
                    obj = this->get_object(type, id.first);
                }
                else
                {
                    obj = this->get_new_object(type, id);

                    // Get the object parser associated with the object type
                    const auto srv_factory = service::extension::factory::get();

                    std::string srv_impl = srv_factory->get_default_implementation_id_from_object_and_type(
                        obj->get_classname(),
                        "sight::service::object_parser"
                    );

                    service::base::sptr srv = srv_factory->create(srv_impl);
                    obj_parser = std::dynamic_pointer_cast<service::object_parser>(srv);
                    obj_parser->set_object_config(elem.second);
                    obj_parser->create_config(obj);
                }

                // If there is no uid defined in the config, we use the one generated from get_id()
                const auto real_id = id.second ? id.first : obj->get_id();
                m_created_objects[real_id] = std::make_pair(obj, obj_parser);
            }
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::create_services(const core::runtime::config_t& _cfg_elem)
{
    for(const auto& service_cfg : boost::make_iterator_range(_cfg_elem.equal_range("service")))
    {
        // Parse the service configuration
        auto srv_config = app::helper::config::parse_service(service_cfg.second, this->msg_head());

        // Check if we can start the service now or if we must deferred its creation
        bool create_service = true;
        std::vector<std::string> uids;

        for(const auto& [key, objectCfg] : srv_config.m_objects)
        {
            // If the current service uses an object that is marked as deferred, this means
            // we will have to manage automatically the start/stop and the connections
            auto it = m_deferred_objects.find(objectCfg.m_uid);
            if(it != m_deferred_objects.end())
            {
                it->second.m_services_cfg.emplace_back(srv_config);
                uids.push_back(objectCfg.m_uid);
                m_deferred_services.insert(srv_config.m_uid);

                // Do not start the service if any non-optional object is not yet present
                if(!objectCfg.m_optional && !core::id::exist(objectCfg.m_uid))
                {
                    create_service = false;
                }
            }

            // Extra check to warn the user that an object is used as output but not marked as deferred
            if(objectCfg.m_access == data::access::out)
            {
                SIGHT_ERROR_IF(
                    this->msg_head() + "Object '" + objectCfg.m_uid + "' is used as output in service '"
                    + srv_config.m_uid + "' but it not declared as 'deferred'.",
                    it == m_deferred_objects.end()
                );
            }
        }

        if(create_service)
        {
            this->create_service(srv_config);
        }
        else
        {
            // Check if a service hasn't been already created with this uid
            SIGHT_ASSERT(
                this->msg_head() + "UID " + srv_config.m_uid + " already exists.",
                !core::id::exist(srv_config.m_uid)
            );

            const std::string msg = config_manager::get_uid_list_as_string(uids);
            SIGHT_DEBUG(
                this->msg_head() + "Service '" + srv_config.m_uid
                + "' has not been created because the object" + msg + "not available."
            );
        }
    }

    for(const auto& service_cfg : boost::make_iterator_range(_cfg_elem.equal_range("serviceList")))
    {
        this->create_services(service_cfg.second);
    }
}

// ------------------------------------------------------------------------

service::base::sptr config_manager::create_service(const detail::service_config& _srv_config)
{
    // Create and bind service
    service::base::sptr srv = this->get_new_service(_srv_config.m_uid, _srv_config.m_type);
    service::register_service(srv);
    m_created_srv.push_back(srv);

    if(!_srv_config.m_worker.empty())
    {
        core::thread::worker::sptr worker = core::thread::get_worker(_srv_config.m_worker);
        if(!worker)
        {
            worker = core::thread::worker::make();
            worker->set_thread_name(_srv_config.m_worker);

            core::thread::add_worker(_srv_config.m_worker, worker);
            m_created_workers.push_back(worker);
        }

        srv->set_worker(worker);
    }

    const std::string err_msg_tail = " when creating service '" + _srv_config.m_uid + "'.";
    srv->set_config(_srv_config.m_config);

    for(const auto& [key, objectCfg] : _srv_config.m_objects)
    {
        set_deferred_id(srv, key.first, objectCfg.m_uid, key.second);

        data::object::sptr obj = this->find_object(objectCfg.m_uid, err_msg_tail);

        SIGHT_ASSERT(
            this->msg_head() + "Object '" << objectCfg.m_uid << "' has not been found" << err_msg_tail,
            obj || objectCfg.m_optional
        );
        if((obj || !objectCfg.m_optional) && objectCfg.m_access != data::access::out)
        {
            set_object(
                srv,
                obj,
                key.first,
                key.second,
                objectCfg.m_access,
                objectCfg.m_auto_connect || _srv_config.m_global_auto_connect,
                objectCfg.m_optional
            );
        }
    }

    bool has_start_connection = false;
    bool has_stop_connection  = false;
    // Set the proxies
    const auto& it_srv_proxy = m_services_proxies.find(_srv_config.m_uid);
    if(it_srv_proxy != m_services_proxies.end())
    {
        for(const auto& it_proxy : it_srv_proxy->second.m_proxy_cnt)
        {
            add_connection(srv, it_proxy.second);

            // Check if we need to monitor the start/stop state because of connections
            for(const auto& slot : it_proxy.second.m_slots)
            {
                if(slot.first == _srv_config.m_uid)
                {
                    if(slot.second == service::slots::START)
                    {
                        has_start_connection = true;
                        break;
                    }
                    // NOLINTNEXTLINE(llvm-else-after-return,readability-else-after-return)
                    else if(slot.second == service::slots::STOP)
                    {
                        has_stop_connection = true;
                        break;
                    }
                }
            }
        }
    }

    // Configure
    srv->configure();

    if(has_start_connection)
    {
        srv->signal(service::signals::STARTED)->connect(this->slot(ADD_STARTED_SRV_SLOT));
    }

    if(has_stop_connection)
    {
        srv->signal(service::signals::STOPPED)->connect(this->slot(REMOVE_STARTED_SRV_SLOT));
    }

    return srv;
}

// ------------------------------------------------------------------------

void config_manager::create_connections()
{
    for(const auto& elem : m_cfg_elem)
    {
        if(elem.first == "connect")
        {
            // Parse all connections
            auto gen_id_fn = [this](){return "proxy_" + this->get_id() + "_" + std::to_string(m_proxy_id++);};

            proxy_connections_t connection_infos = app::helper::config::parse_connections2(
                elem.second,
                this->msg_head(),
                gen_id_fn
            );

            // Proxy that is used for non-deferred connections
            proxy_connections_t created_objects_proxy(connection_infos.m_channel);

            // Register signals
            for(const auto& signal_info : connection_infos.m_signals)
            {
                auto it_deferred_obj = m_deferred_objects.find(signal_info.first);
                if(it_deferred_obj != m_deferred_objects.end())
                {
                    // Deferred Object
                    proxy_connections_t& proxy = it_deferred_obj->second.m_proxy_cnt[connection_infos.m_channel];
                    proxy.add_signal_connection(signal_info);
                }
                else
                {
                    auto it_obj = m_created_objects.find(signal_info.first);
                    if(it_obj != m_created_objects.end())
                    {
                        // Regular object
                        created_objects_proxy.add_signal_connection(signal_info);
                    }
                    else
                    {
                        // Service
                        auto& it_srv               = m_services_proxies[signal_info.first];
                        proxy_connections_t& proxy = it_srv.m_proxy_cnt[connection_infos.m_channel];
                        proxy.add_signal_connection(signal_info);
                        proxy.m_channel = connection_infos.m_channel;
                    }
                }
            }

            // Register slots
            for(const auto& slot_info : connection_infos.m_slots)
            {
                auto it_deferred_obj = m_deferred_objects.find(slot_info.first);
                if(it_deferred_obj != m_deferred_objects.end())
                {
                    // Deferred Object
                    proxy_connections_t& proxy = it_deferred_obj->second.m_proxy_cnt[connection_infos.m_channel];
                    proxy.add_slot_connection(slot_info);
                }
                else
                {
                    auto it_obj = m_created_objects.find(slot_info.first);
                    if(it_obj != m_created_objects.end())
                    {
                        // Regular object
                        created_objects_proxy.add_slot_connection(slot_info);
                    }
                    else
                    {
                        // Service
                        auto& it_srv               = m_services_proxies[slot_info.first];
                        proxy_connections_t& proxy = it_srv.m_proxy_cnt[connection_infos.m_channel];
                        proxy.add_slot_connection(slot_info);
                        proxy.m_channel = connection_infos.m_channel;
                    }
                }
            }

            m_created_objects_proxies[connection_infos.m_channel] = created_objects_proxy;
            this->connect_proxy(connection_infos.m_channel, created_objects_proxy);
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::destroy_proxy(
    const std::string& _channel,
    const proxy_connections_t& _proxy_cfg,
    const std::string& _key,
    data::object::csptr _hint_obj
)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& signal_elt : _proxy_cfg.m_signals)
    {
        if(_key.empty() || signal_elt.first == _key)
        {
            core::object::csptr obj = _hint_obj;
            if(obj == nullptr)
            {
                obj = core::id::get_object(signal_elt.first);
            }

            core::com::has_signals::csptr has_signals = std::dynamic_pointer_cast<const core::com::has_signals>(obj);
            SIGHT_ASSERT(this->msg_head() + "Signal source not found '" + signal_elt.first + "'", obj);

            core::com::signal_base::sptr sig = has_signals->signal(signal_elt.second);

            try
            {
                proxy->disconnect(_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signal_elt.second + "' from '" + signal_elt.first + "' can not be disconnected "
                                                                                     "from the channel '" + _channel + "': " + std::string(
                        e.what()
                                                                                     )
                );
            }
        }
    }

    for(const auto& slot_elt : _proxy_cfg.m_slots)
    {
        if(_key.empty() || slot_elt.first == _key)
        {
            core::object::sptr obj               = core::id::get_object(slot_elt.first);
            core::com::has_slots::sptr has_slots = std::dynamic_pointer_cast<core::com::has_slots>(obj);
            SIGHT_ASSERT(this->msg_head() + "Slot destination not found '" + slot_elt.first + "'", has_slots);

            core::com::slot_base::sptr slot = has_slots->slot(slot_elt.second);

            try
            {
                proxy->disconnect(_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slot_elt.second + "' from '" + slot_elt.first + "' can not be disconnected from the "
                                                                               "channel '" + _channel + "': " + std::string(
                        e.what()
                                                                               )
                );
            }
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::destroy_proxies()
{
    // Remove local proxies from deferred objects
    for(auto& it_deferred_obj : m_deferred_objects)
    {
        if(it_deferred_obj.second.m_object)
        {
            for(const auto& it_proxy : it_deferred_obj.second.m_proxy_cnt)
            {
                this->destroy_proxy(
                    it_proxy.first,
                    it_proxy.second,
                    it_deferred_obj.first,
                    it_deferred_obj.second.m_object
                );
            }

            it_deferred_obj.second.m_object.reset();
        }
    }

    // Remove local proxies from all created objects
    for(const auto& it_proxy : m_created_objects_proxies)
    {
        this->destroy_proxy(it_proxy.first, it_proxy.second);
    }

    m_created_objects_proxies.clear();
}

//------------------------------------------------------------------------------

void config_manager::add_objects(data::object::sptr _obj, const std::string& _id)
{
    SIGHT_ASSERT("Object id can not be empty", !_id.empty());

    core::mt::scoped_lock lock(m_mutex);
    if(m_state != state_started)
    {
        SIGHT_INFO("Skip processing of a new object since the config is not running.");
        return;
    }

    FW_PROFILE("addObjects");

    // Local map used to process services only once
    std::map<std::string, const detail::service_config*> services_map_cfg;

    // Local vector used to store services and keep the declare order (we could use only this one but the map is used
    // to speedup the search
    std::vector<const detail::service_config*> services_cfg;

    // Are there services that were waiting for this object ?
    auto it_deferred_obj = m_deferred_objects.find(_id);
    if(it_deferred_obj != m_deferred_objects.end())
    {
        // For each service waiting to be started
        for(const auto& srv_cfg : it_deferred_obj->second.m_services_cfg)
        {
            if(services_map_cfg.find(srv_cfg.m_uid) == services_map_cfg.end())
            {
                services_map_cfg[srv_cfg.m_uid] = &srv_cfg;
                services_cfg.push_back(&srv_cfg);
            }
        }

        // Connect signals of this deferred object
        it_deferred_obj->second.m_object = _obj;

        for(const auto& connect_cfg : it_deferred_obj->second.m_proxy_cnt)
        {
            this->connect_proxy(connect_cfg.first, connect_cfg.second);
        }
    }

    std::unordered_map<std::string, service::base::sptr> new_services;

    for(const auto& it_service : services_cfg)
    {
        const auto* srv_cfg = it_service;
        SIGHT_ASSERT("config is null", srv_cfg);
        const auto& uid = srv_cfg->m_uid;

        bool create_service = true;

        // Look for all objects (there could be more than the current object) and check if they are all created
        for(const auto& [key, objCfg] : srv_cfg->m_objects)
        {
            // Look first in objects created in this appConfig
            if(m_created_objects.find(objCfg.m_uid) == m_created_objects.end())
            {
                // Not found, now look in the objects that were marked as "deferred"
                const auto it_local_deferred_obj = m_deferred_objects.find(objCfg.m_uid);
                SIGHT_ASSERT(
                    this->msg_head() + "Object '" + objCfg.m_uid + "' used by service '" + uid
                    + "' has not been declared in this config.",
                    it_local_deferred_obj != m_deferred_objects.end()
                );

                const auto object = it_local_deferred_obj->second.m_object;
                if(object == nullptr)
                {
                    if(!objCfg.m_optional)
                    {
                        create_service = false;

                        SIGHT_INFO(
                            this->msg_head() + "Service '" + uid + "' has not been created because the "
                                                                   "object" + objCfg.m_uid + " is not available."
                        );
                    }
                }
                else if(core::id::exist(uid))
                {
                    service::base::sptr srv = service::get(uid);
                    SIGHT_ASSERT(this->msg_head() + "No service registered with UID \"" + uid + "\".", srv);

                    // We have an optional object
                    if(objCfg.m_optional)
                    {
                        // Check if we already registered an object at this key
                        auto registered_obj = srv->has_data::object(key.first, objCfg.m_access, key.second);
                        if(registered_obj != nullptr)
                        {
                            // If this is not the object we have to swap, then unregister it
                            if(registered_obj != object)
                            {
                                reset_object(srv, key.first, key.second);
                            }
                        }

                        if(registered_obj != object)
                        {
                            // We need to register the deferred id for ptr_vector, in case we are running
                            // a remove_object/add_object sequence
                            set_deferred_id(srv, key.first, objCfg.m_uid, key.second);

                            // Register the key on the service
                            set_object(
                                srv,
                                object,
                                key.first,
                                key.second,
                                objCfg.m_access,
                                objCfg.m_auto_connect || srv_cfg->m_global_auto_connect,
                                objCfg.m_optional
                            );

                            if(srv->started())
                            {
                                // Call the swapping callback of the service and wait for it
                                srv->swap_key(
                                    objCfg.m_key,
                                    std::const_pointer_cast<data::object>(registered_obj)
                                ).wait();
                            }
                        }
                    }

                    create_service = false;
                }
            }
        }

        // All the data for this service is ready, create and run it
        if(create_service)
        {
            // Create the service
            new_services.emplace(uid, this->create_service(*srv_cfg));

            // Debug message
            SIGHT_INFO(
                this->msg_head() + "Service '" + uid + "' has been automatically created because its "
                                                       "objects are all available."
            );
        }
    }

    std::vector<service::base::shared_future_t> futures;

    // Start services according to the order given in the configuration
    for(const auto& uid : m_deferred_start_srv)
    {
        auto it_srv = new_services.find(uid);
        if(it_srv != new_services.end())
        {
            futures.push_back(it_srv->second->start());
            m_started_srv.push_back(it_srv->second);

            // Debug message
            SIGHT_INFO(
                this->msg_head() + "Service '" + uid + "' has been automatically started because its "
                                                       "objects are all available."
            );
        }
    }

    boost::wait_for_all(futures.begin(), futures.end());
    futures.clear();

    // Update services according to the order given in the configuration
    for(const auto& uid : m_deferred_update_srv)
    {
        auto it_srv = new_services.find(uid);
        if(it_srv != new_services.end())
        {
            futures.push_back(it_srv->second->update());

            // Debug message
            SIGHT_INFO(
                this->msg_head() + "Service '" + uid + "' has been automatically update because its "
                                                       "objects are all available."
            );
        }
    }

    boost::wait_for_all(futures.begin(), futures.end());
}

//------------------------------------------------------------------------------

void config_manager::remove_objects(data::object::sptr _obj, const std::string& _id)
{
    SIGHT_ASSERT("Object id can not be empty", !_id.empty());

    core::mt::scoped_lock lock(m_mutex);
    if(m_state != state_started)
    {
        SIGHT_INFO("Skip processing of a new object since the config is not running.");
        return;
    }

    FW_PROFILE("removeObjects");

    // Are there services that were connected with this object ?
    const auto it_deferred_obj = m_deferred_objects.find(_id);
    if(it_deferred_obj != m_deferred_objects.end())
    {
        for(const auto& it_proxy : it_deferred_obj->second.m_proxy_cnt)
        {
            this->destroy_proxy(it_proxy.first, it_proxy.second, _id, it_deferred_obj->second.m_object);
        }

        it_deferred_obj->second.m_object.reset();

        // Are there services that were using this object ?
        for(const auto& srv_cfg : it_deferred_obj->second.m_services_cfg)
        {
            if(core::id::exist(srv_cfg.m_uid))
            {
                // Check all objects, to know if this object is optional
                bool optional = true;

                for(const auto& [key, objCfg] : srv_cfg.m_objects)
                {
                    if(_id == objCfg.m_uid)
                    {
                        service::base::sptr srv = service::get(srv_cfg.m_uid);
                        SIGHT_ASSERT("No service registered with UID \"" << srv_cfg.m_uid << "\".", srv);

                        optional &= objCfg.m_optional;
                        if(objCfg.m_optional)
                        {
                            if(srv->has_data::object(key.first, objCfg.m_access, key.second))
                            {
                                reset_object(srv, key.first, key.second);

                                if(srv->started())
                                {
                                    srv->swap_key(objCfg.m_key, _obj).wait();
                                }
                            }
                        }
                    }
                }

                if(!optional)
                {
                    // 1. Stop the service
                    service::base::sptr srv = service::get(srv_cfg.m_uid);
                    SIGHT_ASSERT(this->msg_head() + "No service registered with UID \"" << srv_cfg.m_uid << "\".", srv);

                    std::erase_if(m_started_srv, [&srv](auto& _x){return srv == _x.lock();});

                    SIGHT_ASSERT("Service " << srv->get_id() << " already stopped.", !srv->stopped());
                    srv->stop().wait();

                    // 2. Destroy the service
                    SIGHT_ASSERT(
                        "Service " << srv->get_id() << " must be stopped before destruction.",
                        srv->stopped()
                    );
                    service::unregister_service(srv);

                    std::erase_if(m_created_srv, [&srv](auto& _x){return srv == _x.lock();});

                    sight::service::base::wptr check_srv = srv;
                    srv.reset();

                    SIGHT_ASSERT(
                        this->msg_head() + "The service '" + srv_cfg.m_uid
                        + "'' should have been destroyed, but someone "
                          "still holds a reference which prevents to destroy it properly.",
                        check_srv.expired()
                    );

                    SIGHT_INFO(
                        this->msg_head() + "Service '" + srv_cfg.m_uid
                        + "' has been stopped because the object "
                        + _id + " is no longer available."
                    );
                }
                else
                {
                    // Update auto connections
                    service::base::sptr srv = service::get(srv_cfg.m_uid);
                    SIGHT_ASSERT(this->msg_head() + "No service registered with UID \"" << srv_cfg.m_uid << "\".", srv);

                    auto_disconnect(srv);
                    auto_connect(srv);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void config_manager::connect_proxy(const std::string& _channel, const proxy_connections_t& _connect_cfg)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& signal_cfg : _connect_cfg.m_signals)
    {
        core::object::sptr sig_source = core::id::get_object(signal_cfg.first);
        if(sig_source == nullptr)
        {
            // We didn't found the object or service globally, let's try with local deferred objects
            auto it_deferred_obj = m_deferred_objects.find(signal_cfg.first);
            SIGHT_ASSERT(
                this->msg_head() + "Object '" + signal_cfg.first + "' not found.",
                it_deferred_obj != m_deferred_objects.end()
            );

            sig_source = it_deferred_obj->second.m_object;
        }

        core::com::has_signals::sptr has_signals = std::dynamic_pointer_cast<core::com::has_signals>(sig_source);
        SIGHT_ASSERT(this->msg_head() + "Signal source not found '" + signal_cfg.first + "'", has_signals);

        core::com::signal_base::sptr sig = has_signals->signal(signal_cfg.second);
        SIGHT_ASSERT("Signal '" + signal_cfg.second + "' not found in source '" + signal_cfg.first + "'.", sig);

        try
        {
            proxy->connect(_channel, sig);
        }
        catch(const std::exception& e)
        {
            SIGHT_ERROR(
                "Signal '" + signal_cfg.second + "' from '" + signal_cfg.first + "' can not be connected to the "
                                                                                 "channel '" + _channel + "': " + std::string(
                    e.what()
                                                                                 )
            );
        }
    }

    for(const auto& slot_cfg : _connect_cfg.m_slots)
    {
        core::object::sptr slot_dest         = core::id::get_object(slot_cfg.first);
        core::com::has_slots::sptr has_slots = std::dynamic_pointer_cast<core::com::has_slots>(slot_dest);
        SIGHT_ASSERT(this->msg_head() + "Slot destination not found '" + slot_cfg.first + "'", has_slots);

        core::com::slot_base::sptr slot = has_slots->slot(slot_cfg.second);
        SIGHT_ASSERT("Slot '" + slot_cfg.second + "' not found in source '" + slot_cfg.first + "'.", slot);

        try
        {
            proxy->connect(_channel, slot);
        }
        catch(const std::exception& e)
        {
            SIGHT_ERROR(
                "Slot '" + slot_cfg.second + "' from '" + slot_cfg.first + "' can not be connected to the "
                                                                           "channel '" + _channel + "': "
                + std::string(e.what())
            );
        }
    }
}

//------------------------------------------------------------------------------

std::string config_manager::get_uid_list_as_string(const std::vector<std::string>& _uid_list)
{
    std::string msg = _uid_list.size() == 1 ? " '" : "s '";
    msg += _uid_list[0];
    for(auto it = _uid_list.begin() + 1 ; it < _uid_list.end() ; ++it)
    {
        msg += "', '" + *it;
    }

    msg.append(_uid_list.size() == 1 ? "' is " : "' are ");

    return msg;
}

//------------------------------------------------------------------------------

} // namespace sight::app::detail
