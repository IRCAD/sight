/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "service/AppConfigManager.hpp"

#include "service/helper/Config.hpp"
#include "service/op/Get.hpp"
#include "core/runtime/Convert.hpp"
#include "service/registry/Proxy.hpp"
#include "service/extension/Config.hpp"
#include "service/extension/Factory.hpp"
#include "service/registry/ObjectService.hpp"

#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <core/Profiling.hpp>

#include <data/Composite.hpp>
#include <data/Object.hpp>

#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>

#include <boost/foreach.hpp>
#include <boost/thread/futures/wait_for_all.hpp>

#include <regex>

namespace sight::service
{

// ------------------------------------------------------------------------

static const core::com::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const core::com::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

// ------------------------------------------------------------------------

AppConfigManager::AppConfigManager() :
    m_proxyID(0),
    m_isUnitTest(false)
{
    newSlot(s_ADD_OBJECTS_SLOT, &AppConfigManager::addObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &AppConfigManager::removeObjects, this);

    auto defaultWorker = core::thread::getDefaultWorker();
    core::com::HasSlots::m_slots.setWorker(defaultWorker);
}

// ------------------------------------------------------------------------

AppConfigManager::~AppConfigManager()
{
    SIGHT_ASSERT("Manager must be stopped before destruction.", m_state == STATE_DESTROYED);
}

// ------------------------------------------------------------------------

void AppConfigManager::setConfig(const std::string& _configId, const FieldAdaptorType& _replaceFields)
{
    m_configId = _configId;
    m_cfgElem  =
        extension::AppConfig::getDefault()->getAdaptedTemplateConfig(_configId, _replaceFields, !m_isUnitTest);
}

// ------------------------------------------------------------------------

void AppConfigManager::setConfig(const std::string& _configId, const data::Composite::csptr& _replaceFields)
{
    m_configId = _configId;
    m_cfgElem  =
        extension::AppConfig::getDefault()->getAdaptedTemplateConfig(_configId, _replaceFields, !m_isUnitTest);
}

// ------------------------------------------------------------------------

void AppConfigManager::launch()
{
    FW_PROFILE("launch");

    this->startModule();
    this->create();
    this->start();
    this->update();
}

// ------------------------------------------------------------------------

void AppConfigManager::stopAndDestroy()
{
    this->stop();
    this->destroy();
}

// ------------------------------------------------------------------------

void AppConfigManager::startModule()
{
    SIGHT_ERROR_IF("Module is not specified, it can not be started.", m_configId.empty());
    if(!m_configId.empty() && !m_isUnitTest)
    {
        std::shared_ptr<core::runtime::Module> module = extension::AppConfig::getDefault()->getModule(m_configId);
        module->start();
    }
}

// ------------------------------------------------------------------------

void AppConfigManager::create()
{
    SIGHT_ASSERT("Manager already running.", m_state == STATE_DESTROYED);

    // Create the dummy object for new services that don't work on any object
    // For now this dummy object will also contain all the "deferred" objects
    m_tmpRootObject = data::Composite::New();

    m_addObjectConnection    = service::OSR::getRegisterSignal()->connect(this->slot(s_ADD_OBJECTS_SLOT));
    m_removeObjectConnection = service::OSR::getUnregisterSignal()->connect(this->slot(s_REMOVE_OBJECTS_SLOT));

    this->createObjects(m_cfgElem);
    this->createConnections();
    const auto configTree = core::runtime::Convert::toPropertyTree(m_cfgElem);
    if(configTree.count("config"))
    {
        this->createServices(configTree.get_child("config"));
    }

    m_state = STATE_CREATED;
}

// ------------------------------------------------------------------------

void AppConfigManager::start()
{
    SIGHT_ASSERT("Manager must be created first.", m_state == STATE_CREATED || m_state == STATE_STOPPED);

    this->processStartItems();
    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->startConfig();
    }

    m_state = STATE_STARTED;
}

// ------------------------------------------------------------------------

void AppConfigManager::update()
{
    this->processUpdateItems();
    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->updateConfig();
    }
}

// ------------------------------------------------------------------------

