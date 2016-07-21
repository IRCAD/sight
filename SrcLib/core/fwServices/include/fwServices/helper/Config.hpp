/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_HELPER_CONFIG_HPP__
#define __FWSERVICES_HELPER_CONFIG_HPP__

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

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

    /// Used to register proxy connection in order to properly disconnect it.
    struct ProxyConnections
    {
        typedef std::string UIDType;
        typedef std::string KeyType;
        typedef std::pair<UIDType, KeyType> ProxyEltType;
        typedef std::vector<ProxyEltType> ProxyEltVectType;

        std::string m_channel;
        ProxyEltVectType m_slots;
        ProxyEltVectType m_signals;

        ProxyConnections() : m_channel("undefined")
        {
        }

        ProxyConnections(const std::string& channel) : m_channel(channel)
        {
        }

        ~ProxyConnections()
        {
        }

        void addSignalConnection(UIDType uid, KeyType key)
        {
            m_signals.push_back(std::make_pair(uid, key));
        }
        void addSignalConnection(const SignalInfoType& pair)
        {
            m_signals.push_back(pair);
        }
        void addSlotConnection(UIDType uid, KeyType key)
        {
            m_slots.push_back(std::make_pair(uid, key));
        }
        void addSlotConnection(const SlotInfoType& pair)
        {
            m_slots.push_back(pair);
        }
    };

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
        ::fwServices::helper::SigSlotConnection& helper,
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

    /// Used to store object configuration in a service.
    struct ObjectServiceConfig
    {
        std::string m_uid;
        std::string m_key;
        ::fwServices::IService::AccessType m_access;
        bool m_autoConnect;
        bool m_optional;
    };

    /// Used to store a service configuration.
    struct ServiceConfig
    {
        std::string m_uid;
        std::string m_type;
        bool m_globalAutoConnect;
        std::string m_worker;
        std::vector<ObjectServiceConfig> m_objects;
        std::map<std::string, size_t> m_groupSize;
        CSPTR(::fwRuntime::ConfigurationElement) m_config;
    };

    /// Parse a service and return a service configuration
    static ServiceConfig parseService( const CSPTR(::fwRuntime::ConfigurationElement)& srvElem,
                                       const std::string& errMsgHead);
};

} // namespace helper

} // namespace fwServices

#endif // __FWSERVICES_HELPER_CONFIG_HPP__
