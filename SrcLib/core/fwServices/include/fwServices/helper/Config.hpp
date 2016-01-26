/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_HELPER_CONFIG_HPP__
#define __FWSERVICES_HELPER_CONFIG_HPP__

#include <fwCore/base.hpp>

#include <fwServices/config.hpp>
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

        ProxyConnections(const std::string& channel) : m_channel(channel)
        {
        }

        ~ProxyConnections()
        {
        }

        void addSlotConnection(UIDType uid, KeyType key)
        {
            m_slots.push_back(std::make_pair(uid, key));
        }
        void addSignalConnection(UIDType uid, KeyType key)
        {
            m_signals.push_back(std::make_pair(uid, key));
        }
    };

    typedef std::string ObjectIdType;
    typedef std::vector<ProxyConnections> ProxyConnectionsVectType;
    typedef std::map< ObjectIdType, ProxyConnectionsVectType > ProxyConnectionsMapType;

    /**
     * @brief Parses "<connect>" tags from given configuration to connect signals and slots using given helper.
     *
     * @param cfg configuration element containing "<connect>" tags
     * @param helper SigSlotConnection helper to connect signals and slots
     * @param obj optional object used to retrieve signal if uid is not defined
     */
    FWSERVICES_API static void createConnections(
        CSPTR(::fwRuntime::ConfigurationElement) cfg,
        SPTR(::fwServices::helper::SigSlotConnection) helper,
        const SPTR(::fwTools::Object)& obj = SPTR(::fwTools::Object)());

    /**
     * @brief Parses "<proxy>" tags from given configuration to connect signals and slots using proxies.
     *
     * @param objectKey Id of the object
     * @param cfg configuration element containing "<proxy>" tags
     * @param proxyMap map containing the proxy's signals and slots connections
     * @param obj optional object used to retrieve signal if uid is not defined
     */
    FWSERVICES_API static void createProxy(
        const std::string &objectKey,
        CSPTR(::fwRuntime::ConfigurationElement) cfg,
        ProxyConnectionsMapType &proxyMap,
        const SPTR(::fwData::Object)& obj = SPTR(::fwData::Object)());


    /// Disconnects all proxies associated to objectKey
    FWSERVICES_API static void disconnectProxies(const std::string &objectKey,
                                                 Config::ProxyConnectionsMapType &proxyMap);
};

} // namespace helper
} // namespace fwServices

#endif // __FWSERVICES_HELPER_CONFIG_HPP__

