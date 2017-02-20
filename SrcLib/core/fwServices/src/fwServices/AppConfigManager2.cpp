/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/AppConfigManager2.hpp"

#include "fwServices/helper/Config.hpp"
#include "fwServices/op/Get.hpp"
#include "fwServices/registry/ActiveWorkers.hpp"
#include "fwServices/registry/Proxy.hpp"
#include "fwServices/registry/ServiceConfig.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/operations.hpp>

#include <boost/foreach.hpp>

namespace fwServices
{

// ------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

// ------------------------------------------------------------------------

AppConfigManager2::AppConfigManager2() :
    m_proxyID(0),
    m_isUnitTest(false)
{
    SLM_ASSERT("Can't mix V1 and V2 appConfigs", s_VERSION == 0 || s_VERSION == 2);
    s_VERSION = 2;
    ::fwServices::IService::setVersion(2);

    newSlot(s_ADD_OBJECTS_SLOT, &AppConfigManager2::addObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &AppConfigManager2::removeObjects, this);

    auto defaultWorker = registry::ActiveWorkers::getDefault()->getWorker( registry::ActiveWorkers::s_DEFAULT_WORKER );
    ::fwCom::HasSlots::m_slots.setWorker( defaultWorker );
}

// ------------------------------------------------------------------------

AppConfigManager2::~AppConfigManager2()
{
    SLM_ASSERT("Manager must be stopped before destruction.", m_state == STATE_DESTROYED);
}

// ------------------------------------------------------------------------

void AppConfigManager2::setConfig(const std::string& configId, const FieldAdaptorType& replaceFields)
{
    m_configId = configId;
    m_cfgElem  = registry::AppConfig2::getDefault()->getAdaptedTemplateConfig( configId, replaceFields, !m_isUnitTest );
}

// ------------------------------------------------------------------------

void AppConfigManager2::setConfig(const std::string& configId, const ::fwData::Composite::csptr& replaceFields)
{
    m_configId = configId;
    m_cfgElem  = registry::AppConfig2::getDefault()->getAdaptedTemplateConfig( configId, replaceFields, !m_isUnitTest );
}

// ------------------------------------------------------------------------

void AppConfigManager2::launch()
{
    FW_PROFILE("launch");

    this->startBundle();
    this->create();
    this->start();
    this->update();
}

// ------------------------------------------------------------------------

void AppConfigManager2::stopAndDestroy()
{
    this->stop();
    this->destroy();
}

// ------------------------------------------------------------------------

void AppConfigManager2::startBundle()
{
    SLM_WARN_IF("Bundle is not specified, it can not be started.", m_configId.empty());
    if (!m_configId.empty() && !m_isUnitTest)
    {
        std::shared_ptr< ::fwRuntime::Bundle > bundle = registry::AppConfig2::getDefault()->getBundle(m_configId);
        SLM_WARN_IF("Bundle '" + bundle->getIdentifier() + "' (used for '" + m_configId + "') is already started !",
                    bundle->isStarted());
        if (!bundle->isStarted())
        {
            bundle->start();
        }
    }
}

// ------------------------------------------------------------------------

void AppConfigManager2::create()
{
    SLM_ASSERT("Manager already running.", m_state == STATE_DESTROYED);

    // Create the dummy object for new services that don't work on any object
    // For now this dummy object will also contain all the "deferred" objects
    m_tmpRootObject = ::fwData::Composite::New();

    m_addObjectConnection    = ::fwServices::OSR::getRegisterSignal()->connect( this->slot(s_ADD_OBJECTS_SLOT) );
    m_removeObjectConnection = ::fwServices::OSR::getUnregisterSignal()->connect( this->slot(s_REMOVE_OBJECTS_SLOT) );

    this->createObjects(m_cfgElem);
    this->createConnections();
    this->createServices(m_cfgElem);

    m_state = STATE_CREATED;
}

// ------------------------------------------------------------------------

void AppConfigManager2::start()
{
    SLM_ASSERT("Manager must be created first.", m_state == STATE_CREATED || m_state == STATE_STOPPED);

    this->processStartItems();
    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->startConfig();
    }

