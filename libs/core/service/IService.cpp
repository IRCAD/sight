/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "service/IService.hpp"

#include "service/registry/ObjectService.hpp"
#include "service/registry/Proxy.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/Convert.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/thread/Worker.hpp>

#include <functional>
#include <regex>

namespace sight::service
{

//-----------------------------------------------------------------------------

const core::com::Signals::SignalKeyType IService::s_STARTED_SIG = "started";
const core::com::Signals::SignalKeyType IService::s_UPDATED_SIG = "updated";
const core::com::Signals::SignalKeyType IService::s_SWAPPED_SIG = "swapped";
const core::com::Signals::SignalKeyType IService::s_STOPPED_SIG = "stopped";

const core::com::Signals::SignalKeyType IService::s_NOTIFIED_SIG = "notified";

const core::com::Slots::SlotKeyType IService::s_START_SLOT   = "start";
const core::com::Slots::SlotKeyType IService::s_STOP_SLOT    = "stop";
const core::com::Slots::SlotKeyType IService::s_UPDATE_SLOT  = "update";
const core::com::Slots::SlotKeyType IService::s_SWAP_SLOT    = "swap";
const core::com::Slots::SlotKeyType IService::s_SWAPKEY_SLOT = "swapKey";

//-----------------------------------------------------------------------------

IService::IService() :
    m_configuration(new core::runtime::EConfigurationElement("EmptyConfigurationElement"))
{
    newSignal<StartedSignalType>(s_STARTED_SIG);
    newSignal<UpdatedSignalType>(s_UPDATED_SIG);
    newSignal<SwappedSignalType>(s_SWAPPED_SIG);
    newSignal<StoppedSignalType>(s_STOPPED_SIG);

    newSignal<notification_signal_type>(s_NOTIFIED_SIG);

    m_slotStart   = newSlot(s_START_SLOT, &IService::_startSlot, this);
    m_slotStop    = newSlot(s_STOP_SLOT, &IService::_stopSlot, this);
    m_slotUpdate  = newSlot(s_UPDATE_SLOT, &IService::_updateSlot, this);
    m_slotSwapKey = newSlot(s_SWAPKEY_SLOT, &IService::_swapKeySlot, this);
}

//-----------------------------------------------------------------------------

IService::~IService()
{
    // check if the service manage outputs that are not maintained by someone else.
    if(!m_outputsMap.empty())
    {
        std::string objectKeys;
        for(const auto& obj : m_outputsMap)
        {
            const data::Object::wptr output = obj.second.get_shared();
            if(output.use_count() == 1)
            {
                if(!objectKeys.empty())
                {
                    objectKeys += ", ";
                }

                objectKeys += "'" + std::string(obj.first.first) + "'(nbRef: " + std::to_string(output.use_count())
                              + ")";
            }
        }

        SIGHT_WARN_IF(
            "Service " + this->getID() + " still contains registered outputs: " + objectKeys + ". They will no "
                                                                                               "longer be maintained. You should call setOutput(key, nullptr) before stopping the service to inform "
                                                                                               "AppManager and other services that the object will be destroyed.",
            !objectKeys.empty()
        );
    }
}

//-----------------------------------------------------------------------------

void IService::info(std::ostream& /*unused*/)
{
}

//-----------------------------------------------------------------------------

data::Object::csptr IService::getObject(
    std::string_view _key,
    data::Access _access,
    std::optional<std::size_t> _index
) const
{
    const auto key = std::make_pair(std::string(_key), _index);
    if(_access == data::Access::in)
    {
        auto it = m_inputsMap.find(key);
        if(it != m_inputsMap.end())
        {
            return it->second.getShared();
        }
    }
    else if(_access == data::Access::inout)
    {
        auto it = m_inOutsMap.find(key);
        if(it != m_inOutsMap.end())
        {
            return it->second.getShared();
        }
    }
    else
    {
        auto it = m_outputsMap.find(key);
        if(it != m_outputsMap.end())
        {
            return it->second.get_shared();
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void IService::setOutput(std::string_view key, data::Object::sptr object, std::optional<std::size_t> index)
{
    this->_setOutput(key, object, index);

    this->setPtrObject<data::Access::out>(key, object, index);
}

//-----------------------------------------------------------------------------

void IService::_setOutput(std::string_view key, data::Object::sptr object, std::optional<std::size_t> index)
{
    auto keyPair = std::make_pair(std::string(key), index);
    if(m_outputsMap.find(keyPair) != m_outputsMap.end())
    {
        service::OSR::unregisterServiceOutput(key, this->getSptr(), index);
    }

    if(object == nullptr)
    {
        m_outputsMap.erase(keyPair);
    }
    else
    {
        m_outputsMap[keyPair] = object;
        service::OSR::registerServiceOutput(object, key, this->getSptr(), index);
    }
}

//------------------------------------------------------------------------------

void IService::setInput(
    data::Object::csptr obj,
    std::string_view key,
    std::optional<std::size_t> index
)
{
    this->_setInput(obj, key, index);
}

//------------------------------------------------------------------------------

void IService::setInput(
    data::Object::csptr obj,
    std::string_view key,
    const bool autoConnect,
    const bool optional,
    std::optional<std::size_t> index
)
{
    this->_registerObject(key, data::Access::in, index, autoConnect, optional);
    this->_setInput(obj, key, index);
}

//------------------------------------------------------------------------------

void IService::_setInput(
    data::Object::csptr obj,
    std::string_view key,
    std::optional<std::size_t> index
)
{
    auto keyPair = std::make_pair(std::string(key), index);
    if(obj == nullptr)
    {
        this->setObjectId(key, "", index);
        m_inputsMap.erase(keyPair);
    }
    else
    {
        this->setObjectId(key, obj->getID(), index);
        m_inputsMap[keyPair] = obj; // For compatibility
    }

    this->setPtrObject<data::Access::in>(key, obj, index);
}

//------------------------------------------------------------------------------

void IService::setInOut(
    data::Object::sptr obj,
    std::string_view key,
    std::optional<std::size_t> index
)
{
    this->_setInOut(obj, key, index);
}

//------------------------------------------------------------------------------

void IService::setInOut(
    data::Object::sptr obj,
    std::string_view key,
    const bool autoConnect,
    const bool optional,
    std::optional<std::size_t> index
)
{
    this->_registerObject(key, data::Access::inout, index, autoConnect, optional);
    this->_setInOut(obj, key, index);
}

//------------------------------------------------------------------------------

void IService::_setInOut(
    data::Object::sptr obj,
    std::string_view key,
    std::optional<std::size_t> index
)
{
    auto keyPair = std::make_pair(std::string(key), index);
    if(obj == nullptr)
    {
        this->setObjectId(key, "", index);
        m_inOutsMap.erase(keyPair);
    }
    else
    {
        this->setObjectId(key, obj->getID(), index);
        m_inOutsMap[keyPair] = obj; // For compatibility
    }

    this->setPtrObject<data::Access::inout>(key, obj, index);
}

//------------------------------------------------------------------------------

void IService::setObject(
    data::Object::sptr obj,
    std::string_view key,
    std::optional<std::size_t> index,
    data::Access access,
    const bool autoConnect,
    const bool optional
)
{
    // TODO: Do the registration everytime for compatibility, but when everything works on the new data::ptr system,
    // we should switch to a different code that asserts if the key is not registered
    this->_registerObject(key, access, index, autoConnect, optional);

    const auto keypair = std::make_pair(std::string(key), index);

    if(access == data::Access::in)
    {
        this->setPtrObject<data::Access::in>(key, obj, index);
        m_inputsMap[keypair] = std::dynamic_pointer_cast<const data::Object>(obj);
    }
    else if(access == data::Access::inout)
    {
        this->setPtrObject<data::Access::inout>(key, obj, index);
        m_inOutsMap[keypair] = obj;
    }
    else if(access == data::Access::out)
    {
        this->setPtrObject<data::Access::out>(key, obj, index);
        m_outputsMap[keypair] = obj;
    }
}

//------------------------------------------------------------------------------

void IService::resetObject(
    std::string_view key,
    std::optional<std::size_t> index,
    data::Access access
)
{
    const auto keypair = std::make_pair(std::string(key), index);

    if(access == data::Access::in)
    {
        m_inputsMap.erase(keypair);
        this->setPtrObject<data::Access::in>(key, nullptr, index);
    }
    else if(access == data::Access::inout)
    {
        m_inOutsMap.erase(keypair);
        this->setPtrObject<data::Access::inout>(key, nullptr, index);
    }
    else
    {
        m_outputsMap.erase(keypair);
        this->setPtrObject<data::Access::out>(key, nullptr, index);
    }
}

//------------------------------------------------------------------------------

void IService::registerObject(
    const std::string& objId,
    std::string_view key,
    const data::Access access,
    const bool autoConnect,
    const bool optional,
    std::optional<std::size_t> index
)
{
    // TODO: remove this method, should be replaced by a correct usage of data::ptr + setObjectId() or its replacement
    this->_registerObject(key, access, index, autoConnect, optional);
    SIGHT_ASSERT("Object id must be defined", !objId.empty());
    this->setObjectId(key, objId, index);
}

//-----------------------------------------------------------------------------

bool IService::hasObjectId(std::string_view _key, std::optional<std::size_t> _index) const
{
    bool hasId = false;
    if(auto itr = m_serviceConfig.m_objects.find({std::string(_key), _index}); itr != m_serviceConfig.m_objects.end())
    {
        hasId = !itr->second.m_uid.empty();
    }

    return hasId;
}

//-----------------------------------------------------------------------------

IService::IdType IService::getObjectId(std::string_view _key, std::optional<std::size_t> _index) const
{
    auto keyItr = m_serviceConfig.m_objects.find({std::string(_key), _index});
    SIGHT_THROW_IF(
        "key '" << _key << "' is not registered for '" << this->getID() << "'.",
        keyItr == m_serviceConfig.m_objects.end()
    );
    SIGHT_THROW_IF(
        "Object key '" << _key << "' is not found for service '" << this->getID() << "'",
        keyItr->second.m_uid.empty()
    );
    return keyItr->second.m_uid;
}

//-----------------------------------------------------------------------------

void IService::setObjectId(std::string_view _key, const IService::IdType& _id, std::optional<std::size_t> _index)
{
    if(_index != std::nullopt)
    {
        if(auto keyItr = m_serviceConfig.m_objects.find({std::string(_key), _index});
           keyItr == m_serviceConfig.m_objects.end())
        {
            // Create a new group member from the group config
            m_serviceConfig.m_objects[{std::string(_key), _index}] = m_serviceConfig.m_groups[{std::string(_key)}];
        }
    }

    auto keyItr = m_serviceConfig.m_objects.find({std::string(_key), _index});
    SIGHT_THROW_IF(
        "key '" << _key << "' is not registered for '" << this->getID() << "'.",
        keyItr == m_serviceConfig.m_objects.end()
    );
    ObjectServiceConfig& cfg = keyItr->second;
    cfg.m_uid = _id;
}

//-----------------------------------------------------------------------------

void displayPt(boost::property_tree::ptree& pt, std::string indent = "")
{
    SIGHT_ERROR(indent << " data : '" << pt.data() << "'");

    for(boost::property_tree::ptree::value_type& v : pt)
    {
        SIGHT_ERROR((indent + "  '") << v.first << "':");
        displayPt(v.second, indent + "      ");
    }
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const core::runtime::ConfigurationElement::sptr _cfgElement)
{
    SIGHT_ASSERT("Invalid ConfigurationElement", _cfgElement);
    m_configuration      = _cfgElement;
    m_configurationState = UNCONFIGURED;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const Config& _configuration)
{
    SIGHT_ASSERT("Invalid ConfigurationElement", !_configuration.m_config.empty());

    // TODO: Remove this ugly const_cast
    m_configuration      = core::runtime::Convert::fromPropertyTree(_configuration.m_config);
    m_configurationState = UNCONFIGURED;

    m_serviceConfig.m_uid               = _configuration.m_uid;
    m_serviceConfig.m_type              = _configuration.m_type;
    m_serviceConfig.m_globalAutoConnect = _configuration.m_globalAutoConnect;
    m_serviceConfig.m_worker            = _configuration.m_worker;
    m_serviceConfig.m_config            = _configuration.m_config;

    // Only copy missing entries
    for(const auto& [key, objectCfg] : _configuration.m_objects)
    {
        if(auto it = m_serviceConfig.m_objects.find(key); it == m_serviceConfig.m_objects.end())
        {
            m_serviceConfig.m_objects[key] = objectCfg;
        }
    }
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const ConfigType& ptree)
{
    core::runtime::ConfigurationElement::sptr ce;

    ConfigType serviceConfig;
    serviceConfig.add_child("service", ptree);

    ce = core::runtime::Convert::fromPropertyTree(serviceConfig);

    SIGHT_ASSERT("Invalid ConfigurationElement", ce);

    this->setConfiguration(ce);
}

//-----------------------------------------------------------------------------

core::runtime::ConfigurationElement::sptr IService::getConfiguration() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

IService::ConfigType IService::getConfigTree() const
{
    const auto configTree = core::runtime::Convert::toPropertyTree(this->getConfiguration());

    auto srvConfig = configTree.get_child_optional("service");
    if(srvConfig.is_initialized())
    {
        return srvConfig.get();
    }

    return {};
}

//-----------------------------------------------------------------------------

void IService::configure()
{
    if(m_configurationState == UNCONFIGURED)
    {
        m_configurationState = CONFIGURING;
        if(m_globalState == STOPPED)
        {
            try
            {
                this->configuring();
            }
            catch(const boost::property_tree::ptree_bad_path& e)
            {
                SIGHT_ERROR("Error while configuring the service '" + this->getID() + "' : " + e.what());

                auto config = this->getConfigTree();
                SIGHT_ERROR("With the given configuration:\n" + core::runtime::property_tree::toString(config));
            }
            catch(std::exception& e)
            {
                SIGHT_ERROR("Error while configuring service '" + this->getID() + "' : " + e.what());
            }
        }
        else if(m_globalState == STARTED)
        {
            this->reconfiguring();
        }

        m_configurationState = CONFIGURED;
    }
}

//-----------------------------------------------------------------------------

void IService::configure(const ConfigType& ptree)
{
    core::runtime::ConfigurationElement::sptr ce;

    ConfigType serviceConfig;
    serviceConfig.add_child("service", ptree);

    ce = core::runtime::Convert::fromPropertyTree(serviceConfig);

    SIGHT_ASSERT("Invalid ConfigurationElement", ce);

    this->setConfiguration(ce);
    this->configure();
}

//-----------------------------------------------------------------------------

void IService::reconfiguring()
{
    SIGHT_FATAL(
        "If this method (reconfiguring) is called, it must be overridden in the implementation ("
        << this->getClassname() << ", " << this->getID() << ")"
    );
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::start()
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->_start(false);
    }

    return m_slotStart->asyncRun();
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stop()
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->_stop(false);
    }

    return m_slotStop->asyncRun();
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::update()
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->_update(false);
    }

    return m_slotUpdate->asyncRun();
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swapKey(std::string_view _key, data::Object::sptr _obj)
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->_swapKey(_key, _obj, false);
    }

