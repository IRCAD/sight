/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
    new_slot(ADD_OBJECTS_SLOT, &config_manager::addObjects, this);
    new_slot(REMOVE_OBJECTS_SLOT, &config_manager::removeObjects, this);
    new_slot(
        ADD_STARTED_SRV_SLOT,
        [this](service::base::wptr _srv)
        {
            core::mt::scoped_lock lock(m_mutex);
            if(auto srv = _srv.lock(); srv)
            {
                m_startedSrv.push_back(srv);
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
                    m_startedSrv,
                    [&srv](auto& x)
                {
                    return srv == x.lock();
                });
            }
        });
}

// ------------------------------------------------------------------------

config_manager::~config_manager()
{
    SIGHT_ASSERT("Manager must be stopped before destruction.", m_state == STATE_DESTROYED);
}

// ------------------------------------------------------------------------

void config_manager::setConfig(
    const std::string& _configId,
    const field_adaptor_t& _replaceFields,
    bool autoPrefixId
)
{
    m_configId = _configId;
    m_cfgElem  =
        extension::config::getDefault()->getAdaptedTemplateConfig(_configId, _replaceFields, autoPrefixId);
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
        m_state = STATE_STARTED;
    }
}

// ------------------------------------------------------------------------

void config_manager::stopAndDestroy()
{
    this->stop();
    this->destroy();
}

// ------------------------------------------------------------------------

void config_manager::start_module()
{
    SIGHT_ERROR_IF("Module is not specified, it can not be started.", m_configId.empty());
    if(!m_configId.empty())
    {
        std::shared_ptr<core::runtime::module> module = extension::config::getDefault()->get_module(m_configId);
        module->start();
    }
}

// ------------------------------------------------------------------------

void config_manager::create()
{
    SIGHT_ASSERT("Manager already running.", m_state == STATE_DESTROYED);

    m_addObjectConnection    = connect_register_out(this->slot(ADD_OBJECTS_SLOT));
    m_removeObjectConnection = connect_unregister_out(this->slot(REMOVE_OBJECTS_SLOT));

    this->createObjects(m_cfgElem);
    this->createConnections();
    this->createServices(m_cfgElem);

    m_state = STATE_CREATED;
}

// ------------------------------------------------------------------------

void config_manager::start()
{
    SIGHT_ASSERT("Manager must be created first.", m_state == STATE_CREATED || m_state == STATE_STOPPED);

    core::com::has_slots::m_slots.set_worker(core::thread::get_default_worker());

    this->processStartItems();
    for(auto& createdObject : m_createdObjects)
    {
        if(createdObject.second.second)
        {
            createdObject.second.second->startConfig();
        }
    }

    m_state = STATE_STARTED;
}

// ------------------------------------------------------------------------

