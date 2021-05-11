/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "service/extension/Config.hpp"
#include "service/helper/Config.hpp"

#include "service/registry/Proxy.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>
#include <core/com/helper/SigSlotConnection.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/Convert.hpp>
#include <core/runtime/helper.hpp>
#include <core/tools/Object.hpp>

#include <data/Object.hpp>

#include <array>
#include <regex>
#include <string>
#include <vector>

namespace sight::service
{

namespace helper
{

/// container for the data keywords for a service configuration
const std::array< std::string, 3 > s_DATA_KEYWORDS = {{ "in", "out", "inout" }};

//-----------------------------------------------------------------------------

void Config::createConnections( const core::runtime::ConfigurationElement::csptr& connectionCfg,
                                core::com::helper::SigSlotConnection& connections,
                                const CSPTR(core::tools::Object)& obj)
{
    ConnectionInfo info = parseConnections(connectionCfg, obj);

    core::tools::Object::sptr sigSource    = core::tools::fwID::getObject(info.m_signal.first);
    core::com::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< core::com::HasSignals >(sigSource);

    SIGHT_ASSERT("Signal source not found '" + info.m_signal.first + "'", sigSource);
    SIGHT_ASSERT("invalid signal source '" + info.m_signal.first + "'", hasSignals);

    for(SlotInfoType slotInfo : info.m_slots)
    {
        core::tools::Object::sptr slotObj = core::tools::fwID::getObject(slotInfo.first);
        SIGHT_ASSERT("Failed to retrieve object '" + slotInfo.first + "'", slotObj);
        core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< core::com::HasSlots >(slotObj);
        SIGHT_ASSERT("invalid slot owner " << slotInfo.first, hasSlots);

        connections.connect(hasSignals, info.m_signal.second, hasSlots, slotInfo.second);
    }
}

//-----------------------------------------------------------------------------

Config::ConnectionInfo Config::parseConnections( const core::runtime::ConfigurationElement::csptr& connectionCfg,
                                                 const CSPTR(core::tools::Object)& obj)
{
    ConnectionInfo info;

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    for(core::runtime::ConfigurationElement::csptr elem :   connectionCfg->getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
        {
            SIGHT_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SIGHT_ASSERT(src << " configuration is not correct for "<< elem->getName(),
                         !uid.empty() && !key.empty());

            if (elem->getName() == "signal")
            {
                SIGHT_ASSERT("There must be only one signal by connection",
                             info.m_signal.first.empty() && info.m_signal.second.empty());
                info.m_signal = std::make_pair(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                info.m_slots.push_back( std::make_pair(uid, key) );
            }
        }
        else
        {
            SIGHT_ASSERT("Object uid is not defined, object used to retrieve signal must be present.", obj);
            uid = obj->getID();
            key = src;
            SIGHT_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
            SIGHT_ASSERT("There must be only one signal by connection",
                         info.m_signal.first.empty() && info.m_signal.second.empty());
            info.m_signal = std::make_pair(uid, key);
        }
    }

    // This is ok to return like this, thanks to C++11 rvalue there will be no copy of the vectors inside the struct
    return info;
}

//-----------------------------------------------------------------------------

ProxyConnections Config::parseConnections2(const core::runtime::ConfigurationElement::csptr& connectionCfg,
                                           const std::string& errMsgHead,
                                           std::function<std::string()> generateChannelNameFn)
{
#ifndef _DEBUG
    SIGHT_NOT_USED(errMsgHead);
#endif
    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    std::string channel;
    if(connectionCfg->hasAttribute("channel"))
    {
        channel = connectionCfg->getAttributeValue("channel");
        SIGHT_ASSERT(errMsgHead + "Empty 'channel' attribute", !channel.empty());
    }
    else
    {
        // Generate an UID
        channel = generateChannelNameFn();
    }

    ProxyConnections proxyCnt(channel);

    for(core::runtime::ConfigurationElement::csptr elem : connectionCfg->getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
        {
            SIGHT_ASSERT("errMsgHead + Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SIGHT_ASSERT(errMsgHead + src << " configuration is not correct for "<< elem->getName(),
                         !uid.empty() && !key.empty());

            if (elem->getName() == "signal")
            {
                proxyCnt.addSignalConnection(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                proxyCnt.addSlotConnection(uid, key);
            }
        }
        else
        {
            SIGHT_ASSERT(errMsgHead + "Signal or slot must be written as <signal>fwID/Key</signal> or "
                         "<slot>fwID/Key</slot>", false);
        }
    }

    // This is ok to return like this, thanks to C++11 rvalue there will be no copy of the vectors inside the struct
    return proxyCnt;
}

//-----------------------------------------------------------------------------

void Config::createProxy( const std::string& objectKey,
                          const CSPTR(core::runtime::ConfigurationElement)& cfg,
                          Config::ProxyConnectionsMapType& proxyMap,
                          const CSPTR(data::Object)& obj)
{
    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    SIGHT_ASSERT("Missing 'channel' attribute", cfg->hasAttribute("channel"));
    const std::string channel = cfg->getExistingAttributeValue("channel");
    ProxyConnections proxyCnt(channel);

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;
    for(core::runtime::ConfigurationElement::csptr elem :   cfg->getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
        {
            SIGHT_ASSERT("Wrong value for attribute src: " + src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SIGHT_ASSERT(src + " configuration is not correct for " + elem->getName(), !uid.empty() && !key.empty());

            core::tools::Object::sptr channelObj = core::tools::fwID::getObject(uid);

            if (elem->getName() == "signal")
            {
                core::com::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< core::com::HasSignals >(channelObj);
                SIGHT_ASSERT("Can't find the holder of signal '" + key + "'", hasSignals);
                core::com::SignalBase::sptr sig = hasSignals->signal(key);
                proxy->connect(channel, sig);
                proxyCnt.addSignalConnection(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< core::com::HasSlots >(channelObj);
                SIGHT_ASSERT("Can't find the holder of slot '" + key + "'", hasSlots);
                core::com::SlotBase::sptr slot = hasSlots->slot(key);
                proxy->connect(channel, slot);
                proxyCnt.addSlotConnection(uid, key);
            }
        }
        else
        {
            uid = obj->getID();
            key = src;
            SIGHT_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
            core::com::SignalBase::sptr sig = obj->signal(key);
            proxy->connect(channel, sig);
            proxyCnt.addSignalConnection(uid, key);
        }
    }
    proxyMap[objectKey].push_back(proxyCnt);
}

//-----------------------------------------------------------------------------

void Config::disconnectProxies(const std::string& objectKey, Config::ProxyConnectionsMapType& proxyMap)
{
    ProxyConnectionsMapType::iterator iter = proxyMap.find(objectKey);
    if (iter != proxyMap.end())
    {
        service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

        ProxyConnectionsVectType vectProxyConnections = iter->second;

        for(ProxyConnectionsVectType::value_type proxyConnections :   vectProxyConnections)
        {
            for(ProxyConnections::ProxyEltType signalElt :  proxyConnections.m_signals)
            {
                core::tools::Object::sptr obj          = core::tools::fwID::getObject(signalElt.first);
                core::com::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< core::com::HasSignals >(obj);
                core::com::SignalBase::sptr sig        = hasSignals->signal(signalElt.second);
                proxy->disconnect(proxyConnections.m_channel, sig);
            }
            for(ProxyConnections::ProxyEltType slotElt :  proxyConnections.m_slots)
            {
                core::tools::Object::sptr obj      = core::tools::fwID::getObject(slotElt.first);
                core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< core::com::HasSlots >(obj);
                core::com::SlotBase::sptr slot     = hasSlots->slot(slotElt.second);
                proxy->disconnect(proxyConnections.m_channel, slot);
            }
        }
        vectProxyConnections.clear();
        proxyMap.erase(objectKey);
    }
}

//-----------------------------------------------------------------------------

service::IService::Config Config::parseService(const boost::property_tree::ptree& srvElem,
                                               const std::string& errMsgHead)
{
#ifndef _DEBUG
    SIGHT_NOT_USED(errMsgHead);
#endif
    // Get attributes
    service::IService::Config srvConfig;

    srvConfig.m_uid = srvElem.get<std::string>("<xmlattr>.uid");
    SIGHT_ASSERT(errMsgHead + "'uid' attribute is empty.", !srvConfig.m_uid.empty());

    std::string errMsgTail = " when parsing service '" + srvConfig.m_uid + "'.";

    // Config
    std::string config = srvElem.get<std::string>("<xmlattr>.config", "");
    SIGHT_ASSERT(errMsgHead + "'config' attribute is empty.", !srvConfig.m_uid.empty());

    // Type
    srvConfig.m_type = srvElem.get<std::string>("<xmlattr>.type", "");
    SIGHT_ASSERT(errMsgHead + "Attribute \"type\" is required " + errMsgTail,
                 !srvConfig.m_type.empty());

    // AutoConnect
    srvConfig.m_globalAutoConnect = core::runtime::get_ptree_value(srvElem, "<xmlattr>.autoConnect", false);

    // Worker key
    srvConfig.m_worker = srvElem.get<std::string>("<xmlattr>.worker", "");

    boost::property_tree::ptree cfgElem = srvElem;
    // Get service configuration
    if (!config.empty())
    {
        const auto srvCfgFactory = service::extension::Config::getDefault();
        srvConfig.m_config = srvCfgFactory->getServiceConfig(config, srvConfig.m_type);
        cfgElem            = core::runtime::Convert::toPropertyTree(srvConfig.m_config);
    }
    else
    {
        boost::property_tree::ptree cfg;
        cfg.add_child("service", srvElem);
        srvConfig.m_config = core::runtime::Convert::fromPropertyTree(cfg);
    }

    // Check if user did not bind a service to another service
    auto serviceCfg = cfgElem.equal_range("service");
    SIGHT_ASSERT(errMsgHead + "Cannot bind a service to another service" + errMsgTail,
                 serviceCfg.first == serviceCfg.second);
    serviceCfg = cfgElem.equal_range("serviceList");
    SIGHT_ASSERT(errMsgHead + "Cannot bind a service to another service" + errMsgTail,
                 serviceCfg.first == serviceCfg.second);

    // Check first if we can create this service
    // If there is a missing object in its data list, then it is not possible

    // Collect all input/output configurations
    std::vector< std::pair<std::string, boost::property_tree::ptree> > objectCfgs;
    for(const auto& dataKeyword : s_DATA_KEYWORDS)
    {
        auto objCfgs = cfgElem.equal_range(dataKeyword);
        for (auto objCfg = objCfgs.first; objCfg != objCfgs.second; ++objCfg)
        {
            objectCfgs.push_back(std::make_pair(objCfg->first, objCfg->second));
        }
    }

    // Parse input/output configurations
    for(const auto& cfg : objectCfgs)
    {
        // Access type
        service::IService::ObjectServiceConfig objConfig;
        if(cfg.first == "in")
        {
            objConfig.m_access = service::IService::AccessType::INPUT;
        }
        else if(cfg.first == "out")
        {
            objConfig.m_access = service::IService::AccessType::OUTPUT;
        }
        else if(cfg.first == "inout")
        {
            objConfig.m_access = service::IService::AccessType::INOUT;
        }
        else
        {
            SIGHT_FATAL("Unreachable code");
        }

        // AutoConnect
        objConfig.m_autoConnect = core::runtime::get_ptree_value(cfg.second, "<xmlattr>.autoConnect", false);

        // Optional
        if(objConfig.m_access != service::IService::AccessType::OUTPUT)
        {
            objConfig.m_optional = core::runtime::get_ptree_value(cfg.second, "<xmlattr>.optional", false);
        }
        else
        {
            objConfig.m_optional = true;
        }

        // Do we use groups ?
        const auto group = cfg.second.get_optional<std::string>("<xmlattr>.group");
        if(group)
        {
            auto keyCfgs = cfgElem.equal_range("key");

            size_t count = 0;
            for (auto groupCfg = keyCfgs.first; groupCfg != keyCfgs.second; ++groupCfg)
            {
                service::IService::ObjectServiceConfig grouObjConfig = objConfig;

                // Identifier
                grouObjConfig.m_uid = groupCfg->second.get<std::string>("<xmlattr>.uid", "");
                SIGHT_ASSERT(errMsgHead + "\"uid\" attribute is empty" + errMsgTail, !grouObjConfig.m_uid.empty());

                grouObjConfig.m_key = KEY_GROUP_NAME(group.value(), count++);

                // AutoConnect
                grouObjConfig.m_autoConnect = core::runtime::get_ptree_value(groupCfg->second, "<xmlattr>.autoConnect",
                                                                             false);

                // Optional
                if(grouObjConfig.m_access != service::IService::AccessType::OUTPUT)
                {
                    grouObjConfig.m_optional = core::runtime::get_ptree_value(groupCfg->second, "<xmlattr>.optional",
                                                                              false);
                }

                srvConfig.m_objects.emplace_back(grouObjConfig);
            }
            srvConfig.m_groupSize[group.value()] = count;
        }
        else
        {
            // Identifier
            objConfig.m_uid = cfg.second.get<std::string>("<xmlattr>.uid", "");
            SIGHT_ASSERT(errMsgHead + "\"uid\" attribute is empty" + errMsgTail, !objConfig.m_uid.empty());

            // Key inside the service
            objConfig.m_key = cfg.second.get<std::string>("<xmlattr>.key", "");
            SIGHT_ASSERT(errMsgHead + "Missing object attribute 'key'" + errMsgTail, !objConfig.m_key.empty());

            srvConfig.m_objects.emplace_back(objConfig);
        }
    }

    return srvConfig;
}

// ----------------------------------------------------------------------------

} // namespace helper
} // namespace sight::service
