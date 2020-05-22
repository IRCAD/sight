/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWSERVICES_HELPER_CONFIG_HPP__
#define __FWSERVICES_HELPER_CONFIG_HPP__

#include "fwServices/IService.hpp"
#include "fwServices/config.hpp"
#include "fwServices/helper/ProxyConnections.hpp"

#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

namespace fwTools
{
class Object;
}

namespace fwData
{
class Object;
}

namespace fwRuntime
{
struct ConfigurationElement;
}

namespace fwServices
{

namespace helper
{

class SigSlotConnection;

/**
 * @brief Provides configuration element parsing tools.
 */
class Config
{

public:

    typedef std::pair< std::string, ::fwCom::Signals::SignalKeyType > SignalInfoType;
    typedef std::pair< std::string, ::fwCom::Slots::SlotKeyType > SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    typedef std::string ObjectIdType;
    typedef std::vector<ProxyConnections> ProxyConnectionsVectType;
    typedef std::map< ObjectIdType, ProxyConnectionsVectType > ProxyConnectionsMapType;


    struct ConnectionInfo
    {
        SignalInfoType m_signal;
        SlotInfoContainerType m_slots;
    };

    /**
     * @brief Parses "<connect>" tags from given configuration and return a structure containing the signal and
     *        slots informations.
     *
     * @param cfg configuration element containing "<connect>" tags
     * @param obj optional object used to retrieve signal if uid is not defined [deprecated]
     */
    FWSERVICES_API static ConnectionInfo parseConnections(const CSPTR(::fwRuntime::ConfigurationElement)& cfg,
                                                          const CSPTR(::fwTools::Object)& obj =
                                                              CSPTR(::fwTools::Object)());

    /**
     * @brief Parses "<connect>" tags from given configuration and return a structure containing the signal and
     *        slots informations.
     *
     * @param cfg configuration element containing "<connect>" tags
     */
    FWSERVICES_API static ProxyConnections parseConnections2(
        const CSPTR(::fwRuntime::ConfigurationElement)& connectionCfg,
        const std::string& errMsgHead,
        std::function<std::string ()> generateChannelNameFn);

    /**
     * @brief Parses "<connect>" tags from given configuration to connect signals and slots using given helper.
     *
     * @param cfg configuration element containing "<connect>" tags
     * @param helper SigSlotConnection helper to connect signals and slots
     * @param obj optional object used to retrieve signal if uid is not defined [deprecated]
     */
    FWSERVICES_API static void createConnections(
        const CSPTR(::fwRuntime::ConfigurationElement)& cfg,
        ::fwCom::helper::SigSlotConnection& helper,
        const CSPTR(::fwTools::Object)& obj = CSPTR(::fwTools::Object)());


    /**
     * @brief Parses "<proxy>" tags from given configuration to connect signals and slots using proxies.
     *
     * @param objectKey Id of the object
     * @param cfg configuration element containing "<proxy>" tags
     * @param proxyMap map containing the proxy's signals and slots connections
     * @param obj optional object used to retrieve signal if uid is not defined
     */
    FWSERVICES_API static void createProxy(
        const std::string& objectKey,
        const CSPTR(::fwRuntime::ConfigurationElement)& cfg,
        ProxyConnectionsMapType& proxyMap,
        const CSPTR(::fwData::Object)& obj = CSPTR(::fwData::Object)());


    /// Disconnects all proxies associated to objectKey
    FWSERVICES_API static void disconnectProxies(const std::string& objectKey,
                                                 Config::ProxyConnectionsMapType& proxyMap);

    /// Parse a service and return a service configuration
    FWSERVICES_API static ::fwServices::IService::Config parseService(
        const CSPTR(::fwRuntime::ConfigurationElement)& srvElem,
        const std::string& errMsgHead);
};

} // namespace helper

} // namespace fwServices

#endif // __FWSERVICES_HELPER_CONFIG_HPP__