    m_state = STATE_STARTED;
}

// ------------------------------------------------------------------------

void AppConfigManager2::update()
{
    this->processUpdateItems();
    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->updateConfig();
    }
}

// ------------------------------------------------------------------------

void AppConfigManager2::stop()
{
    SLM_ASSERT("Manager is not started, cannot stop.", m_state == STATE_STARTED);

    ::fwServices::OSR::getRegisterSignal()->disconnect( this->slot(s_ADD_OBJECTS_SLOT) );
    ::fwServices::OSR::getUnregisterSignal()->disconnect( this->slot(s_REMOVE_OBJECTS_SLOT) );

    // Disconnect configuration connections
    this->destroyProxies();

    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->stopConfig();
    }
    this->stopStartedServices();

    OSLM_INFO("Parsing OSR after stopping the config :" << std::endl
                                                        << ::fwServices::OSR::getRegistryInformation());
    m_state = STATE_STOPPED;
}

// ------------------------------------------------------------------------

void AppConfigManager2::destroy()
{
    SLM_ASSERT("Manager is not stopped, cannot destroy.", m_state == STATE_STOPPED || m_state == STATE_CREATED);

    for(auto& createdObject : m_createdObjects)
    {
        createdObject.second.second->destroyConfig();
    }
    this->destroyCreatedServices();

    OSLM_INFO("Parsing OSR after destroying the config :" << std::endl
                                                          << ::fwServices::OSR::getRegistryInformation());

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

void AppConfigManager2::setIsUnitTest(bool isUnitTest)
{
    m_isUnitTest = isUnitTest;
}

// ------------------------------------------------------------------------

fwData::Object::sptr AppConfigManager2::getConfigRoot() const
{
    if (m_createdObjects.empty())
    {
        return m_tmpRootObject;
    }
    return m_createdObjects.begin()->second.first;
}

// ------------------------------------------------------------------------

fwData::Object::sptr AppConfigManager2::findObject(const std::string& uid, const std::string& errMsgTail) const
{
    ::fwData::Object::sptr obj;

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

        SLM_ASSERT(this->msgHead() + "Object '" + uid + "' has not been found" + errMsgTail,
                   itDeferredObj != m_deferredObjects.end());
        obj = itDeferredObj->second.m_object;
    }
    return obj;
}

// ------------------------------------------------------------------------

::fwData::Object::sptr AppConfigManager2::getNewObject(ConfigAttribute type, ConfigAttribute uid) const
{
    // Building object structure
    SPTR(::fwRuntime::Extension) ext = ::fwRuntime::findExtension(type.first);
    if (ext)
    {
        const std::string className = ::fwCore::getClassname< ::fwData::Object >();
        SLM_ASSERT("Extension and classname are different.",
                   ext->getPoint() == className);

        // Start dll to retrieve proxy and register object
        ext->getBundle()->start();
    }

    ::fwData::Object::sptr obj = ::fwData::factory::New(type.first);
    SLM_ASSERT("Factory failed to build object : " + type.first, obj);

    if (uid.second)
    {
        SLM_ASSERT("Object already has an UID.", !obj->hasID());
        SLM_ASSERT("UID " << uid.first << " already exists.", !::fwTools::fwID::exist(uid.first));
        obj->setID(uid.first);
    }

    return obj;
}

// ------------------------------------------------------------------------

::fwData::Object::sptr AppConfigManager2::getNewObject(ConfigAttribute type, const std::string& uid) const
{
    return this->getNewObject(type, ConfigAttribute(uid, true));
}

// ------------------------------------------------------------------------

::fwData::Object::sptr AppConfigManager2::getRefObject(ConfigAttribute type, const std::string& uid) const
{
    SLM_ASSERT(this->msgHead() + "Object with UID \"" + uid + "\" doesn't exist.", ::fwTools::fwID::exist(uid));
    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(uid));

    SLM_ASSERT(this->msgHead() + "The UID '" + uid + "' does not reference any object.", obj);

    if (type.second)
    {
        SLM_ASSERT(this->msgHead() + "Object with UID \"" + uid +
                   "\" has a different type (\"" + obj->getClassname() + "\" != \"" + type.first + "\").",
                   type.first == obj->getClassname());
    }
    return obj;
}

