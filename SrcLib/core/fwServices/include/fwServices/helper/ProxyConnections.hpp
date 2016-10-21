/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_HELPER_PROXYCONNECTIONS_HPP__
#define __FWSERVICES_HELPER_PROXYCONNECTIONS_HPP__

#include "fwServices/config.hpp"

#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hpp>

namespace fwServices
{

namespace helper
{

/// Helper class to register proxy connections.
struct ProxyConnections
{
    typedef std::pair< std::string, ::fwCom::Signals::SignalKeyType > SignalInfoType;
    typedef std::pair< std::string, ::fwCom::Slots::SlotKeyType > SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;
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
    bool empty() const
    {
        return m_slots.empty() && m_signals.empty();
    }
};

} // namespace helper

} // namespace fwServices

#endif // __FWSERVICES_HELPER_PROXYCONNECTIONS_HPP__
