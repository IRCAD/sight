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

void AppManager::ServiceInfo::addObject(const std::string& objId, const ::fwServices::IService::KeyType& key,
                                        const ::fwServices::IService::AccessType access, const bool autoConnect,
                                        const bool optional)
{
    ::fwServices::IService::ObjectServiceConfig cfg;
    cfg.m_uid         = objId;
    cfg.m_key         = key;
    cfg.m_access      = access;
    cfg.m_autoConnect = autoConnect;
    cfg.m_optional    = optional;

    m_objects.emplace_back(cfg);
}

//------------------------------------------------------------------------------

bool AppManager::ServiceInfo::requireObject(const std::string& objId) const
{
    auto itr = std::find_if( m_objects.begin(), m_objects.end(),
                             [&](const ::fwServices::IService::ObjectServiceConfig& objInfo)
        {

            return (objInfo.m_uid == objId && objInfo.m_access != ::fwServices::IService::AccessType::OUTPUT);
        });

    return (itr != m_objects.end());
}

//------------------------------------------------------------------------------

bool AppManager::ServiceInfo::hasAllRequiredObjects() const
{

    bool hasAllObjects = true;

    for (const auto& objectCfg : m_objects)
    {
        if (objectCfg.m_optional == false)
        {
            ::fwServices::IService::sptr srv = m_service.lock();
            if (objectCfg.m_access == ::fwServices::IService::AccessType::INPUT)
            {
                if (nullptr == srv->getInput< ::fwData::Object >(objectCfg.m_key))
                {
                    SLM_DEBUG("The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + srv->getID()
                              + "'");
                    hasAllObjects = false;
                    break;
                }
            }
            else if (objectCfg.m_access == ::fwServices::IService::AccessType::INOUT)
            {
                if (nullptr == srv->getInOut< ::fwData::Object >(objectCfg.m_key))
                {
                    SLM_DEBUG("The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + srv->getID()
                              + "'");
                    hasAllObjects = false;
                    break;
                }
            }
        }
    }
    return hasAllObjects;
}

//------------------------------------------------------------------------------