// ------------------------------------------------------------------------

::fwServices::IService::sptr AppConfigManager2::getNewService(const std::string& uid, const std::string& implType) const
{
    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();

    ::fwServices::IService::sptr srv = srvFactory->create(implType);

    SLM_ASSERT("Factory could not create service of type <" + implType + ">.", srv);
    SLM_ASSERT("Service already has an UID.", !srv->hasID());

    SLM_ASSERT(this->msgHead() + "UID " + uid + " already exists.", !::fwTools::fwID::exist(uid));
    if (!uid.empty())
    {
        srv->setID(uid);
    }

    return srv;
}

// ------------------------------------------------------------------------

void AppConfigManager2::stopStartedServices()
{
    std::vector< ::fwServices::IService::SharedFutureType > futures;

    BOOST_REVERSE_FOREACH(::fwServices::IService::wptr w_srv, m_startedSrv)
    {
        SLM_ASSERT("Service expired.", !w_srv.expired());

        const ::fwServices::IService::sptr srv = w_srv.lock();
        OSLM_ASSERT("Service " << srv->getID() << " already stopped.", !srv->isStopped());
        futures.push_back(srv->stop());
    }
    m_startedSrv.clear();
    ::boost::wait_for_all(futures.begin(), futures.end());
}

// ------------------------------------------------------------------------

void AppConfigManager2::destroyCreatedServices()
{
    BOOST_REVERSE_FOREACH(::fwServices::IService::wptr w_srv, m_createdSrv)
    {
        SLM_ASSERT("Service expired.", !w_srv.expired());

        const ::fwServices::IService::sptr srv = w_srv.lock();
        OSLM_ASSERT("Service " << srv->getID() << " must be stopped before destruction.", srv->isStopped());
        ::fwServices::OSR::unregisterService(srv);
    }
    m_createdSrv.clear();
}

// ------------------------------------------------------------------------

void AppConfigManager2::processStartItems()
{
    std::vector< ::fwServices::IService::SharedFutureType > futures;

    for(const auto& elem :  m_cfgElem->getElements())
    {
        if (elem->getName() == "start")
        {
            SLM_ASSERT("Missing attribute \"uid\".", elem->hasAttribute("uid"));
            const std::string uid = elem->getAttributeValue("uid");
            SLM_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!::fwTools::fwID::exist(uid))
            {
                if(m_deferredServices.find(uid) != m_deferredServices.end())
                {
                    m_deferredStartSrv.push_back(uid);
                    SLM_INFO( this->msgHead() + "Start for service '" + uid + "' will be deferred since at least one "
                              "of its data is missing. With DEBUG log level, you can know which are the "
                              "missing objects.");
                }
                else
                {
                    SLM_FATAL( this->msgHead() + "Start is requested for service '" + uid +
                               "', but it does not exist.");
                }
            }
            else
            {
                const ::fwServices::IService::sptr srv = ::fwServices::get(uid);
                SLM_FATAL_IF( this->msgHead() + "Start is requested for service '" + uid + "', though this identifier "
                              "exists, this is not a service.", !srv);

                futures.push_back(srv->start());

                m_startedSrv.push_back(srv);
            }

        }
    }
    ::boost::wait_for_all(futures.begin(), futures.end());
}

// ------------------------------------------------------------------------

void AppConfigManager2::processUpdateItems()
{
    std::vector< ::fwServices::IService::SharedFutureType > futures;

    for(const auto& elem :  m_cfgElem->getElements())
    {
        if (elem->getName() == "update")
        {
            const std::string uid = elem->getAttributeValue("uid");
            SLM_ASSERT("\"uid\" attribute is empty.", !uid.empty());

            if(!::fwTools::fwID::exist(uid))
            {
                if(m_deferredServices.find(uid) != m_deferredServices.end())
                {
                    m_deferredUpdateSrv.push_back(uid);
                    SLM_INFO( this->msgHead() + "Update for service '" + uid + "'will be deferred since at least one "
                              "of its data is missing. With DEBUG log level, you can know which are the "
                              "missing objects.");
                }
                else
                {
                    SLM_FATAL( this->msgHead() + "Update is requested for service '" + uid +
                               "', but it does not exist.");
                }
            }
            else
            {
                const ::fwServices::IService::sptr srv = ::fwServices::get(uid);
                SLM_FATAL_IF( this->msgHead() + "Update is requested for service '" + uid +
                              "', though this identifier exists, this is not a service.", !srv);

                futures.push_back(srv->update());
            }
        }
    }

    ::boost::wait_for_all(futures.begin(), futures.end());
}