void config_manager::update()
{
    this->processUpdateItems();
    for(auto& createdObject : m_createdObjects)
    {
        if(createdObject.second.second)
        {
            createdObject.second.second->updateConfig();
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::stop()
{
    SIGHT_ASSERT("Manager is not started, cannot stop.", m_state == STATE_STARTED);

    m_addObjectConnection.disconnect();
    m_removeObjectConnection.disconnect();

    std::vector<service::base::shared_future_t> futures;
    {
        core::mt::scoped_lock lock(m_mutex);

        // Disconnect configuration connections
        this->destroyProxies();

        for(auto& createdObject : m_createdObjects)
        {
            if(createdObject.second.second)
            {
                createdObject.second.second->stopConfig();
            }
        }

        for(auto& w_srv : std::views::reverse(m_startedSrv))
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

        m_startedSrv.clear();
        m_state = STATE_STOPPED;
    }
    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));

    app::helper::config::clearProps();
}

// ------------------------------------------------------------------------

void config_manager::destroy()
{
    SIGHT_ASSERT("Manager is not stopped, cannot destroy.", m_state == STATE_STOPPED || m_state == STATE_CREATED);

    for(auto& createdObject : m_createdObjects)
    {
        if(createdObject.second.second)
        {
            createdObject.second.second->destroyConfig();
        }
    }

    this->destroyCreatedServices();

    SIGHT_DEBUG(
        "Parsing OSR after destroying the config :" << std::endl
        << service::getRegistryInformation()
    );

    m_cfgElem.clear();
    m_createdObjects.clear();
    m_deferredObjects.clear();
    m_deferredServices.clear();
    m_deferredStartSrv.clear();
    m_deferredUpdateSrv.clear();
    m_servicesProxies.clear();

    m_state = STATE_DESTROYED;
}

// ------------------------------------------------------------------------

void config_manager::addExistingDeferredObject(const data::object::sptr& _obj, const std::string& _uid)
{
    SIGHT_ASSERT(
        "Existing deferred objects must be added before starting the configuration, it's useless to do it later",
        m_state == STATE_DESTROYED
    );
    DeferredObjectType deferredObject;
    deferredObject.m_object = _obj;
    m_deferredObjects.insert(std::make_pair(_uid, deferredObject));
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::getConfigRoot() const
{
    if(m_createdObjects.empty())
    {
        return nullptr;
    }

    return m_createdObjects.begin()->second.first;
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::findObject(const std::string& uid, std::string_view errMsgTail) const
{
#ifndef _DEBUG
    SIGHT_NOT_USED(errMsgTail);
#endif
    data::object::sptr obj;

    // Look first in objects created in this appConfig
    auto it = m_createdObjects.find(uid);
    if(it != m_createdObjects.end())
    {
        obj = it->second.first;
    }
    else
    {
        // Not found, now look in the objects that were marked as "deferred"
        auto itDeferredObj = m_deferredObjects.find(uid);

        SIGHT_ASSERT(
            this->msgHead() + "Object '" << uid << "' has not been found" << errMsgTail,
            itDeferredObj != m_deferredObjects.end()
        );
        obj = itDeferredObj->second.m_object;
    }

    return obj;
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::getNewObject(ConfigAttribute type, ConfigAttribute uid)
{
    // Building object structure
    SPTR(core::runtime::extension) ext = core::runtime::find_extension(type.first);
    if(ext)
    {
        const std::string className = core::get_classname<data::object>();
        SIGHT_ASSERT(
            "Extension and classname are different.",
            ext->point() == className
        );

        // Start dll to retrieve proxy and register object
        ext->get_module()->start();
    }

    data::object::sptr obj = data::factory::make(type.first);
    SIGHT_ASSERT("factory failed to build object : " + type.first, obj);

    if(uid.second)
    {
        SIGHT_ASSERT("Object already has an UID.", !obj->has_id());
        SIGHT_ASSERT("UID " << uid.first << " already exists.", !core::tools::id::exist(uid.first));
        obj->set_id(uid.first);
    }

    return obj;
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::getNewObject(ConfigAttribute type, const std::string& uid) const
{
    return this->getNewObject(type, ConfigAttribute(uid, true));
}

// ------------------------------------------------------------------------

data::object::sptr config_manager::getObject(ConfigAttribute type, const std::string& uid) const
{
    SIGHT_ASSERT(this->msgHead() + "Object with UID \"" + uid + "\" doesn't exist.", core::tools::id::exist(uid));
    auto obj = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object(uid));

    SIGHT_ASSERT(this->msgHead() + "The UID '" + uid + "' does not reference any object.", obj);

    if(type.second)
    {
        SIGHT_ASSERT(
            this->msgHead() + "Object with UID \"" + uid
            + "\" has a different type (\"" + obj->get_classname() + "\" != \"" + type.first + "\").",
            obj->is_a(type.first)
        );
    }

    return obj;
}

// ------------------------------------------------------------------------

service::base::sptr config_manager::getNewService(const std::string& uid, const std::string& implType) const
{
    auto srvFactory = service::extension::factory::get();

    service::base::sptr srv = srvFactory->create(implType);

    SIGHT_ASSERT("factory could not create service of type <" + implType + ">.", srv);
    SIGHT_ASSERT("Service already has an UID.", !srv->has_id());

    SIGHT_ASSERT(this->msgHead() + "UID " + uid + " already exists.", !core::tools::id::exist(uid));
    if(!uid.empty())
    {
        srv->set_id(uid);
    }

    return srv;
}

// ------------------------------------------------------------------------

void config_manager::destroyCreatedServices()
{
    for(auto& w_srv : std::views::reverse(m_createdSrv))
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

        service::unregisterService(srv);
    }

    m_createdSrv.clear();

    std::ranges::for_each(m_createdWorkers, [](auto& x){core::thread::remove_worker(x);});
    m_createdWorkers.clear();
}

// ------------------------------------------------------------------------

void config_manager::processStartItems()
{
    std::vector<service::base::shared_future_t> futures;

    for(const auto& elem : m_cfgElem)
    {
        if(elem.first == "start")
        {
            const auto uid = elem.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!core::tools::id::exist(uid))
            {
                if(m_deferredServices.find(uid) != m_deferredServices.end())
                {
                    m_deferredStartSrv.push_back(uid);
                    SIGHT_DEBUG(
                        this->msgHead() + "Start for service '" + uid + "' will be deferred since at least one "
                                                                        "of its data is missing. With DEBUG log level, you can know which are the "
                                                                        "missing objects."
                    );
                }
                else
                {
                    SIGHT_FATAL(
                        this->msgHead() + "Start is requested for service '" + uid
                        + "', but it does not exist."
                    );
                }
            }
            else
            {
                const service::base::sptr srv = service::get(uid);
                SIGHT_FATAL_IF(
                    this->msgHead() + "Start is requested for service '" + uid + "', though this identifier "
                                                                                 "exists, this is not a service.",
                    !srv
                );

                futures.emplace_back(srv->start());

                m_startedSrv.push_back(srv);
            }
        }
    }

    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));
}

