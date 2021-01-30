/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "services/AppManager.hpp"

#include <core/com/Slots.hxx>

#include <services/op/Add.hpp>
#include <services/registry/ActiveWorkers.hpp>
#include <services/registry/ObjectService.hpp>
#include <services/registry/Proxy.hpp>

namespace sight::services
{

static const core::com::Slots::SlotKeyType s_ADD_OBJECT_SLOT    = "addObject";
static const core::com::Slots::SlotKeyType s_REMOVE_OBJECT_SLOT = "removeObject";

size_t AppManager::s_counter = 0;

//------------------------------------------------------------------------------

AppManager::ServiceInfo::ServiceInfo(const services::IService::sptr& srv, const bool autoStart,
                                     const bool autoUpdate) :
    m_service(srv),
    m_autoStart(autoStart),
    m_autoUpdate(autoUpdate)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

AppManager::AppManager()
{
    newSlot(s_ADD_OBJECT_SLOT, &AppManager::addObject, this);
    newSlot(s_REMOVE_OBJECT_SLOT, &AppManager::removeObject, this);
}

//------------------------------------------------------------------------------

AppManager::~AppManager()
{
}

//------------------------------------------------------------------------------

void AppManager::create()
{
    SLM_ASSERT("create() method should not be called twice", m_addObjectConnection.expired());

    m_addObjectConnection    = services::OSR::getRegisterSignal()->connect( this->slot(s_ADD_OBJECT_SLOT) );
    m_removeObjectConnection = services::OSR::getUnregisterSignal()->connect( this->slot(s_REMOVE_OBJECT_SLOT) );

    auto defaultWorker = services::registry::ActiveWorkers::getDefaultWorker();
    core::com::HasSlots::m_slots.setWorker( defaultWorker );
}

//------------------------------------------------------------------------------

const std::string& AppManager::getID() const
{
    if (m_uid.empty())
    {
        const std::string classname = core::Demangler(*this).getLeafClassname();
        m_uid = "AppManager-" + classname + "-" + std::to_string(++s_counter);
    }
    return m_uid;
}

//------------------------------------------------------------------------------

void AppManager::destroy()
{
    this->stopAndUnregisterServices();

    m_addObjectConnection.disconnect();
    m_removeObjectConnection.disconnect();

    std::unique_lock<std::recursive_mutex> lock(m_objectMutex);
    // remove all the registered objects
    while (!m_registeredObject.empty())
    {
        auto firstObj = m_registeredObject.begin();
        this->removeObject(firstObj->second, firstObj->first);
    }
    m_registeredObject.clear();
}

//------------------------------------------------------------------------------

void AppManager::requireInput(const std::string& key, const InputType& type, const std::string& defaultValue)
{
    Input input;
    input.key          = key;
    input.type         = type;
    input.defaultValue = defaultValue;
    input.isOptional   = !defaultValue.empty();
    m_inputs[key]      = input;
}

//------------------------------------------------------------------------------

bool AppManager::checkInputs()
{
    bool isOK = true;
    for (const auto& elt: m_inputs)
    {
        const Input& input = elt.second;
        switch (input.type)
        {
            case InputType::OBJECT:
            {
                const auto obj = data::Object::dynamicCast(core::tools::fwID::getObject(input.value));
                if (obj)
                {
                    this->addObject(obj, input.value);
                }
                else if(!input.isOptional)
                {
                    const data::Object::sptr newObj = data::factory::New(input.defaultValue);
                    if (newObj)
                    {
                        this->addObject(obj, this->getInputID(input.key));
                    }
                    else
                    {
                        // TODO print a message
                        SLM_DEBUG("[" + this->getID() + "] missing input for : '" + input.key + "'")
                        isOK = false;
                    }
                }
                break;
            }
            case InputType::CHANNEL:
                if (input.value.empty() && !input.isOptional)
                {
                    SLM_DEBUG("missing input: '" + input.key + "'")
                    isOK = false;
                }
                break;
            case InputType::OTHER:
                if (input.value.empty() && !input.isOptional)
                {
                    SLM_DEBUG("missing input: '" + input.key + "'")
                    isOK = false;
                }
                break;
        }
    }
    return isOK;
}

//------------------------------------------------------------------------------

void AppManager::replaceInput(const std::string& key, const std::string& value)
{
    auto itr = m_inputs.find(key);
    if (itr != m_inputs.end())
    {
        Input& input = itr->second;
        input.value = value;
    }
}

//------------------------------------------------------------------------------

services::IService::sptr AppManager::addService(const std::string& type, const std::string& uid,
                                                bool autoStart, bool autoUpdate)
{
    std::unique_lock<std::mutex> lock(m_serviceMutex);

    auto srv = services::add(type, uid);
    this->internalAddService(srv, autoStart, autoUpdate);
    return srv;
}

//------------------------------------------------------------------------------

services::IService::sptr AppManager::addService(const std::string& type, bool autoStart, bool autoUpdate)
{
    return this->addService(type, "", autoStart, autoUpdate);
}

//------------------------------------------------------------------------------

void AppManager::AppManager::addService(const services::IService::sptr& srv, bool autoStart, bool autoUpdate)
{
    std::unique_lock<std::mutex> lock(m_serviceMutex);

    services::OSR::registerService(srv);
    auto worker = services::registry::ActiveWorkers::getDefaultWorker();
    srv->setWorker(worker);

    this->internalAddService(srv, autoStart, autoUpdate);
}

//------------------------------------------------------------------------------

void AppManager::startService(const services::IService::sptr& srv)
{
    std::unique_lock<std::mutex> lock(m_serviceMutex);

    const ServiceInfo& info = this->getServiceInfo(srv);

    FW_RAISE_IF("Service cannot be started because all the required objects are not present.",
                !srv->hasAllRequiredObjects());
    this->start(info).wait();

    if (info.m_autoUpdate)
    {
        srv->update().wait();
    }
}

//------------------------------------------------------------------------------

void AppManager::stopService(const services::IService::sptr& srv)
{
    std::unique_lock<std::mutex> lock(m_serviceMutex);

    const ServiceInfo& info = this->getServiceInfo(srv);

    this->stop(info).wait();
}

//------------------------------------------------------------------------------

void AppManager::startServices()
{

    std::vector< services::IService::SharedFutureType > futures;
    std::vector< services::IService::sptr > serviceToUpdate;

    {
        std::unique_lock<std::mutex> lock(m_serviceMutex);
        for (auto& srvInfo : m_services)
        {
            services::IService::sptr srv = srvInfo.m_service.lock();

            if (srv->isStopped() && srvInfo.m_autoStart && srv->hasAllRequiredObjects())
            {
                futures.push_back(this->start(srvInfo));

                if (srvInfo.m_autoUpdate)
                {
                    serviceToUpdate.emplace_back(srv);
                }
            }
        }
    }

    m_isStarted = true;

    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));
    futures.clear();

    for (const auto& srv : serviceToUpdate)
    {
        futures.push_back(srv->update());
    }
    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));
}