// ------------------------------------------------------------------------

void AppConfigManager2::createObjects(::fwRuntime::ConfigurationElement::csptr cfgElem)
{
    for(const auto& elem :  cfgElem->getElements())
    {
        if (elem->getName() == "object")
        {
            // Get attributes

            // Id
            ConfigAttribute id("", false);
            if (elem->hasAttribute("uid"))
            {
                id.first = elem->getAttributeValue("uid");
                SLM_ASSERT(this->msgHead() + "\"uid\" attribute is empty.", !id.first.empty());
                id.second = true;
            }

            // Type
            ConfigAttribute type("", false);
            if (elem->hasAttribute("type"))
            {
                type.first = elem->getAttributeValue("type");
                SLM_ASSERT(this->msgHead() + "\"type\" attribute is empty.", !type.first.empty());
                SLM_ASSERT(this->msgHead() + "\"type\" must be a rooted namespace.", type.first.substr(0, 2) == "::");
                type.second = true;
            }

            // Build mode
            ConfigAttribute buildMode("", false);
            if (elem->hasAttribute("src"))
            {
                buildMode.first = elem->getAttributeValue("src");
                SLM_ASSERT("this->msgHead() + \"src\" attribute is empty.", !buildMode.first.empty());

                SLM_ASSERT("Unhandled build mode (bad \"src\" attribute). Must be \"new\", \"deferred\" or \"ref\".",
                           buildMode.first == "ref" || buildMode.first == "src" || buildMode.first == "deferred");
                buildMode.second = true;
            }

            if(buildMode.first == "deferred")
            {
                SLM_ASSERT(this->msgHead() + "Missing attribute \"id\".", id.second);
                auto ret = m_deferredObjects.insert( std::make_pair(id.first, DeferredObjectType()));
                SLM_ASSERT(this->msgHead() + "Object '" + id.first + "' already exists in this config.", ret.second);
            }
            else
            {
                // Creation of a new object
                ::fwData::Object::sptr obj;

                // Create new or get the referenced object
                if (buildMode.second && buildMode.first == "ref")
                {
                    SLM_ASSERT(this->msgHead() + "Missing attribute \"id\".", id.second);
                    obj = this->getRefObject(type, id.first);
                }
                else
                {
                    obj = this->getNewObject(type, id);
                }

                // Get the object parser associated with the object type
                const auto srvFactory = ::fwServices::registry::ServiceFactory::getDefault();

                std::string srvImpl = srvFactory->getDefaultImplementationIdFromObjectAndType(
                    obj->getClassname(), "::fwServices::IXMLParser");

                ::fwServices::IService::sptr srv = srvFactory->create("::fwServices::IXMLParser", srvImpl);
                auto objectParser = ::fwServices::IXMLParser::dynamicCast(srv);
                objectParser->setObjectConfig(elem);
                objectParser->createConfig(obj);

                m_createdObjects[id.first] = std::make_pair( obj, objectParser);
            }
        }
    }
}

// ------------------------------------------------------------------------