// ------------------------------------------------------------------------

void config_manager::processUpdateItems()
{
    std::vector<service::base::shared_future_t> futures;

    for(const auto& elem : m_cfgElem)
    {
        if(elem.first == "update")
        {
            const auto uid = elem.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!core::tools::id::exist(uid))
            {
                if(m_deferredServices.find(uid) != m_deferredServices.end())
                {
                    m_deferredUpdateSrv.push_back(uid);
                    SIGHT_DEBUG(
                        this->msgHead() + "Update for service '" + uid + "'will be deferred since at least one "
                                                                         "of its data is missing. With DEBUG log level, you can know which are the "
                                                                         "missing objects."
                    );
                }
                else
                {
                    SIGHT_FATAL(
                        this->msgHead() + "Update is requested for service '" + uid
                        + "', but it does not exist."
                    );
                }
            }
            else
            {
                const service::base::sptr srv = service::get(uid);
                SIGHT_FATAL_IF(
                    this->msgHead() + "Update is requested for service '" + uid
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

void config_manager::createObjects(const core::runtime::config_t& cfgElem)
{
    for(const auto& elem : cfgElem)
    {
        if(elem.first == "object")
        {
            // Get attributes

            // Id
            ConfigAttribute id("", false);
            if(const auto uid = elem.second.get_optional<std::string>("<xmlattr>.uid"); uid.has_value())
            {
                id.first = uid.value();
                SIGHT_ASSERT(this->msgHead() + "\"uid\" attribute is empty.", !id.first.empty());
                id.second = true;
            }

            // Type
            ConfigAttribute type("", false);
            if(const auto typeCfg = elem.second.get_optional<std::string>("<xmlattr>.type"); typeCfg.has_value())
            {
                type.first = typeCfg.value();
                SIGHT_ASSERT(this->msgHead() + "\"type\" attribute is empty.", !type.first.empty());
                type.second = true;
            }

            // Build mode
            ConfigAttribute buildMode("", false);
            if(const auto buildModeCfg =
                   elem.second.get_optional<std::string>("<xmlattr>.src"); buildModeCfg.has_value())
            {
                buildMode.first = buildModeCfg.value();
                SIGHT_ASSERT("this->msgHead() + \"src\" attribute is empty.", !buildMode.first.empty());

                SIGHT_ASSERT(
                    "Unhandled build mode (bad \"src\" attribute). Must be \"new\", \"deferred\" or \"ref\".",
                    buildMode.first == "ref" || buildMode.first == "src" || buildMode.first == "deferred"
                );
                buildMode.second = true;
            }

            if(buildMode.first == "deferred")
            {
                SIGHT_ASSERT(this->msgHead() + "Missing attribute \"id\".", id.second);
                const auto ret = m_deferredObjects.insert(std::make_pair(id.first, DeferredObjectType()));
                SIGHT_NOT_USED(ret);
                SIGHT_DEBUG_IF(
                    this->msgHead() + "Object '" + id.first + "' already exists in this config.",
                    !ret.second
                );
            }
            else
            {
                // Creation of a new object
                data::object::sptr obj;
                service::object_parser::sptr objParser;

                // Create new or get the referenced object
                if(buildMode.second && buildMode.first == "ref")
                {
                    SIGHT_ASSERT(this->msgHead() + "Missing attribute \"id\".", id.second);
                    obj = this->getObject(type, id.first);
                }
                else
                {
                    obj = this->getNewObject(type, id);

                    // Get the object parser associated with the object type
                    const auto srvFactory = service::extension::factory::get();

                    std::string srvImpl = srvFactory->getDefaultImplementationIdFromObjectAndType(
                        obj->get_classname(),
                        "sight::service::object_parser"
                    );

                    service::base::sptr srv = srvFactory->create(srvImpl);
                    objParser = std::dynamic_pointer_cast<service::object_parser>(srv);
                    objParser->setObjectConfig(elem.second);
                    objParser->createConfig(obj);
                }

                m_createdObjects[id.first] = std::make_pair(obj, objParser);
            }
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::createServices(const core::runtime::config_t& cfgElem)
{
    for(const auto& serviceCfg : boost::make_iterator_range(cfgElem.equal_range("service")))
    {
        // Parse the service configuration
        auto srvConfig = app::helper::config::parseService(serviceCfg.second, this->msgHead());

        // Check if we can start the service now or if we must deferred its creation
        bool createService = true;
        std::vector<std::string> uids;

        for(const auto& [key, objectCfg] : srvConfig.m_objects)
        {
            // If the current service uses an object that is marked as deferred, this means
            // we will have to manage automatically the start/stop and the connections
            auto it = m_deferredObjects.find(objectCfg.m_uid);
            if(it != m_deferredObjects.end())
            {
                it->second.m_servicesCfg.emplace_back(srvConfig);
                uids.push_back(objectCfg.m_uid);
                m_deferredServices.insert(srvConfig.m_uid);

                // Do not start the service if any non-optional object is not yet present
                if(!objectCfg.m_optional && !core::tools::id::exist(objectCfg.m_uid))
                {
                    createService = false;
                }
            }

            // Extra check to warn the user that an object is used as output but not marked as deferred
            if(objectCfg.m_access == data::Access::out)
            {
                SIGHT_ERROR_IF(
                    this->msgHead() + "Object '" + objectCfg.m_uid + "' is used as output in service '"
                    + srvConfig.m_uid + "' but it not declared as 'deferred'.",
                    it == m_deferredObjects.end()
                );
            }
        }

        if(createService)
        {
            this->createService(srvConfig);
        }
        else
        {
            // Check if a service hasn't been already created with this uid
            SIGHT_ASSERT(
                this->msgHead() + "UID " + srvConfig.m_uid + " already exists.",
                !core::tools::id::exist(srvConfig.m_uid)
            );

            const std::string msg = config_manager::getUIDListAsString(uids);
            SIGHT_DEBUG(
                this->msgHead() + "Service '" + srvConfig.m_uid
                + "' has not been created because the object" + msg + "not available."
            );
        }
    }

    for(const auto& serviceCfg : boost::make_iterator_range(cfgElem.equal_range("serviceList")))
    {
        this->createServices(serviceCfg.second);
    }
}

// ------------------------------------------------------------------------

service::base::sptr config_manager::createService(const detail::service_config& srvConfig)
{
    // Create and bind service
    service::base::sptr srv = this->getNewService(srvConfig.m_uid, srvConfig.m_type);
    service::registerService(srv);
    m_createdSrv.push_back(srv);

    if(!srvConfig.m_worker.empty())
    {
        core::thread::worker::sptr worker = core::thread::get_worker(srvConfig.m_worker);
        if(!worker)
        {
            worker = core::thread::worker::make();
            core::thread::add_worker(srvConfig.m_worker, worker);
            m_createdWorkers.push_back(worker);
        }

        srv->set_worker(worker);
    }

    const std::string errMsgTail = " when creating service '" + srvConfig.m_uid + "'.";
    srv->set_config(srvConfig.m_config);

    for(const auto& [key, objectCfg] : srvConfig.m_objects)
    {
        set_deferred_id(srv, key.first, objectCfg.m_uid, key.second);

        data::object::sptr obj = this->findObject(objectCfg.m_uid, errMsgTail);

        SIGHT_ASSERT(
            this->msgHead() + "Object '" << objectCfg.m_uid << "' has not been found" << errMsgTail,
            obj || objectCfg.m_optional
        );
        if((obj || !objectCfg.m_optional) && objectCfg.m_access != data::Access::out)
        {
            set_object(
                srv,
                obj,
                key.first,
                key.second,
                objectCfg.m_access,
                objectCfg.m_autoConnect || srvConfig.m_globalAutoConnect,
                objectCfg.m_optional
            );
        }
    }

    bool hasStartConnection = false;
    bool hasStopConnection  = false;
    // Set the proxies
    const auto& itSrvProxy = m_servicesProxies.find(srvConfig.m_uid);
    if(itSrvProxy != m_servicesProxies.end())
    {
        for(const auto& itProxy : itSrvProxy->second.m_proxyCnt)
        {
            add_connection(srv, itProxy.second);

            // Check if we need to monitor the start/stop state because of connections
            for(const auto& slot : itProxy.second.m_slots)
            {
                if(slot.first == srvConfig.m_uid)
                {
                    if(slot.second == service::slots::START)
                    {
                        hasStartConnection = true;
                        break;
                    }
                    // NOLINTNEXTLINE(llvm-else-after-return,readability-else-after-return)
                    else if(slot.second == service::slots::STOP)
                    {
                        hasStopConnection = true;
                        break;
                    }
                }
            }
        }
    }

    // Configure
    srv->configure();

    if(hasStartConnection)
    {
        srv->signal(service::signals::STARTED)->connect(this->slot(ADD_STARTED_SRV_SLOT));
    }

    if(hasStopConnection)
    {
        srv->signal(service::signals::STOPPED)->connect(this->slot(REMOVE_STARTED_SRV_SLOT));
    }

    return srv;
}

// ------------------------------------------------------------------------

void config_manager::createConnections()
{
    for(const auto& elem : m_cfgElem)
    {
        if(elem.first == "connect")
        {
            // Parse all connections
            auto genIdFn = [this](){return "Proxy_" + this->get_id() + "_" + std::to_string(m_proxyID++);};

            proxy_connections connectionInfos = app::helper::config::parseConnections2(
                elem.second,
                this->msgHead(),
                genIdFn
            );

            // Proxy that is used for non-deferred connections
            proxy_connections createdObjectsProxy(connectionInfos.m_channel);

            // Register signals
            for(const auto& signalInfo : connectionInfos.m_signals)
            {
                auto itDeferredObj = m_deferredObjects.find(signalInfo.first);
                if(itDeferredObj != m_deferredObjects.end())
                {
                    // Deferred Object
                    proxy_connections& proxy = itDeferredObj->second.m_proxyCnt[connectionInfos.m_channel];
                    proxy.addSignalConnection(signalInfo);
                }
                else
                {
                    auto itObj = m_createdObjects.find(signalInfo.first);
                    if(itObj != m_createdObjects.end())
                    {
                        // Regular object
                        createdObjectsProxy.addSignalConnection(signalInfo);
                    }
                    else
                    {
                        // Service
                        auto& itSrv              = m_servicesProxies[signalInfo.first];
                        proxy_connections& proxy = itSrv.m_proxyCnt[connectionInfos.m_channel];
                        proxy.addSignalConnection(signalInfo);
                        proxy.m_channel = connectionInfos.m_channel;
                    }
                }
            }

            // Register slots
            for(const auto& slotInfo : connectionInfos.m_slots)
            {
                auto itDeferredObj = m_deferredObjects.find(slotInfo.first);
                if(itDeferredObj != m_deferredObjects.end())
                {
                    // Deferred Object
                    proxy_connections& proxy = itDeferredObj->second.m_proxyCnt[connectionInfos.m_channel];
                    proxy.addSlotConnection(slotInfo);
                }
                else
                {
                    auto itObj = m_createdObjects.find(slotInfo.first);
                    if(itObj != m_createdObjects.end())
                    {
                        // Regular object
                        createdObjectsProxy.addSlotConnection(slotInfo);
                    }
                    else
                    {
                        // Service
                        auto& itSrv              = m_servicesProxies[slotInfo.first];
                        proxy_connections& proxy = itSrv.m_proxyCnt[connectionInfos.m_channel];
                        proxy.addSlotConnection(slotInfo);
                        proxy.m_channel = connectionInfos.m_channel;
                    }
                }
            }

            m_createdObjectsProxies[connectionInfos.m_channel] = createdObjectsProxy;
            this->connectProxy(connectionInfos.m_channel, createdObjectsProxy);
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::destroyProxy(
    const std::string& _channel,
    const proxy_connections& _proxyCfg,
    const std::string& _key,
    data::object::csptr _hintObj
)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const proxy_connections::ProxyEltType& signalElt : _proxyCfg.m_signals)
    {
        if(_key.empty() || signalElt.first == _key)
        {
            core::tools::object::csptr obj = _hintObj;
            if(obj == nullptr)
            {
                obj = core::tools::id::get_object(signalElt.first);
            }

            core::com::has_signals::csptr hasSignals = std::dynamic_pointer_cast<const core::com::has_signals>(obj);
            SIGHT_ASSERT(this->msgHead() + "Signal source not found '" + signalElt.first + "'", obj);

            core::com::signal_base::sptr sig = hasSignals->signal(signalElt.second);

            try
            {
                proxy->disconnect(_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signalElt.second + "' from '" + signalElt.first + "' can not be disconnected "
                                                                                   "from the channel '" + _channel + "': " + std::string(
                        e.what()
                                                                                   )
                );
            }
        }
    }

    for(const proxy_connections::ProxyEltType& slotElt : _proxyCfg.m_slots)
    {
        if(_key.empty() || slotElt.first == _key)
        {
            core::tools::object::sptr obj       = core::tools::id::get_object(slotElt.first);
            core::com::has_slots::sptr hasSlots = std::dynamic_pointer_cast<core::com::has_slots>(obj);
            SIGHT_ASSERT(this->msgHead() + "Slot destination not found '" + slotElt.first + "'", hasSlots);

            core::com::slot_base::sptr slot = hasSlots->slot(slotElt.second);

            try
            {
                proxy->disconnect(_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotElt.second + "' from '" + slotElt.first + "' can not be disconnected from the "
                                                                             "channel '" + _channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

// ------------------------------------------------------------------------

void config_manager::destroyProxies()
{
    // Remove local proxies from deferred objects
    for(auto& itDeferredObj : m_deferredObjects)
    {
        if(itDeferredObj.second.m_object)
        {
            for(const auto& itProxy : itDeferredObj.second.m_proxyCnt)
            {
                this->destroyProxy(itProxy.first, itProxy.second, itDeferredObj.first, itDeferredObj.second.m_object);
            }

            itDeferredObj.second.m_object.reset();
        }
    }

    // Remove local proxies from all created objects
    for(const auto& itProxy : m_createdObjectsProxies)
    {
        this->destroyProxy(itProxy.first, itProxy.second);
    }

    m_createdObjectsProxies.clear();
}

//------------------------------------------------------------------------------

void config_manager::addObjects(data::object::sptr _obj, const std::string& _id)
{
    core::mt::scoped_lock lock(m_mutex);
    if(m_state != STATE_STARTED)
    {
        SIGHT_INFO("Skip processing of a new object since the config is not running.");
        return;
    }

    FW_PROFILE("addObjects");

    // Local map used to process services only once
    std::map<std::string, const detail::service_config*> servicesMapCfg;

    // Local vector used to store services and keep the declare order (we could use only this one but the map is used
    // to speedup the search
    std::vector<const detail::service_config*> servicesCfg;

    // Are there services that were waiting for this object ?
    auto itDeferredObj = m_deferredObjects.find(_id);
    if(itDeferredObj != m_deferredObjects.end())
    {
        // For each service waiting to be started
        for(const auto& srvCfg : itDeferredObj->second.m_servicesCfg)
        {
            if(servicesMapCfg.find(srvCfg.m_uid) == servicesMapCfg.end())
            {
                servicesMapCfg[srvCfg.m_uid] = &srvCfg;
                servicesCfg.push_back(&srvCfg);
            }
        }

        // Connect signals of this deferred object
        itDeferredObj->second.m_object = _obj;

        for(const auto& connectCfg : itDeferredObj->second.m_proxyCnt)
        {
            this->connectProxy(connectCfg.first, connectCfg.second);
        }
    }

    std::unordered_map<std::string, service::base::sptr> newServices;

    for(const auto& itService : servicesCfg)
    {
        const auto* srvCfg = itService;
        SIGHT_ASSERT("config is null", srvCfg);
        const auto& uid = srvCfg->m_uid;

        bool createService = true;

        // Look for all objects (there could be more than the current object) and check if they are all created
        for(const auto& [key, objCfg] : srvCfg->m_objects)
        {
            // Look first in objects created in this appConfig
            if(m_createdObjects.find(objCfg.m_uid) == m_createdObjects.end())
            {
                // Not found, now look in the objects that were marked as "deferred"
                const auto itLocalDeferredObj = m_deferredObjects.find(objCfg.m_uid);
                SIGHT_ASSERT(
                    this->msgHead() + "Object '" + objCfg.m_uid + "' used by service '" + uid
                    + "' has not been declared in this config.",
                    itLocalDeferredObj != m_deferredObjects.end()
                );

                const auto object = itLocalDeferredObj->second.m_object;
                if(object == nullptr)
                {
                    if(!objCfg.m_optional)
                    {
                        createService = false;

                        SIGHT_INFO(
                            this->msgHead() + "Service '" + uid + "' has not been created because the "
                                                                  "object" + objCfg.m_uid + " is not available."
                        );
                    }
                }
                else if(core::tools::id::exist(uid))
                {
                    service::base::sptr srv = service::get(uid);
                    SIGHT_ASSERT(this->msgHead() + "No service registered with UID \"" + uid + "\".", srv);

                    // We have an optional object
                    if(objCfg.m_optional)
                    {
                        // Check if we already registered an object at this key
                        auto registeredObj = srv->has_data::object(key.first, objCfg.m_access, key.second);
                        if(registeredObj != nullptr)
                        {
                            // If this is not the object we have to swap, then unregister it
                            if(registeredObj != object)
                            {
                                reset_object(srv, key.first, key.second);
                            }
                        }

                        if(registeredObj != object)
                        {
                            // Register the key on the service
                            set_object(
                                srv,
                                object,
                                key.first,
                                key.second,
                                objCfg.m_access,
                                objCfg.m_autoConnect || srvCfg->m_globalAutoConnect,
                                objCfg.m_optional
                            );

                            if(srv->started())
                            {
                                // Call the swapping callback of the service and wait for it
                                srv->swap_key(
                                    objCfg.m_key,
                                    std::const_pointer_cast<data::object>(registeredObj)
                                ).wait();
                            }
                        }
                    }

                    createService = false;
                }
            }
        }

        // All the data for this service is ready, create and run it
        if(createService)
        {
            // Create the service
            newServices.emplace(uid, this->createService(*srvCfg));

            // Debug message
            SIGHT_INFO(
                this->msgHead() + "Service '" + uid + "' has been automatically created because its "
                                                      "objects are all available."
            );
        }
    }

    std::vector<service::base::shared_future_t> futures;

    // Start services according to the order given in the configuration
    for(const auto& uid : m_deferredStartSrv)
    {
        auto itSrv = newServices.find(uid);
        if(itSrv != newServices.end())
        {
            futures.push_back(itSrv->second->start());
            m_startedSrv.push_back(itSrv->second);

            // Debug message
            SIGHT_INFO(
                this->msgHead() + "Service '" + uid + "' has been automatically started because its "
                                                      "objects are all available."
            );
        }
    }

    boost::wait_for_all(futures.begin(), futures.end());
    futures.clear();

    // Update services according to the order given in the configuration
    for(const auto& uid : m_deferredUpdateSrv)
    {
        auto itSrv = newServices.find(uid);
        if(itSrv != newServices.end())
        {
            futures.push_back(itSrv->second->update());

            // Debug message
            SIGHT_INFO(
                this->msgHead() + "Service '" + uid + "' has been automatically update because its "
                                                      "objects are all available."
            );
        }
    }

    boost::wait_for_all(futures.begin(), futures.end());
}

//------------------------------------------------------------------------------

void config_manager::removeObjects(data::object::sptr _obj, const std::string& _id)
{
    core::mt::scoped_lock lock(m_mutex);
    if(m_state != STATE_STARTED)
    {
        SIGHT_INFO("Skip processing of a new object since the config is not running.");
        return;
    }

    FW_PROFILE("removeObjects");

    // Are there services that were connected with this object ?
    const auto itDeferredObj = m_deferredObjects.find(_id);
    if(itDeferredObj != m_deferredObjects.end())
    {
        for(const auto& itProxy : itDeferredObj->second.m_proxyCnt)
        {
            this->destroyProxy(itProxy.first, itProxy.second, _id, itDeferredObj->second.m_object);
        }

        itDeferredObj->second.m_object.reset();

        // Are there services that were using this object ?
        for(const auto& srvCfg : itDeferredObj->second.m_servicesCfg)
        {
            if(core::tools::id::exist(srvCfg.m_uid))
            {
                // Check all objects, to know if this object is optional
                bool optional = true;

                for(const auto& [key, objCfg] : srvCfg.m_objects)
                {
                    if(_id == objCfg.m_uid)
                    {
                        service::base::sptr srv = service::get(srvCfg.m_uid);
                        SIGHT_ASSERT("No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

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
                    service::base::sptr srv = service::get(srvCfg.m_uid);
                    SIGHT_ASSERT(this->msgHead() + "No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                    std::erase_if(m_startedSrv, [&srv](auto& x){return srv == x.lock();});

                    SIGHT_ASSERT("Service " << srv->get_id() << " already stopped.", !srv->stopped());
                    srv->stop().wait();

                    // 2. Destroy the service
                    SIGHT_ASSERT(
                        "Service " << srv->get_id() << " must be stopped before destruction.",
                        srv->stopped()
                    );
                    service::unregisterService(srv);

                    std::erase_if(m_createdSrv, [&srv](auto& x){return srv == x.lock();});

                    sight::service::base::wptr checkSrv = srv;
                    srv.reset();

                    SIGHT_ASSERT(
                        this->msgHead() + "The service '" + srvCfg.m_uid
                        + "'' should have been destroyed, but someone "
                          "still holds a reference which prevents to destroy it properly.",
                        checkSrv.expired()
                    );

                    SIGHT_INFO(
                        this->msgHead() + "Service '" + srvCfg.m_uid
                        + "' has been stopped because the object "
                        + _id + " is no longer available."
                    );
                }
                else
                {
                    // Update auto connections
                    service::base::sptr srv = service::get(srvCfg.m_uid);
                    SIGHT_ASSERT(this->msgHead() + "No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                    auto_disconnect(srv);
                    auto_connect(srv);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void config_manager::connectProxy(const std::string& _channel, const proxy_connections& _connectCfg)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& signalCfg : _connectCfg.m_signals)
    {
        core::tools::object::sptr sigSource = core::tools::id::get_object(signalCfg.first);
        if(sigSource == nullptr)
        {
            // We didn't found the object or service globally, let's try with local deferred objects
            auto itDeferredObj = m_deferredObjects.find(signalCfg.first);
            SIGHT_ASSERT(
                this->msgHead() + "Object '" + signalCfg.first + "' not found.",
                itDeferredObj != m_deferredObjects.end()
            );

            sigSource = itDeferredObj->second.m_object;
        }

        core::com::has_signals::sptr hasSignals = std::dynamic_pointer_cast<core::com::has_signals>(sigSource);
        SIGHT_ASSERT(this->msgHead() + "Signal source not found '" + signalCfg.first + "'", hasSignals);

        core::com::signal_base::sptr sig = hasSignals->signal(signalCfg.second);
        SIGHT_ASSERT("Signal '" + signalCfg.second + "' not found in source '" + signalCfg.first + "'.", sig);

        try
        {
            proxy->connect(_channel, sig);
        }
        catch(const std::exception& e)
        {
            SIGHT_ERROR(
                "Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be connected to the "
                                                                               "channel '" + _channel + "': " + std::string(
                    e.what()
                                                                               )
            );
        }
    }

    for(const auto& slotCfg : _connectCfg.m_slots)
    {
        core::tools::object::sptr slotDest  = core::tools::id::get_object(slotCfg.first);
        core::com::has_slots::sptr hasSlots = std::dynamic_pointer_cast<core::com::has_slots>(slotDest);
        SIGHT_ASSERT(this->msgHead() + "Slot destination not found '" + slotCfg.first + "'", hasSlots);

        core::com::slot_base::sptr slot = hasSlots->slot(slotCfg.second);
        SIGHT_ASSERT("Slot '" + slotCfg.second + "' not found in source '" + slotCfg.first + "'.", slot);

        try
        {
            proxy->connect(_channel, slot);
        }
        catch(const std::exception& e)
        {
            SIGHT_ERROR(
                "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be connected to the "
                                                                         "channel '" + _channel + "': "
                + std::string(e.what())
            );
        }
    }
}

//------------------------------------------------------------------------------

std::string config_manager::getUIDListAsString(const std::vector<std::string>& uidList)
{
    std::string msg = uidList.size() == 1 ? " '" : "s '";
    msg += uidList[0];
    for(auto it = uidList.begin() + 1 ; it < uidList.end() ; ++it)
    {
        msg += "', '" + *it;
    }

    msg.append(uidList.size() == 1 ? "' is " : "' are ");

    return msg;
}

//------------------------------------------------------------------------------

} // namespace sight::app::detail