//------------------------------------------------------------------------------

void AppManager::stopAndUnregisterServices()
{
    std::vector< services::IService::SharedFutureType > futures;

    std::unique_lock<std::mutex> lock(m_serviceMutex);

    // stop the started services
    while (!m_startedService.empty())
    {
        const auto& srv         = m_startedService.back();
        const ServiceInfo& info = this->getServiceInfo(srv);
        futures.emplace_back(this->stop(info));
    }
    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

    // unregister the services
    for(auto& srv : m_services)
    {
        services::OSR::unregisterService(srv.m_service.lock());
    }
    m_startedService.clear();
    m_services.clear();
    m_isStarted = false;
}

//------------------------------------------------------------------------------

void AppManager::addProxyConnection(const helper::ProxyConnections& proxy)
{
    std::unique_lock<std::mutex> lock(m_serviceMutex);

    static size_t count = 0;
    std::string channel = proxy.m_channel;
    if (channel == "undefined")
    {
        channel = "AppManager_channel_" + std::to_string(count++);
    }
    for (const auto& sigInfo: proxy.m_signals)
    {
        auto& itSrv                        = m_proxies[sigInfo.first];
        helper::ProxyConnections& objProxy = itSrv.m_proxyCnt[channel];
        objProxy.addSignalConnection(sigInfo);
        objProxy.m_channel = channel;
    }
    for (const auto& slotInfo: proxy.m_slots)
    {
        auto& itSrv                        = m_proxies[slotInfo.first];
        helper::ProxyConnections& objProxy = itSrv.m_proxyCnt[channel];
        objProxy.addSlotConnection(slotInfo);
        objProxy.m_channel = channel;
    }
}