void AppConfigManager2::createServices(::fwRuntime::ConfigurationElement::csptr cfgElem)
{
    for(const auto& elem : cfgElem->getElements())
    {
        if (elem->getName() == "service")
        {
            // Parse the service configuration
            ServiceConfig srvConfig = ::fwServices::helper::Config::parseService(elem, this->msgHead());

            // Check if we can start the service now or if we must deferred its creation
            bool createService = true;
            std::vector<std::string> uids;

            for(const auto& objectCfg : srvConfig.m_objects)
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
            }

            if(createService)
            {
                this->createService(srvConfig);
            }
            else
            {
                // Check if a service hasn't been already created with this uid
                SLM_ASSERT(this->msgHead() + "UID " + srvConfig.m_uid + " already exists.",
                           !::fwTools::fwID::exist(srvConfig.m_uid));

                const std::string msg = AppConfigManager2::getUIDListAsString(uids);
                SLM_INFO(this->msgHead() + "Service '" + srvConfig.m_uid +
                         "' has not been created because the object" + msg + "not available.");
            }
        }
        else if (elem->getName() == "serviceList")
        {
            this->createServices(elem);
        }
    }
}

// ------------------------------------------------------------------------

::fwServices::IService::sptr AppConfigManager2::createService(const ::fwServices::IService::Config& srvConfig)
{
    // Create and bind service
    const ::fwServices::IService::sptr srv = this->getNewService(srvConfig.m_uid, srvConfig.m_type);
    m_createdSrv.push_back(srv);

    if (!srvConfig.m_worker.empty())
    {
        ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
        ::fwThread::Worker::sptr worker;
        worker = activeWorkers->getWorker(srvConfig.m_worker);
        if (!worker)
        {
            worker = ::fwThread::Worker::New();
            activeWorkers->addWorker(srvConfig.m_worker, worker);
        }
        srv->setWorker(worker);
    }

    std::string errMsgTail = " when creating service '" + srvConfig.m_uid + "'.";

    for(const auto& objectCfg : srvConfig.m_objects)
    {
        srv->setObjectId(objectCfg.m_key, objectCfg.m_uid);

        ::fwData::Object::sptr obj = this->findObject(objectCfg.m_uid, errMsgTail);

        SLM_ASSERT(this->msgHead() + "Object '" + objectCfg.m_uid + "' has not been found" + errMsgTail,
                   (!objectCfg.m_optional && obj) || objectCfg.m_optional);
        if((obj || !objectCfg.m_optional) && objectCfg.m_access != ::fwServices::IService::AccessType::OUTPUT)
        {
            ::fwServices::OSR::registerService(obj, objectCfg.m_key, objectCfg.m_access, srv);
        }
    }

    // Ok for now we assume we always need the root composite
    // This is only true for services that will need to perform some stuff like adding new objects, etc...
    // We will see in the future if this should be replaced or not
    {
        srv->setObjectId(::fwServices::IService::s_DEFAULT_OBJECT, "defaultObjectId");

        ::fwServices::OSR::registerService(m_tmpRootObject, ::fwServices::IService::s_DEFAULT_OBJECT,
                                           ::fwServices::IService::AccessType::INOUT, srv);
    }

    // Set the size of the key groups
    srv->m_keyGroupSize = srvConfig.m_groupSize;

    // Set the proxies
    const auto& itSrvProxy = m_servicesProxies.find(srvConfig.m_uid);
    if(itSrvProxy != m_servicesProxies.end())
    {
        for(const auto& itProxy : itSrvProxy->second.m_proxyCnt)
        {
            srv->addProxyConnection(itProxy.second);
        }
    }

    // Configure
    srv->setConfiguration(srvConfig);
    srv->configure();

    return srv;
}

// ------------------------------------------------------------------------