void AppConfigManager::stop()
{
    SIGHT_ASSERT("Manager is not started, cannot stop.", m_state == STATE_STARTED);

    service::OSR::getRegisterSignal()->disconnect(this->slot(s_ADD_OBJECTS_SLOT));
    service::OSR::getUnregisterSignal()->disconnect(this->slot(s_REMOVE_OBJECTS_SLOT));

    // Disconnect configuration connections
    this->destroyProxies();

    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->stopConfig();
    }

    this->stopStartedServices();

    SIGHT_DEBUG(
        "Parsing OSR after stopping the config :" << std::endl
        << service::OSR::getRegistryInformation()
    );
    m_state = STATE_STOPPED;
}

// ------------------------------------------------------------------------

void AppConfigManager::destroy()
{
    SIGHT_ASSERT("Manager is not stopped, cannot destroy.", m_state == STATE_STOPPED || m_state == STATE_CREATED);

    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->destroyConfig();
    }

    this->destroyCreatedServices();

    SIGHT_DEBUG(
        "Parsing OSR after destroying the config :" << std::endl
        << service::OSR::getRegistryInformation()
    );

    m_cfgElem.reset();
    m_createdObjects.clear();
    m_deferredObjects.clear();
    m_deferredServices.clear();
    m_deferredStartSrv.clear();
    m_deferredUpdateSrv.clear();
    m_servicesProxies.clear();

    m_state = STATE_DESTROYED;
}

// ------------------------------------------------------------------------

void AppConfigManager::setIsUnitTest(bool _isUnitTest)
{
    m_isUnitTest = _isUnitTest;
}

// ------------------------------------------------------------------------

void AppConfigManager::addExistingDeferredObject(const data::Object::sptr& _obj, const std::string& _uid)
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

data::Object::sptr AppConfigManager::getConfigRoot() const
{
    if(m_createdObjects.empty())
    {
        return m_tmpRootObject;
    }

    return m_createdObjects.begin()->second.first;
}

// ------------------------------------------------------------------------