//------------------------------------------------------------------------------

void AppManager::addObject(data::Object::sptr obj, const std::string& id)
{
    std::unique_lock<std::recursive_mutex> lock(m_objectMutex);

    auto it = m_registeredObject.find(id);
    if (it != m_registeredObject.end())
    {
        if (it->second == obj)
        {
            SLM_WARN("Object '" + id + "' is already registered.");
            return;
        }
        else
        {
            SLM_WARN("Object '" + id + "' has not been unregistered, we will do it.");
            this->removeObject(it->second, id);
        }
    }

    auto proxy = services::registry::Proxy::getDefault();

    auto proxyIt = m_proxies.find(id);
    if (proxyIt != m_proxies.end())
    {
        for (auto& cntInfo : proxyIt->second.m_proxyCnt)
        {
            for (auto& sigInfo : cntInfo.second.m_signals)
            {
                auto sig = obj->signal(sigInfo.second);
                proxy->connect(cntInfo.second.m_channel, sig);
            }
        }
    }

    std::vector< ServiceInfo > serviceToStart;
    std::vector< services::IService::sptr > serviceToUpdate;

    std::unique_lock<std::mutex> lockSrv(m_serviceMutex);

    for (auto& srvInfo : m_services)
    {
        services::IService::sptr srv = srvInfo.m_service.lock();
        if (srv->hasObjInfoFromId(id))
        {
            const services::IService::ObjectServiceConfig& objCfg = srv->getObjInfoFromId(id);

            auto registeredObj = services::OSR::getRegistered(objCfg.m_key, objCfg.m_access, srv);

            if (registeredObj != obj)
            {
                if (srv->isStarted() && !objCfg.m_optional)
                {
                    SLM_ERROR("Service should be stopped.");
                    this->stop(srvInfo).wait();
                }

                // unregister the previous object
                if (registeredObj != nullptr)
                {
                    srv->unregisterObject(objCfg.m_key, objCfg.m_access);
                }

                // Register the key on the service
                srv->registerObject(obj, objCfg.m_key, objCfg.m_access, objCfg.m_autoConnect, objCfg.m_optional);

                if (objCfg.m_optional && srv->isStarted())
                {
                    // Call the swapping callback of the service and wait for it
                    srv->swapKey(objCfg.m_key, data::Object::constCast(registeredObj)).wait();
                }
            }

            if (srvInfo.m_autoStart && srv->hasAllRequiredObjects() && !srv->isStarted())
            {
                serviceToStart.emplace_back(srvInfo);

                if (srvInfo.m_autoUpdate)
                {
                    serviceToUpdate.emplace_back(srv);
                }
            }
        }
    }

    // Start the services only if startService() has been called first.
    if (m_isStarted)
    {
        std::vector< services::IService::SharedFutureType > futures;
        for (const auto& srvInfo : serviceToStart)
        {
            futures.push_back(this->start(srvInfo));
        }
        std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));
        futures.clear();

        for (const auto& srv : serviceToUpdate)
        {
            futures.push_back(srv->update());
        }
        std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));
    }

    m_registeredObject.insert(std::make_pair(id, obj));
}

//------------------------------------------------------------------------------

void AppManager::removeObject(data::Object::sptr obj, const std::string& id)
{
    std::unique_lock<std::recursive_mutex> lock(m_objectMutex);

    {
        std::unique_lock<std::mutex> lockSrv(m_serviceMutex);

        std::vector< services::IService::sptr > serviceToStop;

        for (auto& srvInfo : m_services)
        {
            FW_RAISE_IF("service is expired", srvInfo.m_service.expired());
            services::IService::sptr srv = srvInfo.m_service.lock();
            if (srv->hasObjInfoFromId(id))
            {
                const services::IService::ObjectServiceConfig& objCfg = srv->getObjInfoFromId(id);

                if (services::OSR::isRegistered(objCfg.m_key, objCfg.m_access, srv))
                {
                    if (srv->isStarted() && !objCfg.m_optional)
                    {
                        this->stop(srvInfo).wait();
                    }

                    srv->unregisterObject(objCfg.m_key, objCfg.m_access);

                    if (objCfg.m_optional && srv->isStarted())
                    {
                        srv->swapKey(objCfg.m_key, obj).wait();
                    }
                }
            }
        }
    }

    auto proxy = services::registry::Proxy::getDefault();

    auto proxyIt = m_proxies.find(id);
    if (proxyIt != m_proxies.end())
    {
        for (auto& cntInfo : proxyIt->second.m_proxyCnt)
        {
            for (auto& sigInfo : cntInfo.second.m_signals)
            {
                auto sig = obj->signal(sigInfo.second);
                proxy->disconnect(cntInfo.second.m_channel, sig);
            }
        }
    }
    auto it = m_registeredObject.find(id);
    if (it != m_registeredObject.end())
    {
        m_registeredObject.erase(it);
    }
}

