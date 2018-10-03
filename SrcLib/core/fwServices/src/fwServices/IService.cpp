/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IService.hpp"

#include "fwServices/registry/ActiveWorkers.hpp"
#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/registry/Proxy.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwRuntime/Convert.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwThread/Worker.hpp>

#include <fwTools/fwID.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/regex.hpp>

#include <functional>

namespace fwServices
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType IService::s_STARTED_SIG = "started";
const ::fwCom::Slots::SlotKeyType IService::s_UPDATED_SIG = "updated";
const ::fwCom::Slots::SlotKeyType IService::s_STOPPED_SIG = "stopped";

const ::fwCom::Slots::SlotKeyType IService::s_START_SLOT   = "start";
const ::fwCom::Slots::SlotKeyType IService::s_STOP_SLOT    = "stop";
const ::fwCom::Slots::SlotKeyType IService::s_UPDATE_SLOT  = "update";
const ::fwCom::Slots::SlotKeyType IService::s_SWAP_SLOT    = "swap";
const ::fwCom::Slots::SlotKeyType IService::s_SWAPKEY_SLOT = "swapKey";

const std::string IService::s_DEFAULT_OBJECT = "defaultObject";

//-----------------------------------------------------------------------------

IService::IService() :
    m_configuration( new ::fwRuntime::EConfigurationElement("EmptyConfigurationElement") ),
    m_globalState( STOPPED ),
    m_updatingState( NOTUPDATING ),
    m_configurationState( UNCONFIGURED )
{
    newSignal<StartedSignalType>( s_STARTED_SIG );
    newSignal<UpdatedSignalType>( s_UPDATED_SIG );
    newSignal<StoppedSignalType>( s_STOPPED_SIG );

    m_slotStart   = newSlot( s_START_SLOT, &IService::startSlot, this );
    m_slotStop    = newSlot( s_STOP_SLOT, &IService::stopSlot, this );
    m_slotUpdate  = newSlot( s_UPDATE_SLOT, &IService::updateSlot, this );
    m_slotSwapKey = newSlot( s_SWAPKEY_SLOT, &IService::swapKeySlot, this );

#ifndef REMOVE_DEPRECATED
    m_slotSwap = newSlot( s_SWAP_SLOT, &IService::swapSlot, this );
#endif
}

//-----------------------------------------------------------------------------

IService::~IService()
{
}

//-----------------------------------------------------------------------------

void IService::info( std::ostream& )
{
}

//-----------------------------------------------------------------------------