data::Object::sptr AppConfigManager::findObject(const std::string& uid, std::string_view errMsgTail) const
{
#ifndef _DEBUG
    SIGHT_NOT_USED(errMsgTail);
#endif
    data::Object::sptr obj;

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

data::Object::sptr AppConfigManager::getNewObject(ConfigAttribute type, ConfigAttribute uid) const
{
    // Building object structure
    SPTR(core::runtime::Extension) ext = core::runtime::findExtension(type.first);
    if(ext)
    {
        const std::string className = core::getClassname<data::Object>();
        SIGHT_ASSERT(
            "Extension and classname are different.",
            ext->getPoint() == className
        );

        // Start dll to retrieve proxy and register object
        ext->getModule()->start();
    }

    data::Object::sptr obj = data::factory::New(type.first);
    SIGHT_ASSERT("Factory failed to build object : " + type.first, obj);

    if(uid.second)
    {
        SIGHT_ASSERT("Object already has an UID.", !obj->hasID());
        SIGHT_ASSERT("UID " << uid.first << " already exists.", !core::tools::fwID::exist(uid.first));
        obj->setID(uid.first);
    }

    return obj;
}

// ------------------------------------------------------------------------

data::Object::sptr AppConfigManager::getNewObject(ConfigAttribute type, const std::string& uid) const
{
    return this->getNewObject(type, ConfigAttribute(uid, true));
}

// ------------------------------------------------------------------------

data::Object::sptr AppConfigManager::getObject(ConfigAttribute type, const std::string& uid) const
{
    SIGHT_ASSERT(this->msgHead() + "Object with UID \"" + uid + "\" doesn't exist.", core::tools::fwID::exist(uid));
    data::Object::sptr obj = data::Object::dynamicCast(core::tools::fwID::getObject(uid));

    SIGHT_ASSERT(this->msgHead() + "The UID '" + uid + "' does not reference any object.", obj);

    if(type.second)
    {
        SIGHT_ASSERT(
            this->msgHead() + "Object with UID \"" + uid
            + "\" has a different type (\"" + obj->getClassname() + "\" != \"" + type.first + "\").",
            type.first == obj->getClassname()
        );
    }

    return obj;
}

// ------------------------------------------------------------------------

service::IService::sptr AppConfigManager::getNewService(const std::string& uid, const std::string& implType) const
{
    service::extension::Factory::sptr srvFactory = service::extension::Factory::getDefault();

    service::IService::sptr srv = srvFactory->create(implType);

    SIGHT_ASSERT("Factory could not create service of type <" + implType + ">.", srv);
    SIGHT_ASSERT("Service already has an UID.", !srv->hasID());

    SIGHT_ASSERT(this->msgHead() + "UID " + uid + " already exists.", !core::tools::fwID::exist(uid));
    if(!uid.empty())
    {
        srv->setID(uid);
    }

    return srv;
}

// ------------------------------------------------------------------------

void AppConfigManager::stopStartedServices()
{
    std::vector<service::IService::SharedFutureType> futures;

    BOOST_REVERSE_FOREACH(service::IService::wptr w_srv, m_startedSrv)
    {
        SIGHT_ASSERT("Service expired.", !w_srv.expired());

        const service::IService::sptr srv = w_srv.lock();
        SIGHT_ASSERT("Service " << srv->getID() << " already stopped.", !srv->isStopped());
        futures.emplace_back(srv->stop());
    }
    m_startedSrv.clear();
    std::for_each(futures.begin(), futures.end(), std::mem_fn(&std::shared_future<void>::wait));
}

// ------------------------------------------------------------------------

void AppConfigManager::destroyCreatedServices()
{
    BOOST_REVERSE_FOREACH(service::IService::wptr w_srv, m_createdSrv)
    {
        const service::IService::sptr srv = w_srv.lock();
        SIGHT_ASSERT("Service expired.", srv);

        // Force stopping services. It can happen for deferred services and when the associated data is not yet deleted.
        // Anyway, we must stop them before destroying them in all cases.
        if(!srv->isStopped())
        {
            SIGHT_WARN("Service " << srv->getID() << " is still running and will be stopped.")
            srv->stop().wait();
        }

        service::OSR::unregisterService(srv);
    }
    m_createdSrv.clear();
    service::helper::Config::clearKeyProps();

    std::for_each(m_createdWorkers.begin(), m_createdWorkers.end(), [](auto& x){core::thread::removeWorker(x);});
    m_createdWorkers.clear();
}

// ------------------------------------------------------------------------

void AppConfigManager::processStartItems()
{
    std::vector<service::IService::SharedFutureType> futures;

    for(const auto& elem : m_cfgElem->getElements())
    {
        if(elem->getName() == "start")
        {
            SIGHT_ASSERT("Missing attribute \"uid\".", elem->hasAttribute("uid"));
            const std::string uid = elem->getAttributeValue("uid");
            SIGHT_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!core::tools::fwID::exist(uid))
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
                const service::IService::sptr srv = service::get(uid);
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

    std::for_each(futures.begin(), futures.end(), std::mem_fn(&std::shared_future<void>::wait));
}

// ------------------------------------------------------------------------

void AppConfigManager::processUpdateItems()
{
    std::vector<service::IService::SharedFutureType> futures;

    for(const auto& elem : m_cfgElem->getElements())
    {
        if(elem->getName() == "update")
        {
            const std::string uid = elem->getAttributeValue("uid");
            SIGHT_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!core::tools::fwID::exist(uid))
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
                const service::IService::sptr srv = service::get(uid);
                SIGHT_FATAL_IF(
                    this->msgHead() + "Update is requested for service '" + uid
                    + "', though this identifier exists, this is not a service.",
                    !srv
                );

                futures.emplace_back(srv->update());
            }
        }
    }

    std::for_each(futures.begin(), futures.end(), std::mem_fn(&std::shared_future<void>::wait));
}

// ------------------------------------------------------------------------

void AppConfigManager::createObjects(core::runtime::ConfigurationElement::csptr cfgElem)
{
    for(const auto& elem : cfgElem->getElements())
    {
        if(elem->getName() == "object")
        {
            // Get attributes

            // Id
            ConfigAttribute id("", false);
            if(elem->hasAttribute("uid"))
            {
                id.first = elem->getAttributeValue("uid");
                SIGHT_ASSERT(this->msgHead() + "\"uid\" attribute is empty.", !id.first.empty());
                id.second = true;
            }

            // Type
            ConfigAttribute type("", false);
            if(elem->hasAttribute("type"))
            {
                type.first = elem->getAttributeValue("type");
                SIGHT_ASSERT(this->msgHead() + "\"type\" attribute is empty.", !type.first.empty());
                type.second = true;
            }

            // Build mode
            ConfigAttribute buildMode("", false);
            if(elem->hasAttribute("src"))
            {
                buildMode.first = elem->getAttributeValue("src");
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
                data::Object::sptr obj;

                // Create new or get the referenced object
                if(buildMode.second && buildMode.first == "ref")
                {
                    SIGHT_ASSERT(this->msgHead() + "Missing attribute \"id\".", id.second);
                    obj = this->getObject(type, id.first);
                }
                else
                {
                    obj = this->getNewObject(type, id);
                }

                // Get the object parser associated with the object type
                const auto srvFactory = service::extension::Factory::getDefault();

                std::string srvImpl = srvFactory->getDefaultImplementationIdFromObjectAndType(
                    obj->getClassname(),
                    "sight::service::IXMLParser"
                );

                service::IService::sptr srv = srvFactory->create(srvImpl);
                auto objectParser           = service::IXMLParser::dynamicCast(srv);
                objectParser->setObjectConfig(elem);
                objectParser->createConfig(obj);

                m_createdObjects[id.first] = std::make_pair(obj, objectParser);
            }
        }
    }
}