void AppConfigManager2::createConnections()
{
    for(const auto& elem : m_cfgElem->getElements())
    {
        if (elem->getName() == "connect")
        {
            // Parse all connections
            auto genIdFn = [this]()
                           {
                               return "Proxy_" + this->getID() + "_" + std::to_string(m_proxyID++);
                           };

            ProxyConnections connectionInfos = ::fwServices::helper::Config::parseConnections2(elem, this->msgHead(),
                                                                                               genIdFn);
            // Proxy that is used for non-deferred connections
            ProxyConnections createdObjectsProxy(connectionInfos.m_channel);

            // Register signals
            for(const auto& signalInfo : connectionInfos.m_signals)
            {
                auto itDeferredObj = m_deferredObjects.find(signalInfo.first);
                if( itDeferredObj != m_deferredObjects.end() )
                {
                    // Deferred Object
                    ProxyConnections& proxy = itDeferredObj->second.m_proxyCnt[connectionInfos.m_channel];
                    proxy.addSignalConnection(signalInfo);
                }
                else
                {
                    auto itObj = m_createdObjects.find(signalInfo.first);
                    if( itObj != m_createdObjects.end() )
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
                if( itDeferredObj != m_deferredObjects.end() )
                {
                    // Deferred Object
                    ProxyConnections& proxy = itDeferredObj->second.m_proxyCnt[connectionInfos.m_channel];
                    proxy.addSlotConnection(slotInfo);
                }
                else
                {
                    auto itObj = m_createdObjects.find(slotInfo.first);
                    if( itObj != m_createdObjects.end() )
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

void AppConfigManager2::destroyProxy(const std::string& channel, const ProxyConnections& proxyCfg,
                                     const std::string& key, ::fwData::Object::csptr hintObj)
{
    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    for(const ProxyConnections::ProxyEltType& signalElt : proxyCfg.m_signals)
    {
        if(key.empty() || signalElt.first == key)
        {
            ::fwTools::Object::csptr obj = hintObj;
            if(obj == nullptr)
            {
                obj = ::fwTools::fwID::getObject(signalElt.first);
            }
            ::fwCom::HasSignals::csptr hasSignals = std::dynamic_pointer_cast< const ::fwCom::HasSignals >(obj);
            SLM_ASSERT(this->msgHead() + "Signal source not found '" + signalElt.first + "'", obj);

            ::fwCom::SignalBase::sptr sig = hasSignals->signal(signalElt.second);
            proxy->disconnect(channel, sig);
        }
    }
    for(const ProxyConnections::ProxyEltType& slotElt : proxyCfg.m_slots)
    {
        if(key.empty() || slotElt.first == key)
        {
            ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(slotElt.first);
            ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
            SLM_ASSERT(this->msgHead() + "Slot destination not found '" + slotElt.first + "'", hasSlots);

            ::fwCom::SlotBase::sptr slot = hasSlots->slot(slotElt.second);
            proxy->disconnect(channel, slot);
        }
    }
}

// ------------------------------------------------------------------------

void AppConfigManager2::destroyProxies()
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

void AppConfigManager2::addObjects(fwData::Object::sptr obj, const std::string& id)
{
    FW_PROFILE("addObjects");

    // Local map used to process services only once
    std::map< std::string, const ServiceConfig* > servicesMapCfg;

    // Local vector used to store services and keep the declare order (we could use only this one but the map is used
    // to speedup the search
    std::vector< const ServiceConfig* > servicesCfg;

    // Are there services that were waiting for this object ?
    auto itDeferredObj = m_deferredObjects.find(id);
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
        itDeferredObj->second.m_object = obj;

        for(const auto& connectCfg : itDeferredObj->second.m_proxyCnt)
        {
            this->connectProxy(connectCfg.first, connectCfg.second);
        }
    }

    std::unordered_map<std::string, ::fwServices::IService::sptr> newServices;

    for(const auto& itService : servicesCfg)
    {
        auto srvCfg = itService;
        SLM_ASSERT("Config is null", srvCfg);
        auto& uid = srvCfg->m_uid;

        bool createService = true;
        bool reconnect     = false;

        // Look for all objects (there could be more than the current object) and check if they are all created
        for(const auto& objCfg : srvCfg->m_objects)
        {
            // Look first in objects created in this appConfig
            if(m_createdObjects.find(objCfg.m_uid) == m_createdObjects.end())
            {
                // Not found, now look in the objects that were marked as "deferred"
                const auto itDeferredObj = m_deferredObjects.find(objCfg.m_uid);
                SLM_ASSERT( this->msgHead() + "Object '" + objCfg.m_uid + "' used by service '" + uid +
                            "' has not been declared in this AppConfig.",
                            itDeferredObj != m_deferredObjects.end());

                const auto object = itDeferredObj->second.m_object;
                if(object == nullptr)
                {
                    if(!objCfg.m_optional)
                    {
                        createService = false;

                        SLM_INFO( this->msgHead() + "Service '" + uid + "' has not been created because the "
                                  "object" + objCfg.m_uid + " is not available.");
                    }
                }
                else if(::fwTools::fwID::exist(uid))
                {
                    ::fwServices::IService::sptr srv = ::fwServices::get(uid);
                    SLM_ASSERT(this->msgHead() + "No service registered with UID \"" + uid + "\".", srv);

                    // We have an optional object
                    if( objCfg.m_optional && !::fwServices::OSR::isRegistered(objCfg.m_key, objCfg.m_access, srv))
                    {
                        // Register the key on the service
                        ::fwServices::OSR::registerService(object, objCfg.m_key, objCfg.m_access, srv);

                        // Call the swapping callback of the service and wait for it
                        srv->swapKey(objCfg.m_key, nullptr).wait();
                    }

                    createService = false;
                    reconnect     = true;
                }
            }
        }

        // All the data for this service is ready, create and run it
        if(createService)
        {
            // Create the service
            newServices.emplace(std::make_pair(uid, this->createService(*srvCfg)));

            // Debug message
            SLM_INFO( this->msgHead() + "Service '" + uid + "' has been automatically created because its "
                      "objects are all available.");
        }
        else if(reconnect)
        {
            // Update auto connections
            ::fwServices::IService::sptr srv = ::fwServices::get(uid);
            OSLM_ASSERT(this->msgHead() + "No service registered with UID \"" << uid << "\".", srv);

            srv->autoDisconnect();
            srv->autoConnect();
        }
    }

    std::vector< ::fwServices::IService::SharedFutureType > futures;

    // Start services according to the order given in the configuration
    for(const auto& uid : m_deferredStartSrv)
    {
        auto itSrv = newServices.find(uid);
        if( itSrv != newServices.end())
        {
            futures.push_back(itSrv->second->start());
            m_startedSrv.push_back(itSrv->second);

            // Debug message
            SLM_INFO( this->msgHead() + "Service '" + uid + "' has been automatically started because its "
                      "objects are all available.");
        }
    }

    ::boost::wait_for_all(futures.begin(), futures.end());
    futures.clear();

    // Update services according to the order given in the configuration
    for(const auto& uid : m_deferredUpdateSrv)
    {
        auto itSrv = newServices.find(uid);
        if( itSrv != newServices.end())
        {
            futures.push_back(itSrv->second->update());

            // Debug message
            SLM_INFO( this->msgHead() + "Service '" + uid + "' has been automatically update because its "
                      "objects are all available.");
        }
    }

    ::boost::wait_for_all(futures.begin(), futures.end());
}

//------------------------------------------------------------------------------

void AppConfigManager2::removeObjects(fwData::Object::sptr obj, const std::string& id)
{
    FW_PROFILE("removeObjects");

    // Are there services that were connected with this object ?
    const auto itDeferredObj = m_deferredObjects.find(id);
    if(itDeferredObj != m_deferredObjects.end())
    {
        for(const auto& itProxy : itDeferredObj->second.m_proxyCnt)
        {
            this->destroyProxy(itProxy.first, itProxy.second, id, itDeferredObj->second.m_object);
        }

        itDeferredObj->second.m_object.reset();

        // Are there services that were using this object ?
        for(const auto& srvCfg : itDeferredObj->second.m_servicesCfg)
        {
            if( ::fwTools::fwID::exist(srvCfg.m_uid) )
            {
                // Check all objects, to know if this object is optional
                bool optional = true;

                for(const auto& objCfg : srvCfg.m_objects)
                {
                    if(id == objCfg.m_uid)
                    {
                        ::fwServices::IService::sptr srv = ::fwServices::get(srvCfg.m_uid);
                        OSLM_ASSERT("No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                        if(objCfg.m_optional)
                        {
                            if(::fwServices::OSR::isRegistered(objCfg.m_key, objCfg.m_access, srv))
                            {
                                ::fwServices::OSR::unregisterService(objCfg.m_key, objCfg.m_access, srv);

                                srv->swapKey(objCfg.m_key, obj).wait();
                            }
                        }
                        else
                        {
                            optional = false;
                        }
                    }
                }

                if(!optional)
                {
                    // 1. Stop the service
                    ::fwServices::IService::sptr srv = ::fwServices::get(srvCfg.m_uid);
                    OSLM_ASSERT(this->msgHead() + "No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                    OSLM_ASSERT("Service " << srv->getID() << " already stopped.", !srv->isStopped());
                    srv->stop().wait();

                    for(auto it = m_startedSrv.begin(); it != m_startedSrv.end(); ++it)
                    {
                        if(it->lock() == srv)
                        {
                            m_startedSrv.erase(it);
                            break;
                        }
                    }

                    // 2. Destroy the service
                    OSLM_ASSERT("Service " << srv->getID() << " must be stopped before destruction.",
                                srv->isStopped());
                    ::fwServices::OSR::unregisterService(srv);

                    for(auto it = m_createdSrv.begin(); it != m_createdSrv.end(); ++it)
                    {
                        if(it->lock() == srv)
                        {
                            m_createdSrv.erase(it);
                            break;
                        }
                    }

                    ::fwServices::IService::wptr checkSrv = srv;
                    srv.reset();

                    SLM_ASSERT( this->msgHead() + "The service '" + srvCfg.m_uid +
                                "'' should have been destroyed, but someone "
                                "still holds a reference which prevents to destroy it properly.",
                                checkSrv.expired());

                    SLM_INFO( this->msgHead() + "Service '" + srvCfg.m_uid +
                              "' has been stopped because the object " +
                              id + " is no longer available.");
                }
                else
                {
                    // Update auto connections
                    ::fwServices::IService::sptr srv = ::fwServices::get(srvCfg.m_uid);
                    OSLM_ASSERT(this->msgHead() + "No service registered with UID \"" << srvCfg.m_uid << "\".", srv);

                    srv->autoDisconnect();
                    srv->autoConnect();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void AppConfigManager2::connectProxy(const std::string& channel, const ProxyConnections& connectCfg)
{
    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    for(const auto& signalCfg : connectCfg.m_signals)
    {
        ::fwTools::Object::sptr sigSource = ::fwTools::fwID::getObject(signalCfg.first);
        if(sigSource == nullptr)
        {
            // We didn't found the object or service globally, let's try with local deferred objects
            auto itDeferredObj = m_deferredObjects.find(signalCfg.first);
            SLM_ASSERT( this->msgHead() + "Object '" + signalCfg.first + "' not found.",
                        itDeferredObj != m_deferredObjects.end());

            sigSource = itDeferredObj->second.m_object;
        }
        ::fwCom::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< ::fwCom::HasSignals >(sigSource);
        SLM_ASSERT(this->msgHead() + "Signal source not found '" + signalCfg.first + "'", hasSignals);

        ::fwCom::SignalBase::sptr sig = hasSignals->signal(signalCfg.second);
        SLM_ASSERT("Signal '" + signalCfg.second + "' not found in source '" + signalCfg.first + "'.", sig);
        proxy->connect(channel, sig);
    }

    for(const auto& slotCfg : connectCfg.m_slots)
    {
        ::fwTools::Object::sptr slotDest = ::fwTools::fwID::getObject(slotCfg.first);
        ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(slotDest);
        SLM_ASSERT(this->msgHead() + "Slot destination not found '" + slotCfg.first + "'", hasSlots);

        ::fwCom::SlotBase::sptr slot = hasSlots->slot(slotCfg.second);
        SLM_ASSERT("Slot '" + slotCfg.second + "' not found in source '" + slotCfg.first + "'.", slot);
        proxy->connect(channel, slot);
    }
}

//------------------------------------------------------------------------------

std::string AppConfigManager2::getUIDListAsString(const std::vector<std::string>& uidList)
{
    std::string msg = uidList.size() == 1 ? " '" : "s '";
    msg += uidList[0];
    for(auto it = uidList.begin() + 1; it < uidList.end(); ++it)
    {
        msg += "', '" + *it;
    }
    msg = uidList.size() == 1 ? msg + "' is " : msg + "' are ";

    return msg;
}

//------------------------------------------------------------------------------

} // namespace fwServices
