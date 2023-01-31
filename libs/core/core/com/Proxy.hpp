/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "core/config.hpp"

#include <core/base.hpp>
#include <core/com/SignalBase.hpp>
#include <core/com/SlotBase.hpp>
#include <core/mt/types.hpp>

#include <map>
#include <vector>

namespace sight::core::com
{

/**
 * @brief This class defines a proxy for signal/slot connections.
 *
 * @note All the signals registered in a Proxy's channel are connected to all slots registered in the same channel.
 */
class CORE_CLASS_API Proxy
{
public:

    using sptr           = std::shared_ptr<Proxy>;
    using ChannelKeyType = std::string;

    /// Constructor, does nothing
    CORE_API Proxy() = default;

    /// Destructor, does nothing
    CORE_API ~Proxy();

    /// Returns an instance of Proxy.
    CORE_API static Proxy::sptr get();

    /// Registers a signal in the channel. It will be connected to all slots in the channel.
    CORE_API void connect(ChannelKeyType channel, core::com::SignalBase::sptr signal);

    /// Registers a slot in the channel. It will be connected to all signals in the channel.
    CORE_API void connect(ChannelKeyType channel, core::com::SlotBase::sptr slot);

    /// Unregisters the signal. Disconnects it from the slots in channel
    CORE_API void disconnect(ChannelKeyType channel, core::com::SignalBase::sptr signal);

    /// Unregisters the slot. Disconnects it from the signals in channel
    CORE_API void disconnect(ChannelKeyType channel, core::com::SlotBase::sptr slot);

private:

    /**
     * @brief Structure to regsiter signal and slot informations
     * Contains a signal container, a slot container and a mutex to protect access
     */
    struct SigSlots
    {
        using SignalContainerType = std::set<core::com::SignalBase::sptr>;
        using SlotContainerType   = std::set<core::com::SlotBase::sptr>;
        SignalContainerType m_signals;
        SlotContainerType m_slots;
        SlotContainerType::iterator m_lastConnectedSlot;

        core::mt::ReadWriteMutex m_mutex;
    };

    using ChannelMapType = std::map<ChannelKeyType, SPTR(SigSlots)>;

    /// Association channels, SigSlot
    ChannelMapType m_channels;

    /// Used to protect the m_channels access.
    mutable core::mt::ReadWriteMutex m_channelMutex;
};

} // namespace sight::core::com
