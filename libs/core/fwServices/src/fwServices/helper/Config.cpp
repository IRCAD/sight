/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwServices/helper/Config.hpp"
#include "fwServices/registry/ServiceConfig.hpp"

#include "fwServices/registry/Proxy.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/Object.hpp>

#include <regex>
#include <string>
#include <vector>

namespace fwServices
{

namespace helper
{

/// container for the data keywords for a service configuration
const std::array< std::string, 3 > s_DATA_KEYWORDS = {{ "in", "out", "inout" }};

//-----------------------------------------------------------------------------

void Config::createConnections( const ::fwRuntime::ConfigurationElement::csptr& connectionCfg,
                                ::fwCom::helper::SigSlotConnection& connections,
                                const CSPTR(::fwTools::Object)& obj)
{
    ConnectionInfo info = parseConnections(connectionCfg, obj);

    ::fwTools::Object::sptr sigSource    = ::fwTools::fwID::getObject(info.m_signal.first);
    ::fwCom::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< ::fwCom::HasSignals >(sigSource);

    SLM_ASSERT("Signal source not found '" + info.m_signal.first + "'", sigSource);
    SLM_ASSERT("invalid signal source '" + info.m_signal.first + "'", hasSignals);

    for(SlotInfoType slotInfo : info.m_slots)
    {
        ::fwTools::Object::sptr slotObj = ::fwTools::fwID::getObject(slotInfo.first);
        SLM_ASSERT("Failed to retrieve object '" + slotInfo.first + "'", slotObj);
        ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(slotObj);
        SLM_ASSERT("invalid slot owner " << slotInfo.first, hasSlots);

        connections.connect(hasSignals, info.m_signal.second, hasSlots, slotInfo.second);
    }
}

//-----------------------------------------------------------------------------

Config::ConnectionInfo Config::parseConnections( const ::fwRuntime::ConfigurationElement::csptr& connectionCfg,
                                                 const CSPTR(::fwTools::Object)& obj)
{
    ConnectionInfo info;

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    for(::fwRuntime::ConfigurationElement::csptr elem :   connectionCfg->getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
        {
            OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            OSLM_ASSERT(src << " configuration is not correct for "<< elem->getName(),
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

ProxyConnections Config::parseConnections2(const ::fwRuntime::ConfigurationElement::csptr& connectionCfg,
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

    for(::fwRuntime::ConfigurationElement::csptr elem : connectionCfg->getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
        {
            OSLM_ASSERT("errMsgHead + Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            OSLM_ASSERT(errMsgHead + src << " configuration is not correct for "<< elem->getName(),
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
                          const CSPTR(::fwRuntime::ConfigurationElement)& cfg,
                          Config::ProxyConnectionsMapType& proxyMap,
                          const CSPTR(::fwData::Object)& obj)
{
    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    SLM_ASSERT("Missing 'channel' attribute", cfg->hasAttribute("channel"));
    const std::string channel = cfg->getExistingAttributeValue("channel");
    ProxyConnections proxyCnt(channel);

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;
    for(::fwRuntime::ConfigurationElement::csptr elem :   cfg->getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
        {
            SLM_ASSERT("Wrong value for attribute src: " + src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SLM_ASSERT(src + " configuration is not correct for " + elem->getName(), !uid.empty() && !key.empty());

            ::fwTools::Object::sptr channelObj = ::fwTools::fwID::getObject(uid);

            if (elem->getName() == "signal")
            {
                ::fwCom::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< ::fwCom::HasSignals >(channelObj);
                SLM_ASSERT("Can't find the holder of signal '" + key + "'", hasSignals);
                ::fwCom::SignalBase::sptr sig = hasSignals->signal(key);
                proxy->connect(channel, sig);
                proxyCnt.addSignalConnection(uid, key);
            }
            else if (elem->getName() == "slot")
            {
                ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(channelObj);
                SLM_ASSERT("Can't find the holder of slot '" + key + "'", hasSlots);
                ::fwCom::SlotBase::sptr slot = hasSlots->slot(key);
                proxy->connect(channel, slot);
                proxyCnt.addSlotConnection(uid, key);
            }
        }
        else
        {
            uid = obj->getID();
            key = src;
            SLM_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem->getName() == "signal");
            ::fwCom::SignalBase::sptr sig = obj->signal(key);
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
        ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

        ProxyConnectionsVectType vectProxyConnections = iter->second;

        for(ProxyConnectionsVectType::value_type proxyConnections :   vectProxyConnections)
        {
            for(ProxyConnections::ProxyEltType signalElt :  proxyConnections.m_signals)
            {
                ::fwTools::Object::sptr obj          = ::fwTools::fwID::getObject(signalElt.first);
                ::fwCom::HasSignals::sptr hasSignals = std::dynamic_pointer_cast< ::fwCom::HasSignals >(obj);
                ::fwCom::SignalBase::sptr sig        = hasSignals->signal(signalElt.second);
                proxy->disconnect(proxyConnections.m_channel, sig);
            }
            for(ProxyConnections::ProxyEltType slotElt :  proxyConnections.m_slots)
            {
                ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(slotElt.first);
                ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                ::fwCom::SlotBase::sptr slot     = hasSlots->slot(slotElt.second);
                proxy->disconnect(proxyConnections.m_channel, slot);
            }
        }
        vectProxyConnections.clear();
        proxyMap.erase(objectKey);
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::Config Config::parseService(const ::fwRuntime::ConfigurationElement::csptr& srvElem,
                                                    const std::string& errMsgHead)
{
#ifndef _DEBUG
    FwCoreNotUsedMacro(errMsgHead);
#endif
    SLM_ASSERT("Configuration element is not a \"service\" node.", srvElem->getName() == "service");

    // Get attributes
    ::fwServices::IService::Config srvConfig;

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
    const ::fwRuntime::ConfigurationElement::AttributePair attribAutoConnect =
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
    ::fwRuntime::ConfigurationElement::csptr cfgElem = srvElem;
    if (!config.empty())
    {
        const auto srvCfgFactory = ::fwServices::registry::ServiceConfig::getDefault();
        cfgElem = srvCfgFactory->getServiceConfig(config, srvConfig.m_type);
    }
    srvConfig.m_config = cfgElem;

    // Check if user did not bind a service to another service
    for(::fwRuntime::ConfigurationElement::csptr elem :  cfgElem->getElements())
    {
        SLM_ASSERT(errMsgHead + "Cannot bind a service to another service" + errMsgTail,
                   elem->getName() != "service" &&
                   elem->getName() != "serviceList");
    }

    // Check first if we can create this service
    // If there is a missing object in its data list, then it is not possible
    auto cfgConstElem = ::fwRuntime::ConfigurationElement::constCast(srvElem);

    // Collect all input/output configurations
    std::vector< ::fwRuntime::ConfigurationElement::sptr > objectCfgs;
    for(const auto& dataKeyword : s_DATA_KEYWORDS)
    {
        auto objCfgs = cfgConstElem->find(dataKeyword);
        std::move(objCfgs.begin(), objCfgs.end(), std::back_inserter(objectCfgs));
    }

    // Parse input/output configurations
    for(const auto& cfg : objectCfgs)
    {
        // Access type
        ::fwServices::IService::ObjectServiceConfig objConfig;
        if(cfg->getName() == "in")
        {
            objConfig.m_access = ::fwServices::IService::AccessType::INPUT;
        }
        else if(cfg->getName() == "out")
        {
            objConfig.m_access = ::fwServices::IService::AccessType::OUTPUT;
        }
        else if(cfg->getName() == "inout")
        {
            objConfig.m_access = ::fwServices::IService::AccessType::INOUT;
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
        if(objConfig.m_access != ::fwServices::IService::AccessType::OUTPUT)
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
            std::vector< ::fwRuntime::ConfigurationElement::sptr > keyCfgs = cfg->find("key");

            size_t count = 0;
            for(const auto& groupCfg : keyCfgs)
            {
                ::fwServices::IService::ObjectServiceConfig grouObjConfig = objConfig;

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
                if(grouObjConfig.m_access != ::fwServices::IService::AccessType::OUTPUT)
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
} // namespace fwServices
