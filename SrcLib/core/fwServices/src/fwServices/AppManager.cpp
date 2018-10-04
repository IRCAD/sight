/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/AppManager.hpp"

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <fwCom/Slots.hxx>

#include <boost/foreach.hpp>
#include <boost/make_unique.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>

namespace fwServices
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECT_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECT_SLOT = "removeObject";

//------------------------------------------------------------------------------

AppManager::ServiceInfo::ServiceInfo(const ::fwServices::IService::sptr& srv, const bool autoStart,
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
    m_addObjectConnection    = ::fwServices::OSR::getRegisterSignal()->connect( this->slot(s_ADD_OBJECT_SLOT) );
    m_removeObjectConnection = ::fwServices::OSR::getUnregisterSignal()->connect( this->slot(s_REMOVE_OBJECT_SLOT) );

    auto defaultWorker = ::fwServices::registry::ActiveWorkers::getDefaultWorker();
    ::fwCom::HasSlots::m_slots.setWorker( defaultWorker );
}

//------------------------------------------------------------------------------

void AppManager::destroy()
{
    this->stopAndUnaddServices();

    // remove all the registered objects
    while (!m_registeredObject.empty())
    {
        auto firstObj = m_registeredObject.begin();
        this->removeObject(firstObj->second, firstObj->first);
    }
    m_registeredObject.clear();
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr AppManager::addService(const std::string& type, const std::string& uid,
                                                    bool autoStart, bool autoUpdate)
{
    auto srv = ::fwServices::add(type, uid);
    ServiceInfo info(srv, autoStart, autoUpdate);
    m_services.emplace_back(info);

    return srv;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr AppManager::addService(const std::string& type, bool autoStart, bool autoUpdate)
{
    return this->addService(type, "", autoStart, autoUpdate);
}

//------------------------------------------------------------------------------

void AppManager::AppManager::addService(const ::fwServices::IService::sptr& srv, bool autoStart, bool autoUpdate)
{
    ServiceInfo info(srv, autoStart, autoUpdate);
    m_services.emplace_back(info);

    ::fwServices::OSR::registerService(srv);
    auto worker = ::fwServices::registry::ActiveWorkers::getDefaultWorker();
    srv->setWorker(worker);
}

//------------------------------------------------------------------------------

void AppManager::startService(const ::fwServices::IService::sptr& srv)
{
    const ServiceInfo& info = this->getServiceInfo(srv);

    FW_RAISE_IF("Service cannot be started because all the required objects are not present.",
                !srv->hasAllRequiredObjects());
    this->start(info).wait();
}

//------------------------------------------------------------------------------

void AppManager::stopService(const ::fwServices::IService::sptr& srv)
{
    const ServiceInfo& info = this->getServiceInfo(srv);

    this->stop(info).wait();
}

//------------------------------------------------------------------------------

void AppManager::startServices()
{
    std::vector< ::fwServices::IService::SharedFutureType > futures;
    std::vector< ::fwServices::IService::sptr > serviceToUpdate;

    for (auto& srvInfo : m_services)
    {
        ::fwServices::IService::sptr srv = srvInfo.m_service.lock();
        if (srvInfo.m_autoStart && srv->hasAllRequiredObjects())
        {
            futures.push_back(this->start(srvInfo));

            if (srvInfo.m_autoUpdate)
            {
                serviceToUpdate.emplace_back(srv);
            }
        }
    }

    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));
    futures.clear();

    for (const auto& srv : serviceToUpdate)
    {
        futures.push_back(srv->update());
    }
    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));
}

//------------------------------------------------------------------------------

void AppManager::stopAndUnaddServices()
{
    std::vector< ::fwServices::IService::SharedFutureType > futures;

    // stop the started services
    BOOST_REVERSE_FOREACH(auto& srv, m_startedService)
    {
        const ServiceInfo& info = this->getServiceInfo(srv);
        futures.emplace_back(this->stop(info));
    }
    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

    // unregister the services
    for(auto& srv : m_services)
    {
        ::fwServices::OSR::unregisterService(srv.m_service.lock());
    }
    m_startedService.clear();
    m_services.clear();
}

//------------------------------------------------------------------------------

