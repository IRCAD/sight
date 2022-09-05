/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#pragma once

#include "service/config.hpp"

#include <core/com/Signals.hpp>
#include <core/com/Slots.hpp>

#include <utility>

namespace sight::service::helper
{

/// Helper class to register proxy connections.
struct ProxyConnections
{
    typedef std::pair<std::string, core::com::Signals::SignalKeyType> SignalInfoType;
    typedef std::pair<std::string, core::com::Slots::SlotKeyType> SlotInfoType;
    typedef std::vector<SlotInfoType> SlotInfoContainerType;
    typedef std::string UIDType;
    typedef std::string KeyType;
    typedef std::pair<UIDType, KeyType> ProxyEltType;
    typedef std::vector<ProxyEltType> ProxyEltVectType;

    std::string m_channel;
    ProxyEltVectType m_slots;
    ProxyEltVectType m_signals;

    ProxyConnections() :
        m_channel("undefined")
    {
    }

    ProxyConnections(std::string channel) :
        m_channel(std::move(channel))
    {
    }

    ~ProxyConnections()
    = default;

    //------------------------------------------------------------------------------

    void addSignalConnection(const UIDType& uid, const KeyType& key)
    {
        m_signals.push_back(std::make_pair(uid, key));
    }

    //------------------------------------------------------------------------------

    void addSignalConnection(const SignalInfoType& pair)
    {
        m_signals.push_back(pair);
    }

    //------------------------------------------------------------------------------

    void addSlotConnection(const UIDType& uid, const KeyType& key)
    {
        m_slots.push_back(std::make_pair(uid, key));
    }

    //------------------------------------------------------------------------------

    void addSlotConnection(const SlotInfoType& pair)
    {
        m_slots.push_back(pair);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool empty() const
    {
        return m_slots.empty() && m_signals.empty();
    }
};

} // namespace sight::service::helper