const ::fwServices::IService::ObjectServiceConfig& AppManager::ServiceInfo::getObjInfo(const std::string& objId) const
{
    auto itr = std::find_if( m_objects.begin(), m_objects.end(),
                             [&](const ::fwServices::IService::ObjectServiceConfig& objInfo)
        {

            return (objInfo.m_uid == objId);
        });

    SLM_ASSERT("Object '" + objId + "' is not registered.", itr != m_objects.end());

    return *itr;
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
    this->stopAndUnregisterServices();

    // remove all the registered objects
    for (auto obj: m_registeredObject)
    {
        this->removeObject(obj.second, obj.first);
    }
    m_registeredObject.clear();
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr AppManager::registerService(const std::string& type, const std::string& uid,
                                                         bool autoStart, bool autoUpdate)
{
    auto srv = ::fwServices::add(type, uid);
    ServiceInfo info(srv, autoStart, autoUpdate);
    m_services.emplace_back(info);

    return srv;
}

//------------------------------------------------------------------------------

void AppManager::AppManager::registerService(const ::fwServices::IService::sptr& srv, bool autoStart, bool autoUpdate)
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
                !info.hasAllRequiredObjects());
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
        if (srvInfo.m_autoStart && srvInfo.hasAllRequiredObjects())
        {
            ::fwServices::IService::sptr srv = srvInfo.m_service.lock();
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

void AppManager::stopAndUnregisterServices()
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

void AppManager::registerObject(const ::fwServices::IService::sptr& srv, const std::string& objId,
                                const ::fwServices::IService::KeyType& key,
                                const ::fwServices::IService::AccessType access,
                                const bool autoConnect, const bool optional)
{
    ServiceInfo& info = this->getServiceInfo(srv);

    info.addObject(objId, key, access, autoConnect, optional);

    if (access == ::fwServices::IService::AccessType::OUTPUT)
    {
        srv->setObjectId(key, objId);
    }
    else
    {
        auto it = m_registeredObject.find(objId);
        if (it != m_registeredObject.end())
        {
            srv->registerObject(it->second, key, access, autoConnect, optional);

            if (info.m_autoStart && info.hasAllRequiredObjects())
            {
                this->start(info);

                if (info.m_autoUpdate)
                {
                    srv->update().wait();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void AppManager::connectSignal(const std::string& channel, const ::fwServices::IService::sptr& srv,
                               const std::string sigName)
{
    ServiceInfo& info = this->getServiceInfo(srv);

    info.m_signalConnection.insert(std::make_pair(channel, sigName));
}

//------------------------------------------------------------------------------

void AppManager::connectSlot(const std::string& channel, const ::fwServices::IService::sptr& srv,
                             const std::string& slotName)
{
    ServiceInfo& info = this->getServiceInfo(srv);

    info.m_slotConnection.insert(std::make_pair(channel, slotName));
}

//------------------------------------------------------------------------------

void AppManager::connectObjectSignal(const std::string& channel, const std::string& objId,
                                     const std::string& sigName)
{
    auto it = m_objectConnection.find(objId);
    if (it != m_objectConnection.end())
    {
        ConnectionInfo info = it->second;
        info.insert(std::make_pair(channel, sigName));
    }
    else
    {
        ConnectionInfo info;
        info.insert(std::make_pair(channel, sigName));
        m_objectConnection.emplace(std::make_pair(objId, info));
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
    auto cntIt = m_objectConnection.find(id);
    if (cntIt != m_objectConnection.end())
    {
        for (auto& sigInfo : cntIt->second)
        {
            auto sig = obj->signal(sigInfo.second);
            proxy->connect(sigInfo.first, sig);
        }
    }

    std::vector< ServiceInfo > serviceToStart;
    std::vector< ::fwServices::IService::sptr > serviceToUpdate;

    for (auto& srvInfo : m_services)
    {
        if (srvInfo.requireObject(id))
        {
            ::fwServices::IService::sptr srv = srvInfo.m_service.lock();

            const ::fwServices::IService::ObjectServiceConfig& objCfg = srvInfo.getObjInfo(id);

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

            if (srvInfo.m_autoStart && srvInfo.hasAllRequiredObjects() && !srv->isStarted())
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
        if (srvInfo.requireObject(id))
        {
            ::fwServices::IService::sptr srv = srvInfo.m_service.lock();
            const ::fwServices::IService::ObjectServiceConfig& objCfg = srvInfo.getObjInfo(id);

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
    auto it = m_registeredObject.find(id);
    if (it != m_registeredObject.end())
    {
        m_registeredObject.erase(it);
    }
    auto proxy = ::fwServices::registry::Proxy::getDefault();
    auto cntIt = m_objectConnection.find(id);
    if (cntIt != m_objectConnection.end())
    {
        for (auto& sigInfo : cntIt->second)
        {
            auto sig = obj->signal(sigInfo.second);
            proxy->disconnect(sigInfo.first, sig);
        }
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

    auto proxy = ::fwServices::registry::Proxy::getDefault();

    for (auto& sigInfo : info.m_signalConnection)
    {
        auto sig = srv->signal(sigInfo.second);
        proxy->connect(sigInfo.first, sig);
    }

    for (auto& slotInfo : info.m_slotConnection)
    {
        auto slot = srv->slot(slotInfo.second);
        proxy->connect(slotInfo.first, slot);
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

    auto proxy = ::fwServices::registry::Proxy::getDefault();

    for (auto& sigInfo : info.m_signalConnection)
    {
        auto sig = srv->signal(sigInfo.second);
        proxy->disconnect(sigInfo.first, sig);
    }

    for (auto& slotInfo : info.m_slotConnection)
    {
        auto slot = srv->slot(slotInfo.second);
        proxy->disconnect(slotInfo.first, slot);
    }

    return future;
}

//------------------------------------------------------------------------------

} // namespace fwServices