    return m_slotSwapKey->asyncRun(_key, _obj);
}

//-----------------------------------------------------------------------------

IService::GlobalStatus IService::getStatus() const noexcept
{
    return m_globalState;
}

//-----------------------------------------------------------------------------

bool IService::isStarted() const noexcept
{
    return m_globalState == STARTED;
}

//-----------------------------------------------------------------------------

bool IService::isStopped() const noexcept
{
    return m_globalState == STOPPED;
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

void IService::setWorker(core::thread::Worker::sptr worker)
{
    m_associatedWorker = worker;
    core::com::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

core::thread::Worker::sptr IService::getWorker() const
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

IService::SharedFutureType IService::_startSlot()
{
    return this->_start(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::_start(bool _async)
{
    SIGHT_FATAL_IF("Service " << this->getID() << " already started", m_globalState != STOPPED);

    this->_connectToConfig();

    m_globalState = STARTING;

    PackagedTaskType task([this](auto&& ...){starting();});
    SharedFutureType future = task.get_future();
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Error while STARTING service '" + this->getID() + "' : " + e.what());
        SIGHT_ERROR("Service '" + this->getID() + "' is still STOPPED.");
        m_globalState = STOPPED;
        this->_disconnectFromConfig();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_globalState = STARTED;

    this->_autoConnect();

    auto sig = this->signal<StartedSignalType>(s_STARTED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::_stopSlot()
{
    return this->_stop(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::_stop(bool _async)
{
    SIGHT_FATAL_IF("Service " << this->getID() << " already stopped", m_globalState != STARTED);

    this->_autoDisconnect();

    PackagedTaskType task([this](auto&& ...){stopping();});
    SharedFutureType future = task.get_future();

    m_globalState = STOPPING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while STOPPING service '" + this->getID() + "' : " + e.what());
        SIGHT_ERROR("Service '" + this->getID() + "' is still STARTED.");
        m_globalState = STARTED;
        this->_autoConnect();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_globalState = STOPPED;

    auto sig = this->signal<StoppedSignalType>(s_STOPPED_SIG);
    sig->asyncEmit();

    this->_disconnectFromConfig();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::_swapKeySlot(std::string_view _key, data::Object::sptr _obj)
{
    return this->_swapKey(_key, _obj, true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::_swapKey(std::string_view _key, data::Object::sptr _obj, bool _async)
{
    SIGHT_FATAL_IF(
        "Service " << this->getID() << " is not STARTED, no swapping with Object "
        << (_obj ? _obj->getID() : "nullptr"),
        m_globalState != STARTED
    );

    auto fn = [this, _key]{swapping(_key);};
    PackagedTaskType task(fn);
    SharedFutureType future = task.get_future();

    this->_autoDisconnect();

    m_globalState = SWAPPING;
    task();
    m_globalState = STARTED;

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while SWAPPING service '" + this->getID() + "' : " + e.what());

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }

    this->_autoConnect();

    auto sig = this->signal<SwappedSignalType>(s_SWAPPED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::_updateSlot()
{
    return this->_update(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::_update(bool _async)
{
    if(m_globalState != STARTED)
    {
        SIGHT_WARN(
            "INVOKING update WHILE STOPPED (" << m_globalState << ") on service '" << this->getID()
            << "' of type '" << this->getClassname() << "': update is discarded."
        );
        return {};
    }

    SIGHT_ASSERT(
        "INVOKING update WHILE NOT IDLE (" << m_updatingState << ") on service '" << this->getID()
        << "' of type '" << this->getClassname() << "'",
        m_updatingState == NOTUPDATING
    );

    PackagedTaskType task([this](auto&& ...){updating();});
    SharedFutureType future = task.get_future();
    m_updatingState = UPDATING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while UPDATING service '" + this->getID() + "' : " + e.what());

        m_updatingState = NOTUPDATING;
        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with future.get()
            return future;
        }

        // Rethrow the same exception
        throw;
    }
    m_updatingState = NOTUPDATING;

    auto sig = this->signal<StartedSignalType>(s_UPDATED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

void IService::_connectToConfig()
{
    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signalCfg.first == this->getID());

            core::com::SignalBase::sptr sig = this->signal(signalCfg.second);
            SIGHT_ASSERT("Signal '" + signalCfg.second + "' not found in source '" + signalCfg.first + "'.", sig);
            try
            {
                proxy->connect(proxyCfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be connected to the"
                                                                                   " channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                                   )
                );
            }
        }

        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == this->getID());

            core::com::SlotBase::sptr slot = this->slot(slotCfg.second);
            SIGHT_ASSERT("Slot '" + slotCfg.second + "' not found in source '" + slotCfg.first + "'.", slot);

            try
            {
                proxy->connect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be connected to the "
                                                                             "channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::_autoConnect()
{
    service::IService::KeyConnectionsMap connectionMap = this->getAutoConnections();

    SIGHT_ERROR_IF(
        "The service '" + this->getID() + "'(" + this->getClassname()
        + ") is set to 'autoConnect=\"true\"' but is has no object to connect",
        m_serviceConfig.m_globalAutoConnect && m_serviceConfig.m_objects.empty()
    );

    for(const auto& [key, objectCfg] : m_serviceConfig.m_objects)
    {
        if(m_serviceConfig.m_globalAutoConnect || objectCfg.m_autoConnect)
        {
            service::IService::KeyConnectionsType connections;
            if(!connectionMap.empty())
            {
                auto it = connectionMap.find(objectCfg.m_key);
                if(it != connectionMap.end())
                {
                    connections = it->second;
                }
                else
                {
                    // Special case if we have a key from a group we check with the name of the group
                    std::smatch match;
                    static const std::regex reg("(.*)#[0-9]+");
                    if(std::regex_match(objectCfg.m_key, match, reg) && match.size() == 2)
                    {
                        const std::string group = match[1].str();
                        auto itConnection       = connectionMap.find(group);
                        if(itConnection != connectionMap.end())
                        {
                            connections = itConnection->second;
                        }
                    }
                }

                SIGHT_ERROR_IF(
                    "Object '" + objectCfg.m_key + "' of '" + this->getID() + "'(" + this->getClassname()
                    + ") is set to 'autoConnect=\"true\"' but there is no connection available.",
                    connections.empty() && objectCfg.m_autoConnect
                );
            }
            else
            {
                SIGHT_ERROR(
                    "Object '" + objectCfg.m_key + "' of '" + this->getID() + "'(" + this->getClassname()
                    + ") is set to 'autoConnect=\"true\"' but there is no connection available."
                );
            }

            data::Object::csptr obj;

            switch(objectCfg.m_access)
            {
                case data::Access::in:
                {
                    auto itObj = m_inputsMap.find(key);
                    if(itObj != m_inputsMap.end())
                    {
                        obj = itObj->second.getShared();
                    }

                    break;
                }

                case data::Access::inout:
                {
                    auto itObj = m_inOutsMap.find(key);
                    if(itObj != m_inOutsMap.end())
                    {
                        obj = itObj->second.getShared();
                    }

                    break;
                }

                case data::Access::out:
                {
                    SIGHT_WARN("Can't autoConnect to an output for now");
                    auto itObj = m_outputsMap.find(key);
                    if(itObj != m_outputsMap.end())
                    {
                        obj = itObj->second.get_shared();
                    }

                    break;
                }
            }

            SIGHT_ASSERT(
                "Object '" + objectCfg.m_key
                + "' has not been found when autoConnecting service '" + m_serviceConfig.m_uid + "'.",
                (!objectCfg.m_optional && obj) || objectCfg.m_optional
            );

            if(obj)
            {
                m_autoConnections.connect(obj, this->getSptr(), connections);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::_disconnectFromConfig()
{
    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signalCfg.first == this->getID());

            core::com::SignalBase::sptr sig = this->signal(signalCfg.second);

            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be disconnected "
                                                                                   "from the channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                                   )
                );
            }
        }

        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == this->getID());

            core::com::SlotBase::sptr slot = this->slot(slotCfg.second);
            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be disconnected from the "
                                                                             "channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::_autoDisconnect()
{
    m_autoConnections.disconnect();
}

//-----------------------------------------------------------------------------

void IService::_addProxyConnection(const helper::ProxyConnections& proxy)
{
    m_proxies[proxy.m_channel] = proxy;
}

//------------------------------------------------------------------------------

bool IService::hasAllRequiredObjects() const
{
    bool hasAllObjects = true;

    for(const auto& [key, objectCfg] : m_serviceConfig.m_objects)
    {
        if(!objectCfg.m_optional)
        {
            if(objectCfg.m_access == data::Access::in)
            {
                if(m_inputsMap.find(key) == m_inputsMap.end())
                {
                    SIGHT_DEBUG(
                        "The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + this->getID()
                        + "'"
                    );
                    hasAllObjects = false;
                    break;
                }
            }
            else if(objectCfg.m_access == data::Access::inout)
            {
                if(m_inOutsMap.find(key) == m_inOutsMap.end())
                {
                    SIGHT_DEBUG(
                        "The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + this->getID()
                        + "'"
                    );
                    hasAllObjects = false;
                    break;
                }
            }
        }
    }

    for(const auto& [key, objectCfg] : m_serviceConfig.m_groups)
    {
        if(!objectCfg.m_optional)
        {
            if(objectCfg.m_access == data::Access::in)
            {
                if(m_inputsMap.find({key, 0}) == m_inputsMap.end())
                {
                    SIGHT_DEBUG(
                        "The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + this->getID()
                        + "'"
                    );
                    hasAllObjects = false;
                    break;
                }
            }
            else if(objectCfg.m_access == data::Access::inout)
            {
                if(m_inOutsMap.find({key, 0}) == m_inOutsMap.end())
                {
                    SIGHT_DEBUG(
                        "The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + this->getID()
                        + "'"
                    );
                    hasAllObjects = false;
                    break;
                }
            }
        }
    }

    return hasAllObjects;
}

//------------------------------------------------------------------------------

void IService::notify(NotificationType type, const std::string& message) const
{
    const auto sig = this->signal<notification_signal_type>(s_NOTIFIED_SIG);
    sig->asyncEmit(type, message);
}

//------------------------------------------------------------------------------

std::optional<std::tuple<const std::string&, std::optional<std::size_t>,
                         const service::IService::ObjectServiceConfig&> > IService::_getObjInfoFromId(
    const std::string& objId
) const
{
    const auto idItr = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const auto& objCfg)
        {
            return objCfg.second.m_uid == objId;
        });

    return (idItr == m_serviceConfig.m_objects.end()) ? std::nullopt
                                                      : std::make_optional(
        std::make_tuple(
            std::cref(idItr->first.first),
            idItr->first.second,
            std::cref(idItr->second)
        )
                                                      );
}

//------------------------------------------------------------------------------

const service::IService::ObjectServiceConfig* IService::_getObjInfoFromKey(const std::string& objKey) const
{
    {
        auto idItr = std::find_if(
            m_serviceConfig.m_objects.begin(),
            m_serviceConfig.m_objects.end(),
            [&](const auto& objCfg)
            {
                return objCfg.second.m_key == objKey;
            });

        if(idItr != m_serviceConfig.m_objects.end())
        {
            return &idItr->second;
        }
    }
    {
        auto idItr = std::find_if(
            m_serviceConfig.m_groups.begin(),
            m_serviceConfig.m_groups.end(),
            [&](const auto& objCfg)
            {
                return objCfg.second.m_key == objKey;
            });

        return (idItr == m_serviceConfig.m_groups.end()) ? nullptr : &idItr->second;
    }
}

//-----------------------------------------------------------------------------

void IService::_registerObject(
    const std::string_view key,
    const data::Access access,
    std::optional<std::size_t> index,
    const bool autoConnect,
    const bool optional
)
{
    auto keyPair = std::make_pair(std::string(key), index);
    auto itr     = m_serviceConfig.m_objects.find(keyPair);
    if(itr == m_serviceConfig.m_objects.end())
    {
        ObjectServiceConfig objConfig;
        objConfig.m_key         = key;
        objConfig.m_access      = access;
        objConfig.m_autoConnect = autoConnect;
        objConfig.m_optional    = optional;

        if(index.has_value())
        {
            [[maybe_unused]] const ObjectServiceConfig& groupConfig = m_serviceConfig.m_groups[std::string(key)];

            SIGHT_ASSERT(
                "Access type of '" << key << "'  does not match previous registration",
                access == groupConfig.m_access
            );
        }

        m_serviceConfig.m_objects[keyPair] = objConfig;
    }
    else
    {
        ObjectServiceConfig& objConfig = itr->second;

        SIGHT_ASSERT(
            "[" << this->getID() << "] " << "access type of '" << key << "'  does not match previous registration",
            access == objConfig.m_access
        );

        objConfig.m_autoConnect = autoConnect;
        objConfig.m_optional    = optional;
    }
}

//-----------------------------------------------------------------------------

void IService::_registerObjectGroup(
    std::string_view key,
    const data::Access access,
    const bool autoConnect,
    const bool optional
)
{
    ObjectServiceConfig objConfig;
    objConfig.m_key         = key;
    objConfig.m_access      = access;
    objConfig.m_autoConnect = autoConnect;
    objConfig.m_optional    = optional;

    m_serviceConfig.m_groups[std::string(key)] = objConfig;
}

//-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see IService::operator<<(std::ostream & _ostream, IService& _service)
 * @note Invoke IService::info( std::ostream )
 */
std::ostream& operator<<(std::ostream& _ostream, IService& _service)
{
    _service.info(_ostream);
    return _ostream;
}

//-----------------------------------------------------------------------------

} // namespace sight::service