// ------------------------------------------------------------------------

void AppConfigManager::createServices(const boost::property_tree::ptree& cfgElem)
{
    auto serviceCfg = cfgElem.equal_range("service");
    for(auto itCfg = serviceCfg.first ; itCfg != serviceCfg.second ; ++itCfg)
    {
        // Parse the service configuration
        Config srvConfig = service::helper::Config::parseService(itCfg->second, this->msgHead());

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

                if(!objectCfg.m_optional)
                {
                    createService = false;
                }
            }
            else
            {
                SIGHT_ERROR_IF(
                    this->msgHead() + "Object '" + objectCfg.m_uid + "' is not deferred but it is used "
                                                                     "as an optional key in service '" + srvConfig.m_uid + "'. This is useless, so maybe you "
                                                                                                                           "intended to use a deferred object instead ?",
                    objectCfg.m_optional
                );
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
                !core::tools::fwID::exist(srvConfig.m_uid)
            );

            const std::string msg = AppConfigManager::getUIDListAsString(uids);
            SIGHT_DEBUG(
                this->msgHead() + "Service '" + srvConfig.m_uid
                + "' has not been created because the object" + msg + "not available."
            );
        }
    }

    serviceCfg = cfgElem.equal_range("serviceList");
    for(auto itCfg = serviceCfg.first ; itCfg != serviceCfg.second ; ++itCfg)
    {
        this->createServices(itCfg->second);
    }
}

// ------------------------------------------------------------------------

service::IService::sptr AppConfigManager::createService(const service::IService::Config& srvConfig)
{
    // Create and bind service
    const service::IService::sptr srv = this->getNewService(srvConfig.m_uid, srvConfig.m_type);
    service::OSR::registerService(srv);
    m_createdSrv.push_back(srv);

    if(!srvConfig.m_worker.empty())
    {
        core::thread::Worker::sptr worker = core::thread::getWorker(srvConfig.m_worker);
        if(!worker)
        {
            worker = core::thread::Worker::New();
            core::thread::addWorker(srvConfig.m_worker, worker);
            m_createdWorkers.push_back(worker);
        }

        srv->setWorker(worker);
    }

    const std::string errMsgTail = " when creating service '" + srvConfig.m_uid + "'.";
    srv->setConfiguration(srvConfig);

    for(const auto& [key, objectCfg] : srvConfig.m_objects)
    {
        srv->setObjectId(key.first, objectCfg.m_uid, key.second);

        data::Object::sptr obj = this->findObject(objectCfg.m_uid, errMsgTail);

        SIGHT_ASSERT(
            this->msgHead() + "Object '" << objectCfg.m_uid << "' has not been found" << errMsgTail,
            (!objectCfg.m_optional && obj) || objectCfg.m_optional
        );
        if((obj || !objectCfg.m_optional) && objectCfg.m_access != data::Access::out)
        {
            srv->setObject(
                obj,
                key.first,
                key.second,
                objectCfg.m_access,
                objectCfg.m_autoConnect,
                objectCfg.m_optional
            );
        }
    }

    // Set the proxies
    const auto& itSrvProxy = m_servicesProxies.find(srvConfig.m_uid);
    if(itSrvProxy != m_servicesProxies.end())
    {
        for(const auto& itProxy : itSrvProxy->second.m_proxyCnt)
        {
            srv->_addProxyConnection(itProxy.second);
        }
    }

    // Configure
    srv->configure();

    return srv;
}

