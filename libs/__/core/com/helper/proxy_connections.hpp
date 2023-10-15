/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "core/config.hpp"

#include <core/com/signals.hpp>
#include <core/com/slots.hpp>

#include <utility>

namespace sight::core::com::helper
{

/// Helper class to register proxy connections.
struct proxy_connections
{
    using SignalInfoType        = std::pair<std::string, core::com::signals::key_t>;
    using SlotInfoType          = std::pair<std::string, core::com::slots::key_t>;
    using SlotInfoContainerType = std::vector<SlotInfoType>;
    using UIDType               = std::string;
    using key_t                 = std::string;
    using ProxyEltType          = std::pair<UIDType, key_t>;
    using ProxyEltVectType      = std::vector<ProxyEltType>;

    std::string m_channel;
    ProxyEltVectType m_slots;
    ProxyEltVectType m_signals;

    proxy_connections() :
        m_channel("undefined")
    {
    }

    proxy_connections(std::string channel) :
        m_channel(std::move(channel))
    {
    }

    ~proxy_connections()
    = default;

    //------------------------------------------------------------------------------

    void addSignalConnection(const UIDType& uid, const key_t& key)
    {
        m_signals.push_back(std::make_pair(uid, key));
    }

    //------------------------------------------------------------------------------

    void addSignalConnection(const SignalInfoType& pair)
    {
        m_signals.push_back(pair);
    }

    //------------------------------------------------------------------------------

    void addSlotConnection(const UIDType& uid, const key_t& key)
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

} // namespace sight::core::com::helper