void IService::setOutput(const IService::KeyType& key, const fwData::Object::sptr& object, size_t index)
{
    std::string outKey = key;

    if(m_keyGroupSize.find(key) != m_keyGroupSize.end())
    {
        outKey = KEY_GROUP_NAME(key, index);
    }
    if(::fwServices::OSR::isRegistered(outKey, ::fwServices::IService::AccessType::OUTPUT, this->getSptr()))
    {
        ::fwServices::OSR::unregisterServiceOutput(outKey, this->getSptr());
    }

    if(object != nullptr)
    {
        ::fwServices::OSR::registerServiceOutput(object, outKey, this->getSptr());
    }
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr IService::getObject()
{
    FW_DEPRECATED("getObject()", "getInput() or getInOut()", "20.0");

    // Handle compatibility with new behavior
    if(m_associatedObject.expired())
    {
        // If we have an appXml but with an old service definition, we consider that the old primary object is the
        // first one in the map
        if(!m_inputsMap.empty())
        {
            return ::fwData::Object::constCast(m_inputsMap.begin()->second.lock());
        }
        else if(!m_inOutsMap.empty())
        {
            return m_inOutsMap.begin()->second.lock();
        }
        else if(!m_outputsMap.empty())
        {
            return m_outputsMap.begin()->second;
        }
        else
        {
            OSLM_ASSERT("Associated Object of " <<this->getID()<<" ["<<this->getClassname()<<"] is expired", false );
        }
    }
    return m_associatedObject.lock();
}

//------------------------------------------------------------------------------

void IService::registerInput(const ::fwData::Object::csptr& obj, const std::string& key, const bool autoConnect,
                             const bool optional)
{
    this->registerObject(obj->getID(), key, AccessType::INPUT, autoConnect, optional);

    ::fwServices::OSR::registerServiceInput(obj, key, this->getSptr());
}

//------------------------------------------------------------------------------

void IService::unregisterInput(const std::string& key)
{
    this->unregisterObject(key, AccessType::INPUT);
}

//------------------------------------------------------------------------------

void IService::registerInOut(const ::fwData::Object::sptr& obj, const std::string& key, const bool autoConnect,
                             const bool optional)
{
    this->registerObject(obj, key, AccessType::INOUT, autoConnect, optional);
}

//------------------------------------------------------------------------------

void IService::unregisterInOut(const std::string& key)
{
    this->unregisterObject(key, AccessType::INOUT);
}

//------------------------------------------------------------------------------

void IService::registerObject(const ::fwData::Object::sptr& obj, const std::string& key,
                              AccessType access, const bool autoConnect, const bool optional)
{
    this->registerObject(key, access, autoConnect, optional);

    if (access == AccessType::INPUT)
    {
        m_inputsMap[key] = obj;
    }
    else if (access == AccessType::INOUT)
    {
        m_inOutsMap[key] = obj;
    }
    else if (access == AccessType::OUTPUT)
    {
        m_outputsMap[key] = obj;
    }

    ::fwServices::OSR::registerService(obj, key, access, this->getSptr());
}

//------------------------------------------------------------------------------

void IService::registerObject(const std::string& objId,
                              const ::fwServices::IService::KeyType& key,
                              const ::fwServices::IService::AccessType access,
                              const bool autoConnect, const bool optional)
{
    this->registerObject(key, access, autoConnect, optional);
    SLM_ASSERT("Object id must be defined", !objId.empty());
    this->setObjectId(key, objId);
}

//------------------------------------------------------------------------------

void IService::unregisterObject(const std::string& key, AccessType access)
{
    ::fwServices::OSR::unregisterService(key, access, this->getSptr());

    if(access == ::fwServices::IService::AccessType::INPUT)
    {
        m_inputsMap.erase(key);
    }
    else if(access == ::fwServices::IService::AccessType::INOUT)
    {
        m_inOutsMap.erase(key);
    }
    else
    {
        m_outputsMap.erase(key);
    }
}
//------------------------------------------------------------------------------

void IService::unregisterObject(const std::string& objId)
{
    auto keyItr = std::find_if( m_idsMap.begin(),  m_idsMap.end(),
                                [&](const std::map<KeyType, IdType>::value_type& id)
        {
            return (id.second == objId);
        });

    if (keyItr == m_idsMap.end())
    {
        SLM_ERROR("object '" + objId + "' is not registered");
        return;
    }
    const std::string key = keyItr->first;

    auto newItr = std::find_if( m_serviceConfig.m_objects.begin(),  m_serviceConfig.m_objects.end(),
                                [&](const ObjectServiceConfig& config)
        {
            return (config.m_key == key);
        });

    m_serviceConfig.m_objects.erase(newItr);
    auto it = m_idsMap.find(objId);
    m_idsMap.erase(it);
}

//-----------------------------------------------------------------------------

bool IService::hasObjectId(const KeyType& _key) const
{
    auto it                = m_idsMap.find(_key);
    const bool hasObjectId = (it != m_idsMap.end());
    return (it != m_idsMap.end());
}

//-----------------------------------------------------------------------------

IService::IdType IService::getObjectId(const IService::KeyType& _key) const
{
    auto it = m_idsMap.find(_key);
    FW_RAISE_IF("Object key '" + _key + "' not found in service " + this->getID() + ".", it == m_idsMap.end());
    return it->second;
}

//-----------------------------------------------------------------------------

void IService::setObjectId(const IService::KeyType& _key, const IService::IdType& _id)
{
    m_idsMap[_key] = _id;
}

//-----------------------------------------------------------------------------

void displayPt(::boost::property_tree::ptree& pt, std::string indent = "")
{
    OSLM_ERROR(indent << " data : '" << pt.data() << "'" );

    for( ::boost::property_tree::ptree::value_type& v :  pt)
    {
        OSLM_ERROR((indent + "  '") << v.first << "':" );
        displayPt(v.second, indent + "      ");

    }
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const ::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    SLM_ASSERT( "Invalid ConfigurationElement", _cfgElement );
    m_configuration      = _cfgElement;
    m_configurationState = UNCONFIGURED;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const Config& _configuration)
{
    SLM_ASSERT( "Invalid ConfigurationElement", _configuration.m_config );

    // TODO: Remove this ugly const_cast
    m_configuration      = ::fwRuntime::ConfigurationElement::constCast(_configuration.m_config);
    m_configurationState = UNCONFIGURED;

    m_serviceConfig = _configuration;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration( const ConfigType& ptree )
{
    ::fwRuntime::ConfigurationElement::sptr ce;

    ConfigType serviceConfig;
    serviceConfig.add_child("service", ptree);

    ce = ::fwRuntime::Convert::fromPropertyTree(serviceConfig);

    SLM_ASSERT( "Invalid ConfigurationElement", ce );

    this->setConfiguration(ce);
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr IService::getConfiguration() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

IService::ConfigType IService::getConfigTree() const
{
    const auto configTree = ::fwRuntime::Convert::toPropertyTree(this->getConfiguration());

    // This is in case we get the configuration from a ::fwServices::registry::ServiceConfig
    auto srvConfig = configTree.get_child_optional("config");

    if(srvConfig.is_initialized())
    {
        return srvConfig.value();
    }
    else
    {
        srvConfig = configTree.get_child_optional("service");
        if(srvConfig.is_initialized())
        {
            return srvConfig.value();
        }
        return IService::ConfigType();
    }
}

//-----------------------------------------------------------------------------

void IService::configure()
{
    if( m_configurationState == UNCONFIGURED )
    {
        m_configurationState = CONFIGURING;
        if( m_globalState == STOPPED )
        {
            try
            {
                this->configuring();
            }
            catch (std::exception& e)
            {
                SLM_ERROR("Error while configuring service '" + this->getID() + "' : " + e.what());
            }
        }
        else if( m_globalState == STARTED )
        {
            this->reconfiguring();
        }
        m_configurationState = CONFIGURED;
    }
}

//-----------------------------------------------------------------------------

void IService::configure(const ConfigType& ptree)
{
    ::fwRuntime::ConfigurationElement::sptr ce;

    ConfigType serviceConfig;
    serviceConfig.add_child("service", ptree);

    ce = ::fwRuntime::Convert::fromPropertyTree(serviceConfig);

    SLM_ASSERT( "Invalid ConfigurationElement", ce );

    this->setConfiguration(ce);
    this->configure();
}

//-----------------------------------------------------------------------------

void IService::reconfiguring()
{
    OSLM_FATAL(
        "If this method (reconfiguring) is called, it must be overriden in the implementation ("
            << this->getClassname() <<", "<< this->getID() << ")" );
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::start()
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        return this->internalStart(false);
    }
    else
    {
        return m_slotStart->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stop()
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        return this->internalStop(false);
    }
    else
    {
        return m_slotStop->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::update()
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        return this->internalUpdate(false);
    }
    else
    {
        return m_slotUpdate->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swapKey(const IService::KeyType& _key, fwData::Object::sptr _obj)
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        return this->internalSwapKey(_key, _obj, false);
    }
    else
    {
        return m_slotSwapKey->asyncRun( _key, _obj );
    }
}

//-----------------------------------------------------------------------------

IService::GlobalStatus IService::getStatus() const noexcept
{
    return m_globalState;
}

//-----------------------------------------------------------------------------

bool IService::isStarted() const noexcept
{
    return (m_globalState == STARTED);
}

//-----------------------------------------------------------------------------

bool IService::isStopped() const noexcept
{
    return (m_globalState == STOPPED);
}

//-----------------------------------------------------------------------------

IService::ConfigurationStatus IService::getConfigurationStatus() const noexcept
{
    return m_configurationState;
}

//-----------------------------------------------------------------------------

IService::UpdatingStatus IService::getUpdatingStatus() const noexcept
{
    return m_updatingState;
}

//-----------------------------------------------------------------------------

void IService::setWorker( ::fwThread::Worker::sptr worker )
{
    m_associatedWorker = worker;
    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//-----------------------------------------------------------------------------

::fwThread::Worker::sptr IService::getWorker() const
{
    return m_associatedWorker;
}

//-----------------------------------------------------------------------------

IService::KeyConnectionsMap IService::getAutoConnections() const
{
    KeyConnectionsMap connections;
    return connections;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::startSlot()
{
    return this->internalStart(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalStart(bool _async)
{
    OSLM_FATAL_IF("Service "<<this->getID()<<" already started", m_globalState != STOPPED);

    this->connectToConfig();

    m_globalState = STARTING;

    PackagedTaskType task( std::bind(&IService::starting, this) );
    SharedFutureType future = task.get_future();
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch (const std::exception& e)
    {
        SLM_ERROR("Error while STARTING service '" + this->getID() + "' : " + e.what());
        SLM_ERROR("Service '" + this->getID() + "' is still STOPPED.");
        m_globalState = STOPPED;
        this->disconnectFromConfig();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }

    }
    m_globalState = STARTED;

    this->autoConnect();

    auto sig = this->signal<StartedSignalType>(s_STARTED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stopSlot()
{
    return this->internalStop(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalStop(bool _async)
{
    OSLM_FATAL_IF("Service "<<this->getID()<<" already stopped", m_globalState != STARTED);

    this->autoDisconnect();

    PackagedTaskType task( std::bind(&IService::stopping, this) );
    SharedFutureType future = task.get_future();

    m_globalState = STOPPING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch (std::exception& e)
    {
        SLM_ERROR("Error while STOPPING service '" + this->getID() + "' : " + e.what());
        SLM_ERROR("Service '" + this->getID() + "' is still STARTED.");
        m_globalState = STARTED;
        this->autoConnect();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }
    m_globalState = STOPPED;

    auto sig = this->signal<StoppedSignalType>(s_STOPPED_SIG);
    sig->asyncEmit();

    this->disconnectFromConfig();

    return future;

}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swapKeySlot(const KeyType& _key, ::fwData::Object::sptr _obj)
{
    return this->internalSwapKey(_key, _obj, true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalSwapKey(const KeyType& _key, ::fwData::Object::sptr _obj, bool _async)
{
    OSLM_FATAL_IF("Service "<< this->getID() << " is not STARTED, no swapping with Object " <<
                  (_obj ? _obj->getID() : "nullptr"),
                  m_globalState != STARTED);

    auto fn = std::bind(static_cast<void (IService::*)(const KeyType&)>(&IService::swapping), this, _key);
    PackagedTaskType task( fn );
    SharedFutureType future = task.get_future();

    this->autoDisconnect();

    m_globalState = SWAPPING;
    task();
    m_globalState = STARTED;

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch (std::exception& e)
    {
        SLM_ERROR("Error while SWAPPING service '" + this->getID() + "' : " + e.what());

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }

    this->autoConnect();

    return future;

}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::updateSlot()
{
    return this->internalUpdate(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalUpdate(bool _async)
{
    if(m_globalState != STARTED)
    {
        OSLM_WARN("INVOKING update WHILE STOPPED ("<<m_globalState<<") on service '" << this->getID() <<
                  "' of type '" << this->getClassname() << "': update is discarded." );
        return SharedFutureType();
    }
    OSLM_ASSERT("INVOKING update WHILE NOT IDLE ("<<m_updatingState<<") on service '" << this->getID() <<
                "' of type '" << this->getClassname() << "'", m_updatingState == NOTUPDATING );

    PackagedTaskType task( std::bind(&IService::updating, this) );
    SharedFutureType future = task.get_future();
    m_updatingState = UPDATING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch (std::exception& e)
    {
        SLM_ERROR("Error while UPDATING service '" + this->getID() + "' : " + e.what());

        m_updatingState = NOTUPDATING;
        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }
    m_updatingState = NOTUPDATING;

    auto sig = this->signal<StartedSignalType>(s_UPDATED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

void IService::connectToConfig()
{
    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SLM_ASSERT("Invalid signal source", signalCfg.first == this->getID());

            ::fwCom::SignalBase::sptr sig = this->signal(signalCfg.second);
            SLM_ASSERT("Signal '" + signalCfg.second + "' not found in source '" + signalCfg.first + "'.", sig);
            try
            {
                proxy->connect(proxyCfg.second.m_channel, sig);

            }
            catch (const std::exception& e)
            {
                SLM_ERROR("Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be connected to the"
                          " channel '" + proxyCfg.second.m_channel + "': " + std::string(e.what()));
            }
        }

        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SLM_ASSERT("Invalid slot destination", slotCfg.first == this->getID());

            ::fwCom::SlotBase::sptr slot = this->slot(slotCfg.second);
            SLM_ASSERT("Slot '" + slotCfg.second + "' not found in source '" + slotCfg.first + "'.", slot);

            try
            {
                proxy->connect(proxyCfg.second.m_channel, slot);
            }
            catch (const std::exception& e)
            {
                SLM_ERROR("Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be connected to the "
                          "channel '" + proxyCfg.second.m_channel + "': " + std::string(e.what()));
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::autoConnect()
{
    ::fwServices::IService::KeyConnectionsMap connectionMap = this->getAutoConnections();

    SLM_ERROR_IF("The service '" + this->getID() + "'(" + this->getClassname() +
                 ") is set to 'autoConnect=\"yes\"' but is has no object to connect",
                 m_serviceConfig.m_globalAutoConnect && m_serviceConfig.m_objects.empty());

    // For compatibility with V1, we allow services to connect explicitly with the default object
    // For these services we will ignore all auto connections with any other data
    // This is intended notably for managers-like services
    const bool hasDefaultObjectConnectionV1 =
        (connectionMap.find(::fwServices::IService::s_DEFAULT_OBJECT) != connectionMap.end());

    for(const auto& objectCfg : m_serviceConfig.m_objects)
    {
        if ((m_serviceConfig.m_globalAutoConnect || objectCfg.m_autoConnect) && !hasDefaultObjectConnectionV1)
        {
            ::fwServices::IService::KeyConnectionsType connections;
            if(!connectionMap.empty())
            {
                auto it = connectionMap.find(objectCfg.m_key);
                if( it != connectionMap.end())
                {
                    connections = it->second;
                }
                else
                {
                    // Special case if we have a key from a group we check with the name of the group
                    boost::smatch match;
                    static const ::boost::regex reg("(.*)#[0-9]+");
                    if( ::boost::regex_match(objectCfg.m_key, match, reg ) && match.size() == 2)
                    {
                        const std::string group = match[1].str();
                        auto it                 = connectionMap.find(group);
                        if( it != connectionMap.end())
                        {
                            connections = it->second;
                        }
                    }
                }
                SLM_ERROR_IF("Object '" + objectCfg.m_key + "' of '" + this->getID() + "'(" + this->getClassname() +
                             ") is set to 'autoConnect=\"yes\"' but there is no connection available.",
                             connections.empty() && objectCfg.m_autoConnect);
            }
            else
            {
                // V1 compatibility, we didn't implemented the new function, so we stick to the old behavior
                // This also allows to get the default connection with the s_UPDATE_SLOT. When we remove this
                // function, we will have to implement this behavior with getAutoConnections()
                connections = this->getObjSrvConnections();
                FW_DEPRECATED_IF("getObjSrvConnections()", "getAutoConnections()", "20.0", !connections.empty());
            }

            ::fwData::Object::csptr obj;

            switch(objectCfg.m_access)
            {
                case AccessType::INPUT:
                {
                    auto itObj = m_inputsMap.find(objectCfg.m_key);
                    if(itObj != m_inputsMap.end())
                    {
                        obj = itObj->second.lock();
                    }
                    break;
                }
                case AccessType::INOUT:
                {
                    auto itObj = m_inOutsMap.find(objectCfg.m_key);
                    if(itObj != m_inOutsMap.end())
                    {
                        obj = itObj->second.lock();
                    }
                    break;
                }
                case AccessType::OUTPUT:
                {
                    SLM_WARN("Can't autoConnect to an output for now");
                    auto itObj = m_outputsMap.find(objectCfg.m_key);
                    if(itObj != m_outputsMap.end())
                    {
                        obj = itObj->second;
                    }
                    break;
                }
            }

            SLM_ASSERT("Object '" + objectCfg.m_uid +
                       "' has not been found when autoConnecting service '" + m_serviceConfig.m_uid + "'.",
                       (!objectCfg.m_optional && obj) || objectCfg.m_optional);

            if(obj)
            {
                m_autoConnections.connect( obj, this->getSptr(), connections );
            }
        }
    }

    // Autoconnect with the default object - to be cleaned when V1 compatibility is over
    auto defaultObj = this->getInOut< ::fwData::Object >(s_DEFAULT_OBJECT);

    if(m_serviceConfig.m_globalAutoConnect && defaultObj)
    {
        ::fwServices::IService::KeyConnectionsType connections;
        auto it = connectionMap.find(::fwServices::IService::s_DEFAULT_OBJECT);
        if( it != connectionMap.end())
        {
            connections = it->second;
        }
        else if(m_serviceConfig.m_objects.size() == 0)
        {
            // Only use the old callback automatically in case we put a composite as the only data
            connections = this->getObjSrvConnections();
        }

        m_autoConnections.connect( defaultObj, this->getSptr(), connections );
    }
}

//-----------------------------------------------------------------------------

void IService::disconnectFromConfig()
{
    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SLM_ASSERT("Invalid signal source", signalCfg.first == this->getID());

            ::fwCom::SignalBase::sptr sig = this->signal(signalCfg.second);

            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, sig);
            }
            catch (const std::exception& e)
            {
                SLM_ERROR("Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be disconnected "
                          "from the channel '" + proxyCfg.second.m_channel + "': " + std::string(e.what()));
            }
        }
        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SLM_ASSERT("Invalid slot destination", slotCfg.first == this->getID());

            ::fwCom::SlotBase::sptr slot = this->slot(slotCfg.second);
            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, slot);
            }
            catch (const std::exception& e)
            {
                SLM_ERROR("Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be disconnected from the "
                          "channel '" + proxyCfg.second.m_channel + "': " + std::string(e.what()));
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::autoDisconnect()
{
    m_autoConnections.disconnect();
}

//-----------------------------------------------------------------------------

void IService::addProxyConnection(const helper::ProxyConnections& proxy)
{
    m_proxies[proxy.m_channel] = proxy;
}

//-----------------------------------------------------------------------------

bool IService::isObjectRequired(const std::string& objId) const
{
    auto itr = std::find_if( m_idsMap.begin(),  m_idsMap.end(),
                             [&](const std::pair<KeyType, IdType>& id)
        {
            return (id.second == objId);
        });

    return (itr != m_idsMap.end());
}

//------------------------------------------------------------------------------

bool IService::hasAllRequiredObjects() const
{

    bool hasAllObjects = true;

    for (const auto& objectCfg : m_serviceConfig.m_objects)
    {
        if (objectCfg.m_optional == false)
        {
            if (objectCfg.m_access == ::fwServices::IService::AccessType::INPUT)
            {
                if (nullptr == this->getInput< ::fwData::Object >(objectCfg.m_key))
                {
                    SLM_DEBUG("The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + srv->getID()
                              + "'");
                    hasAllObjects = false;
                    break;
                }
            }
            else if (objectCfg.m_access == ::fwServices::IService::AccessType::INOUT)
            {
                if (nullptr == this->getInOut< ::fwData::Object >(objectCfg.m_key))
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

const IService::ObjectServiceConfig& IService::getObjInfo(const std::string& objId) const
{
    auto keyItr = std::find_if( m_idsMap.begin(),  m_idsMap.end(),
                                [&](const std::pair<KeyType, IdType>& id)
        {
            return (id.second == objId);
        });

    SLM_ASSERT("object '" + objId + "' is not registered for '" + this->getID() + "'.", keyItr != m_idsMap.end());
    const std::string key = keyItr->first;

    auto itr = std::find_if( m_serviceConfig.m_objects.begin(),  m_serviceConfig.m_objects.end(),
                             [&](const ObjectServiceConfig& config)
        {
            return (config.m_key == key);
        });
    SLM_ASSERT("Object '" + objId + "' is not registered '" + this->getID() + "'.",
               itr != m_serviceConfig.m_objects.end());

    return *itr;
}

//-----------------------------------------------------------------------------

void IService::registerObject(const ::fwServices::IService::KeyType& key,
                              const ::fwServices::IService::AccessType access,
                              const bool autoConnect, const bool optional)
{
    auto itr = std::find_if( m_serviceConfig.m_objects.begin(), m_serviceConfig.m_objects.end(),
                             [&](const ObjectServiceConfig& objInfo)
        {

            return (objInfo.m_key == key);
        });
    if (itr == m_serviceConfig.m_objects.end())
    {
        ObjectServiceConfig objConfig;
        objConfig.m_key         = key;
        objConfig.m_access      = access;
        objConfig.m_autoConnect = autoConnect;
        objConfig.m_optional    = optional;

        m_serviceConfig.m_objects.push_back(objConfig);
    }
}

//-----------------------------------------------------------------------------

void IService::registerObjectGroup(const std::string& key, AccessType access, const std::uint8_t minNbObject,
                                   const bool autoConnect, const std::uint8_t maxNbObject)
{
    for (std::uint8_t i = 0; i < maxNbObject; ++i)
    {
        const bool optional = (i < minNbObject ? false : true);
        ObjectServiceConfig objConfig;
        objConfig.m_key         = KEY_GROUP_NAME(key, i);
        objConfig.m_access      = access;
        objConfig.m_autoConnect = autoConnect;
        objConfig.m_optional    = optional;

        m_serviceConfig.m_objects.push_back(objConfig);
    }
    m_keyGroupSize[key] = minNbObject;
}

//-----------------------------------------------------------------------------

IService::KeyConnectionsType IService::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    return connections;
}

//-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see IService::operator<<(std::ostream & _ostream, IService& _service)
 * @note Invoke IService::info( std::ostream )
 */
std::ostream& operator<<(std::ostream& _ostream, IService& _service)
{
    _service.info( _ostream );
    return _ostream;
}

//-----------------------------------------------------------------------------

#ifndef REMOVE_DEPRECATED
IService::SharedFutureType IService::swap( ::fwData::Object::sptr _obj )
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        return this->internalSwap(_obj, false);
    }
    else
    {
        return m_slotSwap->asyncRun( _obj );
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swapSlot(::fwData::Object::sptr _obj)
{
    return this->internalSwap(_obj, true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalSwap(::fwData::Object::sptr _obj, bool _async)
{
    OSLM_ASSERT("Swapping on "<< this->getID() << " with same Object " << _obj->getID(),
                m_associatedObject.lock() != _obj );
    OSLM_FATAL_IF("Service "<< this->getID() << " is not STARTED, no swapping with Object " << _obj->getID(),
                  m_globalState != STARTED);

    PackagedTaskType task( std::bind(static_cast<void (IService::*)()>(&IService::swapping), this) );
    SharedFutureType future = task.get_future();

    m_globalState = SWAPPING;
    ::fwServices::OSR::swapService( _obj, this->getSptr() );
    task();
    m_globalState = STARTED;

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch (std::exception& e)
    {
        SLM_ERROR("Error while SWAPPING service '" + this->getID() + "' : " + e.what());

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }

    return future;
}
#endif

}