void AppManager::addProxyConnection(const helper::ProxyConnections& proxy)
{
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

void AppManager::addObject(::fwData::Object::sptr obj, const std::string& id)
{
    auto it = m_registeredObject.find(id);
    if (it != m_registeredObject.end())
    {
        if (it->second == obj)
        {
            OSLM_WARN("Object '" + id + "' is already registered.");
            return;
        }
        else
        {
            OSLM_WARN("Object '" + id + "' has not been unregistered, we will do it.");
            this->removeObject(it->second, id);
        }
    }

    auto proxy = ::fwServices::registry::Proxy::getDefault();

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
    std::vector< ::fwServices::IService::sptr > serviceToUpdate;

    for (auto& srvInfo : m_services)
    {
        ::fwServices::IService::sptr srv = srvInfo.m_service.lock();
        if (srv->hasObjInfoFromId(id))
        {
            const ::fwServices::IService::ObjectServiceConfig& objCfg = srv->getObjInfoFromId(id);

            auto registeredObj = ::fwServices::OSR::getRegistered(objCfg.m_key, objCfg.m_access, srv);

            if (registeredObj != obj)
            {
                if (srv->isStarted() && !objCfg.m_optional)
                {
                    OSLM_ERROR("Service should be stopped.");
                    this->stop(srvInfo).wait();
                }

                // unregister the previous object
                if (registeredObj != nullptr)
                {
                    srv->unregisterObject(objCfg.m_key, objCfg.m_access);
                }

                // Register the key on the service
                srv->registerObject(obj, objCfg.m_key, objCfg.m_access, objCfg.m_autoConnect, objCfg.m_optional);

                if (objCfg.m_optional)
                {
                    // Call the swapping callback of the service and wait for it
                    srv->swapKey(objCfg.m_key, ::fwData::Object::constCast(registeredObj)).wait();
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

    std::vector< ::fwServices::IService::SharedFutureType > futures;
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

    m_registeredObject.insert(std::make_pair(id, obj));
}

//------------------------------------------------------------------------------

void AppManager::removeObject(::fwData::Object::sptr obj, const std::string& id)
{
    std::vector< ::fwServices::IService::sptr > serviceToStop;

    for (auto& srvInfo : m_services)
    {
        FW_RAISE_IF("service is expired", srvInfo.m_service.expired());
        ::fwServices::IService::sptr srv = srvInfo.m_service.lock();
        if (srv->hasObjInfoFromId(id))
        {
            const ::fwServices::IService::ObjectServiceConfig& objCfg = srv->getObjInfoFromId(id);

            if (::fwServices::OSR::isRegistered(objCfg.m_key, objCfg.m_access, srv))
            {
                if (srv->isStarted() && !objCfg.m_optional)
                {
                    this->stop(srvInfo).wait();
                }

                srv->unregisterObject(objCfg.m_key, objCfg.m_access);

                if (objCfg.m_optional)
                {
                    srv->swapKey(objCfg.m_key, obj).wait();
                }
            }
        }
    }

    auto proxy = ::fwServices::registry::Proxy::getDefault();

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

::fwData::Object::sptr AppManager::getObject(const std::string& id) const
{
    ::fwData::Object::sptr obj;

    auto itr = m_registeredObject.find(id);
    if (itr != m_registeredObject.end())
    {
        obj = itr->second;
    }
    return obj;
}

//------------------------------------------------------------------------------

AppManager::ServiceInfo& AppManager::getServiceInfo(const ::fwServices::IService::sptr& srv)
{
    auto itr = std::find_if( m_services.begin(), m_services.end(), [&](const ServiceInfo& srvInfo)
        {

            return (srvInfo.m_service.lock() == srv);
        });

    SLM_ASSERT("Service '" + srv->getID() + "' is not registered.", itr != m_services.end());
    return *itr;
}

//------------------------------------------------------------------------------

const AppManager::ServiceInfo& AppManager::getServiceInfo(const ::fwServices::IService::sptr& srv) const
{
    const auto itr = std::find_if( m_services.begin(), m_services.end(), [&](const ServiceInfo& srvInfo)
        {

            return (srvInfo.m_service.lock() == srv);
        });

    SLM_ASSERT("Service '" + srv->getID() + "' is not registered.", itr != m_services.end());
    return *itr;
}

//------------------------------------------------------------------------------

::fwServices::IService::SharedFutureType AppManager::start(const ServiceInfo& info)
{
    ::fwServices::IService::sptr srv = info.m_service.lock();

    auto proxyIt = m_proxies.find(srv->getID());
    if (proxyIt != m_proxies.end())
    {
        for (const auto& proxyCnt: proxyIt->second.m_proxyCnt)
        {
            srv->addProxyConnection(proxyCnt.second);
        }
    }

    ::fwServices::IService::SharedFutureType future = srv->start();
    m_startedService.emplace_back(srv);

    return future;
}

//------------------------------------------------------------------------------

::fwServices::IService::SharedFutureType AppManager::stop(const ServiceInfo& info)
{
    ::fwServices::IService::sptr srv = info.m_service.lock();

    ::fwServices::IService::SharedFutureType future = srv->stop();
    auto it = std::find(m_startedService.begin(), m_startedService.end(), srv);
    m_startedService.erase(it);

    return future;
}

//------------------------------------------------------------------------------

} // namespace fwServices