// ------------------------------------------------------------------------

void AppConfigManager::createConnections()
{
    for(const auto& elem : m_cfgElem->getElements())
    {
        if(elem->getName() == "connect")
        {
            // Parse all connections
            auto genIdFn = [this]()
                           {
                               return "Proxy_" + this->getID() + "_" + std::to_string(m_proxyID++);
                           };

            ProxyConnections connectionInfos = service::helper::Config::parseConnections2(
                elem,
                this->msgHead(),
                genIdFn
            );

            // Proxy that is used for non-deferred connections
            ProxyConnections createdObjectsProxy(connectionInfos.m_channel);

            // Register signals
            for(const auto& signalInfo : connectionInfos.m_signals)
            {
                auto itDeferredObj = m_deferredObjects.find(signalInfo.first);
                if(itDeferredObj != m_deferredObjects.end())
                {
                    // Deferred Object
                    ProxyConnections& proxy = itDeferredObj->second.m_proxyCnt[connectionInfos.m_channel];
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
                        auto& itSrv             = m_servicesProxies[signalInfo.first];
                        ProxyConnections& proxy = itSrv.m_proxyCnt[connectionInfos.m_channel];
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
                    ProxyConnections& proxy = itDeferredObj->second.m_proxyCnt[connectionInfos.m_channel];
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
                        auto& itSrv             = m_servicesProxies[slotInfo.first];
                        ProxyConnections& proxy = itSrv.m_proxyCnt[connectionInfos.m_channel];
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

void AppConfigManager::destroyProxy(
    const std::string& _channel,
    const ProxyConnections& _proxyCfg,
    const std::string& _key,
    data::Object::csptr _hintObj
)
{
    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    for(const ProxyConnections::ProxyEltType& signalElt : _proxyCfg.m_signals)
    {
        if(_key.empty() || signalElt.first == _key)
        {
            core::tools::Object::csptr obj = _hintObj;
            if(obj == nullptr)
            {
                obj = core::tools::fwID::getObject(signalElt.first);
            }

            core::com::HasSignals::csptr hasSignals = std::dynamic_pointer_cast<const core::com::HasSignals>(obj);
            SIGHT_ASSERT(this->msgHead() + "Signal source not found '" + signalElt.first + "'", obj);

            core::com::SignalBase::sptr sig = hasSignals->signal(signalElt.second);

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

    for(const ProxyConnections::ProxyEltType& slotElt : _proxyCfg.m_slots)
    {
        if(_key.empty() || slotElt.first == _key)
        {
            core::tools::Object::sptr obj      = core::tools::fwID::getObject(slotElt.first);
            core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast<core::com::HasSlots>(obj);
            SIGHT_ASSERT(this->msgHead() + "Slot destination not found '" + slotElt.first + "'", hasSlots);

            core::com::SlotBase::sptr slot = hasSlots->slot(slotElt.second);

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

void AppConfigManager::destroyProxies()
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

void AppConfigManager::addObjects(data::Object::sptr _obj, const std::string& _id)
{
    FW_PROFILE("addObjects");

    // Local map used to process services only once
    std::map<std::string, const Config*> servicesMapCfg;

    // Local vector used to store services and keep the declare order (we could use only this one but the map is used
    // to speedup the search
    std::vector<const Config*> servicesCfg;

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

    std::unordered_map<std::string, service::IService::sptr> newServices;

    for(const auto& itService : servicesCfg)
    {
        auto srvCfg = itService;
        SIGHT_ASSERT("Config is null", srvCfg);
        auto& uid = srvCfg->m_uid;

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
                    + "' has not been declared in this AppConfig.",
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
                else if(core::tools::fwID::exist(uid))
                {
                    service::IService::sptr srv = service::get(uid);
                    SIGHT_ASSERT(this->msgHead() + "No service registered with UID \"" + uid + "\".", srv);

                    // We have an optional object
                    if(objCfg.m_optional)
                    {
                        // Check if we already registered an object at this key
                        auto registeredObj = srv->getObject(key.first, objCfg.m_access, key.second);
                        if(registeredObj != nullptr)
                        {
                            // If this is not the object we have to swap, then unregister it
                            if(registeredObj != object)
                            {
                                srv->resetObject(key.first, key.second, objCfg.m_access);
                            }
                        }

                        if(registeredObj != object)
                        {
                            // Register the key on the service
                            srv->setObject(
                                object,
                                key.first,
                                key.second,
                                objCfg.m_access,
                                objCfg.m_autoConnect,
                                objCfg.m_optional
                            );

                            if(srv->isStarted())
                            {
                                // Call the swapping callback of the service and wait for it
                                srv->swapKey(objCfg.m_key, data::Object::constCast(registeredObj)).wait();
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
            newServices.emplace(std::make_pair(uid, this->createService(*srvCfg)));

            // Debug message
            SIGHT_INFO(
                this->msgHead() + "Service '" + uid + "' has been automatically created because its "
                                                      "objects are all available."
            );
        }
    }

    std::vector<service::IService::SharedFutureType> futures;

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

void AppConfigManager::removeObjects(data::Object::sptr _obj, const std::string& _id)
{
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
            if(core::tools::fwID::exist(srvCfg.m_uid))
            {
                // Check all objects, to know if this object is optional
                bool optional = true;

                for(const auto& [key, objCfg] : srvCfg.m_objects)
                {
                    if(_id == objCfg.m_uid)
                    {
                        service::IService::sptr srv = service::get(srvCfg.m_uid);
                        SIGHT_ASSERT("No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                        optional &= objCfg.m_optional;
                        if(objCfg.m_optional)
                        {
                            if(srv->getObject(key.first, objCfg.m_access, key.second))
                            {
                                srv->resetObject(key.first, key.second, objCfg.m_access);

                                if(srv->isStarted())
                                {
                                    srv->swapKey(objCfg.m_key, _obj).wait();
                                }
                            }
                        }
                    }
                }

                if(!optional)
                {
                    // 1. Stop the service
                    service::IService::sptr srv = service::get(srvCfg.m_uid);
                    SIGHT_ASSERT(this->msgHead() + "No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                    SIGHT_ASSERT("Service " << srv->getID() << " already stopped.", !srv->isStopped());
                    srv->stop().wait();

                    for(auto it = m_startedSrv.begin() ; it != m_startedSrv.end() ; ++it)
                    {
                        if(it->lock() == srv)
                        {
                            m_startedSrv.erase(it);
                            break;
                        }
                    }

                    // 2. Destroy the service
                    SIGHT_ASSERT(
                        "Service " << srv->getID() << " must be stopped before destruction.",
                        srv->isStopped()
                    );
                    service::OSR::unregisterService(srv);

                    for(auto it = m_createdSrv.begin() ; it != m_createdSrv.end() ; ++it)
                    {
                        if(it->lock() == srv)
                        {
                            m_createdSrv.erase(it);
                            break;
                        }
                    }

                    service::IService::wptr checkSrv = srv;
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
                    service::IService::sptr srv = service::get(srvCfg.m_uid);
                    SIGHT_ASSERT(this->msgHead() + "No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                    srv->_autoDisconnect();
                    srv->_autoConnect();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void AppConfigManager::connectProxy(const std::string& _channel, const ProxyConnections& _connectCfg)
{
    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    for(const auto& signalCfg : _connectCfg.m_signals)
    {
        core::tools::Object::sptr sigSource = core::tools::fwID::getObject(signalCfg.first);
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

        core::com::HasSignals::sptr hasSignals = std::dynamic_pointer_cast<core::com::HasSignals>(sigSource);
        SIGHT_ASSERT(this->msgHead() + "Signal source not found '" + signalCfg.first + "'", hasSignals);

        core::com::SignalBase::sptr sig = hasSignals->signal(signalCfg.second);
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
        core::tools::Object::sptr slotDest = core::tools::fwID::getObject(slotCfg.first);
        core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast<core::com::HasSlots>(slotDest);
        SIGHT_ASSERT(this->msgHead() + "Slot destination not found '" + slotCfg.first + "'", hasSlots);

        core::com::SlotBase::sptr slot = hasSlots->slot(slotCfg.second);
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

std::string AppConfigManager::getUIDListAsString(const std::vector<std::string>& uidList)
{
    std::string msg = uidList.size() == 1 ? " '" : "s '";
    msg += uidList[0];
    for(auto it = uidList.begin() + 1 ; it < uidList.end() ; ++it)
    {
        msg += "', '" + *it;
    }

    msg = uidList.size() == 1 ? msg + "' is " : msg + "' are ";

    return msg;
}

//------------------------------------------------------------------------------

} // namespace sight::service
