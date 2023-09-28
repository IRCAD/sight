/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service/extension/Config.hpp"
#include "service/helper/Config.hpp"

#include "core/com/proxy.hpp"

#include "service/detail/Service.hpp"
#include "service/extension/Factory.hpp"

#include <core/com/has_signals.hpp>
#include <core/com/has_slots.hpp>
#include <core/com/helper/sig_slot_connection.hpp>
#include <core/runtime/helper.hpp>
#include <core/tools/object.hpp>

#include <data/Object.hpp>

#include <array>
#include <regex>
#include <string>
#include <vector>

namespace sight::service::helper
{

/// container for the data keywords for a service configuration
const std::array<std::string, 3> s_DATA_KEYWORDS = {{"in", "out", "inout"}};

static std::map<std::string, service::base::sptr> s_servicesProps;
static std::mutex s_servicesPropsMutex;

//-----------------------------------------------------------------------------

void Config::createConnections(
    const core::runtime::config_t& connectionCfg,
    core::com::helper::sig_slot_connection& connections,
    const CSPTR(core::tools::object)& obj
)
{
    ConnectionInfo info = parseConnections(connectionCfg, obj);

    core::tools::object::sptr sigSource     = core::tools::id::get_object(info.m_signal.first);
    core::com::has_signals::sptr hasSignals = std::dynamic_pointer_cast<core::com::has_signals>(sigSource);

    SIGHT_ASSERT("Signal source not found '" + info.m_signal.first + "'", sigSource);
    SIGHT_ASSERT("invalid signal source '" + info.m_signal.first + "'", hasSignals);

    for(const SlotInfoType& slotInfo : info.m_slots)
    {
        core::tools::object::sptr slotObj = core::tools::id::get_object(slotInfo.first);
        SIGHT_ASSERT("Failed to retrieve object '" + slotInfo.first + "'", slotObj);
        core::com::has_slots::sptr hasSlots = std::dynamic_pointer_cast<core::com::has_slots>(slotObj);
        SIGHT_ASSERT("invalid slot owner " << slotInfo.first, hasSlots);

        connections.connect(hasSignals, info.m_signal.second, hasSlots, slotInfo.second);
    }
}

//-----------------------------------------------------------------------------

Config::ConnectionInfo Config::parseConnections(
    const core::runtime::config_t& connectionCfg,
    const CSPTR(core::tools::object)& obj
)
{
    ConnectionInfo info;

    for(const auto& elem : connectionCfg)
    {
        const auto src = elem.second.get_value<std::string>();
        static const std::regex re("(.*)/(.*)");
        std::smatch match;
        std::string uid;
        std::string key;

        if(std::regex_match(src, match, re))
        {
            SIGHT_ASSERT("Wrong value for attribute src: " << src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SIGHT_ASSERT(
                src << " configuration is not correct for " << elem.first,
                !uid.empty() && !key.empty()
            );

            if(elem.first == "signal")
            {
                SIGHT_ASSERT(
                    "There must be only one signal by connection",
                    info.m_signal.first.empty() && info.m_signal.second.empty()
                );
                info.m_signal = {uid, key};
            }
            else if(elem.first == "slot")
            {
                info.m_slots.emplace_back(uid, key);
            }
        }
        else
        {
            SIGHT_ASSERT("Object uid is not defined, object used to retrieve signal must be present.", obj);
            uid = obj->get_id();
            key = src;
            SIGHT_ASSERT("Element must be a signal or must be written as <fwID/Key>", elem.first == "signal");
            SIGHT_ASSERT(
                "There must be only one signal by connection",
                info.m_signal.first.empty() && info.m_signal.second.empty()
            );
            info.m_signal = {uid, key};
        }
    }

    // This is ok to return like this, thanks to C++11 rvalue there will be no copy of the vectors inside the struct
    return info;
}

//-----------------------------------------------------------------------------

ProxyConnections Config::parseConnections2(
    const core::runtime::config_t& connectionCfg,
    const std::string& errMsgHead,
    std::function<std::string()> generateChannelNameFn
)
{
#ifndef _DEBUG
    SIGHT_NOT_USED(errMsgHead);
#endif
    const std::string channel = connectionCfg.get<std::string>("<xmlattr>.channel", generateChannelNameFn());

    ProxyConnections proxyCnt(channel);

    for(const auto& elem : connectionCfg)
    {
        const static std::regex re("(.*)/(.*)");
        std::smatch match;
        std::string uid;
        std::string key;

        const auto src = elem.second.get_value<std::string>();
        if(std::regex_match(src, match, re))
        {
            SIGHT_ASSERT("errMsgHead + Wrong value for attribute src: " << src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SIGHT_ASSERT(
                errMsgHead + src << " configuration is not correct for " << elem.first,
                !uid.empty() && !key.empty()
            );

            if(elem.first == "signal")
            {
                proxyCnt.addSignalConnection(uid, key);
            }
            else if(elem.first == "slot")
            {
                proxyCnt.addSlotConnection(uid, key);
            }
        }
        else if(elem.first != "<xmlattr>")
        {
            SIGHT_ASSERT(
                errMsgHead + "Signal or slot must be written as <signal>fwID/Key</signal> or "
                             "<slot>fwID/Key</slot>",
                false
            );
        }
    }

    // This is ok to return like this, thanks to C++11 rvalue there will be no copy of the vectors inside the struct
    return proxyCnt;
}

//-----------------------------------------------------------------------------

void Config::disconnectProxies(const std::string& objectKey, Config::ProxyConnectionsMapType& proxyMap)
{
    auto iter = proxyMap.find(objectKey);
    if(iter != proxyMap.end())
    {
        core::com::proxy::sptr proxy = core::com::proxy::get();

        ProxyConnectionsVectType vectProxyConnections = iter->second;

        for(const ProxyConnectionsVectType::value_type& proxyConnections : vectProxyConnections)
        {
            for(const ProxyConnections::ProxyEltType& signalElt : proxyConnections.m_signals)
            {
                core::tools::object::sptr obj           = core::tools::id::get_object(signalElt.first);
                core::com::has_signals::sptr hasSignals = std::dynamic_pointer_cast<core::com::has_signals>(obj);
                core::com::signal_base::sptr sig        = hasSignals->signal(signalElt.second);
                proxy->disconnect(proxyConnections.m_channel, sig);
            }

            for(const ProxyConnections::ProxyEltType& slotElt : proxyConnections.m_slots)
            {
                core::tools::object::sptr obj       = core::tools::id::get_object(slotElt.first);
                core::com::has_slots::sptr hasSlots = std::dynamic_pointer_cast<core::com::has_slots>(obj);
                core::com::slot_base::sptr slot     = hasSlots->slot(slotElt.second);
                proxy->disconnect(proxyConnections.m_channel, slot);
            }
        }

        vectProxyConnections.clear();
        proxyMap.erase(objectKey);
    }
}

//-----------------------------------------------------------------------------

service::detail::ServiceConfig Config::parseService(
    const boost::property_tree::ptree& srvElem,
    const std::string& errMsgHead
)
{
#ifndef _DEBUG
    SIGHT_NOT_USED(errMsgHead);
#endif

    // Get attributes
    service::detail::ServiceConfig srvConfig;

    srvConfig.m_uid = srvElem.get<std::string>("<xmlattr>.uid");
    SIGHT_ASSERT(errMsgHead + "'uid' attribute is empty.", !srvConfig.m_uid.empty());

    std::string errMsgTail = " when parsing service '" + srvConfig.m_uid + "'.";

    // Config
    std::string config = srvElem.get<std::string>("<xmlattr>.config", "");
    SIGHT_ASSERT(errMsgHead + "'config' attribute is empty.", !srvConfig.m_uid.empty());

    // Type
    srvConfig.m_type = srvElem.get<std::string>("<xmlattr>.type", "");
    SIGHT_ASSERT(
        std::string(errMsgHead) + "Attribute \"type\" is required " + errMsgTail,
        !srvConfig.m_type.empty()
    );

    // AutoConnect
    srvConfig.m_globalAutoConnect = core::runtime::get_ptree_value(srvElem, "<xmlattr>.autoConnect", false);

    // Worker key
    srvConfig.m_worker = srvElem.get<std::string>("<xmlattr>.worker", "");

    // Get service configuration
    if(!config.empty())
    {
        const auto srvCfgFactory = service::extension::Config::getDefault();
        srvConfig.m_config = srvCfgFactory->getServiceConfig(config, srvConfig.m_type);
    }
    else
    {
        srvConfig.m_config = srvElem;
    }

    // Check if user did not bind a service to another service
    auto serviceCfg = srvElem.equal_range("service");
    SIGHT_ASSERT(
        std::string(errMsgHead) + "Cannot bind a service to another service" + errMsgTail,
        serviceCfg.first == serviceCfg.second
    );
    serviceCfg = srvElem.equal_range("serviceList");
    SIGHT_ASSERT(
        std::string(errMsgHead) + "Cannot bind a service to another service" + errMsgTail,
        serviceCfg.first == serviceCfg.second
    );

    // Check first if we can create this service
    // If there is a missing object in its data list, then it is not possible

    // Collect all input/output configurations
    std::vector<std::pair<std::string, boost::property_tree::ptree> > objectCfgs;
    for(const auto& dataKeyword : s_DATA_KEYWORDS)
    {
        auto objCfgs = srvElem.equal_range(dataKeyword);
        for(auto objCfg = objCfgs.first ; objCfg != objCfgs.second ; ++objCfg)
        {
            objectCfgs.emplace_back(objCfg->first, objCfg->second);
        }
    }

    // Parse input/output configurations
    for(const auto& cfg : objectCfgs)
    {
        // Access type
        service::detail::ObjectServiceConfig objConfig;
        if(cfg.first == "in")
        {
            objConfig.m_access = data::Access::in;
        }
        else if(cfg.first == "out")
        {
            objConfig.m_access = data::Access::out;
        }
        else if(cfg.first == "inout")
        {
            objConfig.m_access = data::Access::inout;
        }
        else
        {
            SIGHT_FATAL("Unreachable code");
        }

        // Do we use groups ?
        const auto group = cfg.second.get_optional<std::string>("<xmlattr>.group");
        if(group)
        {
            auto keyCfgs           = cfg.second.equal_range("key");
            const std::string& key = group.value();
            const auto defaultCfg  = getKeyProps(srvConfig.m_type, key);

            objConfig.m_autoConnect = core::runtime::get_ptree_value(
                cfg.second,
                "<xmlattr>.autoConnect",
                defaultCfg.first
            );

            // Optional is global to all keys in the group
            if(objConfig.m_access != data::Access::out)
            {
                objConfig.m_optional = core::runtime::get_ptree_value(
                    cfg.second,
                    "<xmlattr>.optional",
                    defaultCfg.second
                );
            }
            else
            {
                objConfig.m_optional = true;
            }

            std::size_t count = 0;
            for(auto groupCfg = keyCfgs.first ; groupCfg != keyCfgs.second ; ++groupCfg)
            {
                service::detail::ObjectServiceConfig groupObjConfig = objConfig;

                // Identifier
                groupObjConfig.m_uid = groupCfg->second.get<std::string>("<xmlattr>.uid", "");
                SIGHT_ASSERT(
                    std::string(errMsgHead) + "\"uid\" attribute is empty" + errMsgTail,
                    !groupObjConfig.m_uid.empty()
                );

                groupObjConfig.m_key = key;

                // AutoConnect can be overriden by element in the group
                groupObjConfig.m_autoConnect = core::runtime::get_ptree_value(
                    groupCfg->second,
                    "<xmlattr>.autoConnect",
                    groupObjConfig.m_autoConnect
                );

                // Optional can be overriden by element in the group
                if(groupObjConfig.m_access != data::Access::out)
                {
                    groupObjConfig.m_optional = core::runtime::get_ptree_value(
                        groupCfg->second,
                        "<xmlattr>.optional",
                        groupObjConfig.m_optional
                    );
                }
                else
                {
                    groupObjConfig.m_optional = true;
                }

                // Assign the current object config in the service config
                srvConfig.m_objects[{key, count++}] = groupObjConfig;
            }
        }
        else
        {
            // Identifier
            objConfig.m_uid = cfg.second.get<std::string>("<xmlattr>.uid", "");
            SIGHT_ASSERT(
                std::string(errMsgHead) + "\"uid\" attribute is empty" + errMsgTail,
                !objConfig.m_uid.empty()
            );

            // Key inside the service
            objConfig.m_key = cfg.second.get<std::string>("<xmlattr>.key", "");
            SIGHT_ASSERT(
                std::string(errMsgHead) + "Missing object attribute 'key'" + errMsgTail,
                !objConfig.m_key.empty()
            );

            const auto defaultCfg = getKeyProps(srvConfig.m_type, objConfig.m_key);

            // AutoConnect
            objConfig.m_autoConnect = core::runtime::get_ptree_value(
                cfg.second,
                "<xmlattr>.autoConnect",
                defaultCfg.first
            );

            // Optional
            if(objConfig.m_access != data::Access::out)
            {
                objConfig.m_optional = core::runtime::get_ptree_value(
                    cfg.second,
                    "<xmlattr>.optional",
                    defaultCfg.second
                );
            }
            else
            {
                objConfig.m_optional = true;
            }

            // Assign the current object config in the service config
            srvConfig.m_objects[{objConfig.m_key, std::nullopt}] = objConfig;
        }
    }

    return srvConfig;
}

//------------------------------------------------------------------------------

std::pair<bool, bool> Config::getKeyProps(
    const std::string& serviceType,
    const std::string& key
)
{
    std::lock_guard guard(s_servicesPropsMutex);

    service::base::sptr srv;
    auto it = s_servicesProps.find(serviceType);
    if(it == s_servicesProps.end())
    {
        service::extension::Factory::sptr srvFactory = service::extension::Factory::getDefault();
        srv                          = srvFactory->create(serviceType);
        s_servicesProps[serviceType] = srv;
    }
    else
    {
        srv = it->second;
    }

    return srv->m_pimpl->getKeyProps(key);
}

// ----------------------------------------------------------------------------

void Config::clearProps()
{
    std::lock_guard guard(s_servicesPropsMutex);
    s_servicesProps.clear();
}

// ----------------------------------------------------------------------------

} // namespace sight::service::helper