//------------------------------------------------------------------------------

data::Object::sptr AppManager::getObject(const std::string& id) const
{
    std::unique_lock<std::recursive_mutex> lock(m_objectMutex);

    data::Object::sptr obj;

    auto itr = m_registeredObject.find(id);
    if (itr != m_registeredObject.end())
    {
        obj = itr->second;
    }
    return obj;
}

//------------------------------------------------------------------------------

void AppManager::internalAddService(const services::IService::sptr& srv, const bool autoStart,
                                    const bool autoUpdate)
{
    ServiceInfo info(srv, autoStart, autoUpdate);
    m_services.emplace_back(info);

    // register the object to the service
    for (const auto& obj : m_registeredObject)
    {
        if (srv->hasObjInfoFromId(obj.first))
        {
            const services::IService::ObjectServiceConfig& objCfg = srv->getObjInfoFromId(obj.first);

            // Register the key on the service
            srv->registerObject(obj.second, objCfg.m_key, objCfg.m_access, objCfg.m_autoConnect, objCfg.m_optional);
        }
    }
    if(autoStart && m_isStarted && srv->hasAllRequiredObjects())
    {
        this->start(info);
        if (info.m_autoUpdate)
        {
            srv->update().wait();
        }
    }
}

//------------------------------------------------------------------------------

AppManager::ServiceInfo& AppManager::getServiceInfo(const services::IService::sptr& srv)
{
    auto itr = std::find_if( m_services.begin(), m_services.end(), [&](const ServiceInfo& srvInfo)
        {

            return (srvInfo.m_service.lock() == srv);
        });

    SLM_ASSERT("Service '" + srv->getID() + "' is not registered.", itr != m_services.end());
    return *itr;
}

//------------------------------------------------------------------------------

const AppManager::ServiceInfo& AppManager::getServiceInfo(const services::IService::sptr& srv) const
{
    const auto itr = std::find_if( m_services.begin(), m_services.end(), [&](const ServiceInfo& srvInfo)
        {

            return (srvInfo.m_service.lock() == srv);
        });

    SLM_ASSERT("Service '" + srv->getID() + "' is not registered.", itr != m_services.end());
    return *itr;
}

//------------------------------------------------------------------------------

std::string AppManager::getInputID(const std::string& id) const
{
    std::string uid = this->getID() + "-" + id;
    const auto itr  = m_inputs.find(id);
    if (itr != m_inputs.end())
    {
        uid = itr->second.value;
    }
    return uid;
}

//------------------------------------------------------------------------------

services::IService::SharedFutureType AppManager::start(const ServiceInfo& info)
{
    services::IService::sptr srv = info.m_service.lock();

    auto proxyIt = m_proxies.find(srv->getID());
    if (proxyIt != m_proxies.end())
    {
        for (const auto& proxyCnt: proxyIt->second.m_proxyCnt)
        {
            srv->addProxyConnection(proxyCnt.second);
        }
    }

    services::IService::SharedFutureType future = srv->start();
    m_startedService.emplace_back(srv);

    return future;
}

//------------------------------------------------------------------------------

services::IService::SharedFutureType AppManager::stop(const ServiceInfo& info)
{
    services::IService::sptr srv = info.m_service.lock();

    services::IService::SharedFutureType future = srv->stop();
    auto it                                     = std::find(m_startedService.begin(), m_startedService.end(), srv);
    m_startedService.erase(it);

    return future;
}

//------------------------------------------------------------------------------

} // namespace sight::services
