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

#include "services/helper/Config.hpp"
#include "services/registry/ServiceConfig.hpp"

#include "services/registry/Proxy.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>
#include <core/com/helper/SigSlotConnection.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Object.hpp>

#include <data/Object.hpp>

#include <array>
#include <regex>
#include <string>
#include <vector>

namespace sight::services
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

    SLM_ASSERT("Signal source not found '" + info.m_signal.first + "'", sigSource);
    SLM_ASSERT("invalid signal source '" + info.m_signal.first + "'", hasSignals);

    for(SlotInfoType slotInfo : info.m_slots)
    {
        core::tools::Object::sptr slotObj = core::tools::fwID::getObject(slotInfo.first);
        SLM_ASSERT("Failed to retrieve object '" + slotInfo.first + "'", slotObj);
        core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< core::com::HasSlots >(slotObj);
        SLM_ASSERT("invalid slot owner " << slotInfo.first, hasSlots);

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
            SLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SLM_ASSERT(src << " configuration is not correct for "<< elem->getName(),
                       !uid.empty() && !key.empty());

            if (elem->getName() == "signal")
            {
                SLM_ASSERT("There must be only one signal by connection",
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
            SLM_ASSERT("Object uid is not defined, object used to retrieve signal must be present.", obj);
            uid = obj->getID();
            key = src;
            SLM_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
            SLM_ASSERT("There must be only one signal by connection",
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
    FwCoreNotUsedMacro(errMsgHead);
#endif
    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    std::string channel;
    if(connectionCfg->hasAttribute("channel"))
    {
        channel = connectionCfg->getAttributeValue("channel");
        SLM_ASSERT(errMsgHead + "Empty 'channel' attribute", !channel.empty());
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
            SLM_ASSERT("errMsgHead + Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SLM_ASSERT(errMsgHead + src << " configuration is not correct for "<< elem->getName(),
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
            SLM_ASSERT(errMsgHead + "Signal or slot must be written as <signal>fwID/Key</signal> or "
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
    services::registry::Proxy::sptr proxy = services::registry::Proxy::getDefault();

    SLM_ASSERT("Missing 'channel' attribute", cfg->hasAttribute("channel"));
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
            SLM_ASSERT("Wrong value for attribute src: " + src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SLM_ASSERT(src + " configuration is not correct for " + elem->getName(), !uid.empty() && !key.empty());

            core::tools::Object::sptr channelObj = core::tools::fwID::getObject(uid);

            if (elem->getName() == "signal")
            {
                core::com::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< core::com::HasSignals >(channelObj);
                SLM_ASSERT("Can't find the holder of signal '" + key + "'", hasSignals);
                core::com::SignalBase::sptr sig = hasSignals->signal(key);
                proxy->connect(channel, sig);
                proxyCnt.addSignalConnection(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                core::com::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< core::com::HasSlots >(channelObj);
                SLM_ASSERT("Can't find the holder of slot '" + key + "'", hasSlots);
                core::com::SlotBase::sptr slot = hasSlots->slot(key);
                proxy->connect(channel, slot);
                proxyCnt.addSlotConnection(uid, key);
            }
        }
        else
        {
            uid = obj->getID();
            key = src;
            SLM_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
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
        services::registry::Proxy::sptr proxy = services::registry::Proxy::getDefault();

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

services::IService::Config Config::parseService(const core::runtime::ConfigurationElement::csptr& srvElem,
                                                const std::string& errMsgHead)
{
#ifndef _DEBUG
    FwCoreNotUsedMacro(errMsgHead);
#endif
    SLM_ASSERT("Configuration element is not a \"service\" node.", srvElem->getName() == "service");

    // Get attributes
    services::IService::Config srvConfig;

    // Uid
    if (srvElem->hasAttribute("uid"))
    {
        srvConfig.m_uid = srvElem->getAttributeValue("uid");
        SLM_ASSERT(errMsgHead + "'uid' attribute is empty.", !srvConfig.m_uid.empty());
    }

    std::string errMsgTail = " when parsing service '" + srvConfig.m_uid + "'.";

    // Config
    std::string config;
    if (srvElem->hasAttribute("config"))
    {
        config = srvElem->getAttributeValue("config");
        SLM_ASSERT(errMsgHead + "\"config\" attribute is empty" + errMsgTail, !config.empty());
    }

    // Type
    SLM_ASSERT(errMsgHead + "'type'' attribute is empty" + errMsgTail, srvElem->hasAttribute("type"));
    srvConfig.m_type = srvElem->getAttributeValue("type");
    SLM_ASSERT(errMsgHead + "Attribute \"type\" is required " + errMsgTail,
               !srvConfig.m_type.empty());

    // AutoConnect
    srvConfig.m_globalAutoConnect = false;
    const core::runtime::ConfigurationElement::AttributePair attribAutoConnect =
        srvElem->getSafeAttributeValue("autoConnect");
    if(attribAutoConnect.first)
    {
        SLM_ASSERT("'autoConnect' attribute must be either 'yes' or 'no'" + errMsgTail,
                   (!attribAutoConnect.first) || attribAutoConnect.second == "yes" || attribAutoConnect.second == "no");
        srvConfig.m_globalAutoConnect = (attribAutoConnect.second == "yes");
    }

    // Worker key
    srvConfig.m_worker = srvElem->getAttributeValue("worker");

    // Get service configuration
    core::runtime::ConfigurationElement::csptr cfgElem = srvElem;
    if (!config.empty())
    {
        const auto srvCfgFactory = services::registry::ServiceConfig::getDefault();
        cfgElem = srvCfgFactory->getServiceConfig(config, srvConfig.m_type);
    }
    srvConfig.m_config = cfgElem;

    // Check if user did not bind a service to another service
    for(core::runtime::ConfigurationElement::csptr elem :  cfgElem->getElements())
    {
        SLM_ASSERT(errMsgHead + "Cannot bind a service to another service" + errMsgTail,
                   elem->getName() != "service" &&
                   elem->getName() != "serviceList");
    }

    // Check first if we can create this service
    // If there is a missing object in its data list, then it is not possible
    auto cfgConstElem = core::runtime::ConfigurationElement::constCast(srvElem);

    // Collect all input/output configurations
    std::vector< core::runtime::ConfigurationElement::sptr > objectCfgs;
    for(const auto& dataKeyword : s_DATA_KEYWORDS)
    {
        auto objCfgs = cfgConstElem->find(dataKeyword);
        std::move(objCfgs.begin(), objCfgs.end(), std::back_inserter(objectCfgs));
    }

    // Parse input/output configurations
    for(const auto& cfg : objectCfgs)
    {
        // Access type
        services::IService::ObjectServiceConfig objConfig;
        if(cfg->getName() == "in")
        {
            objConfig.m_access = services::IService::AccessType::INPUT;
        }
        else if(cfg->getName() == "out")
        {
            objConfig.m_access = services::IService::AccessType::OUTPUT;
        }
        else if(cfg->getName() == "inout")
        {
            objConfig.m_access = services::IService::AccessType::INOUT;
        }
        else
        {
            SLM_FATAL("Unreachable code");
        }

        // AutoConnect
        auto autoConnect = cfg->getSafeAttributeValue("autoConnect");
        objConfig.m_autoConnect = false;
        if(autoConnect.first)
        {
            SLM_ASSERT(errMsgHead + "'autoConnect' attribute must be either 'yes' or 'no'" + errMsgTail,
                       autoConnect.second == "yes" || autoConnect.second == "no" );
            objConfig.m_autoConnect = (autoConnect.second == "yes");
        }

        // Optional
        if(objConfig.m_access != services::IService::AccessType::OUTPUT)
        {
            const std::string optionalStr = cfg->getAttributeValue("optional");

            objConfig.m_optional = false;
            if(!optionalStr.empty())
            {
                SLM_ASSERT("'optional' attribute must be either 'yes' or 'no'" + errMsgTail,
                           optionalStr == "" || optionalStr == "yes" || optionalStr == "no");
                objConfig.m_optional = optionalStr == "yes" ? true : false;
            }
        }
        else
        {
            objConfig.m_optional = true;
        }

        // Do we use groups ?
        std::string group = cfg->getAttributeValue("group");
        if(!group.empty())
        {
            std::vector< core::runtime::ConfigurationElement::sptr > keyCfgs = cfg->find("key");

            size_t count = 0;
            for(const auto& groupCfg : keyCfgs)
            {
                services::IService::ObjectServiceConfig grouObjConfig = objConfig;

                // Identifier
                grouObjConfig.m_uid = groupCfg->getAttributeValue("uid");
                SLM_ASSERT(errMsgHead + "\"uid\" attribute is empty" + errMsgTail, !grouObjConfig.m_uid.empty());

                grouObjConfig.m_key = KEY_GROUP_NAME(group, count++);

                // AutoConnect
                auto autoConnectPeyKey = groupCfg->getSafeAttributeValue("autoConnect");
                if(autoConnectPeyKey.first)
                {
                    SLM_ASSERT(errMsgHead + "'autoConnect' attribute must be either 'yes' or 'no'" + errMsgTail,
                               autoConnectPeyKey.second == "yes" || autoConnectPeyKey.second == "no" );
                    grouObjConfig.m_autoConnect = (autoConnectPeyKey.second == "yes");
                }
                // Optional
                if(grouObjConfig.m_access != services::IService::AccessType::OUTPUT)
                {
                    const std::string optionalStr = groupCfg->getAttributeValue("optional");

                    if(!optionalStr.empty())
                    {
                        SLM_ASSERT("'optional' attribute must be either 'yes' or 'no'" + errMsgTail,
                                   optionalStr == "" || optionalStr == "yes" || optionalStr == "no");
                        grouObjConfig.m_optional = optionalStr == "yes" ? true : false;
                    }
                }

                srvConfig.m_objects.emplace_back(grouObjConfig);
            }
            srvConfig.m_groupSize[group] = count;
        }
        else
        {
            // Identifier
            objConfig.m_uid = cfg->getAttributeValue("uid");
            SLM_ASSERT(errMsgHead + "\"uid\" attribute is empty" + errMsgTail, !objConfig.m_uid.empty());

            // Key inside the service
            objConfig.m_key = cfg->getAttributeValue("key");
            SLM_ASSERT(errMsgHead + "Missing object attribute 'key'" + errMsgTail, !objConfig.m_key.empty());

            srvConfig.m_objects.emplace_back(objConfig);
        }
    }

    return srvConfig;
}

// ----------------------------------------------------------------------------

} // namespace helper
} // namespace sight::services
