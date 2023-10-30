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
#include <core/com/signal_base.hpp>
#include <core/com/slot_base.hpp>
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
class CORE_CLASS_API proxy
{
public:

    using sptr             = std::shared_ptr<proxy>;
    using channel_key_type = std::string;

    /// Constructor, does nothing
    CORE_API proxy() = default;

    /// Destructor, does nothing
    CORE_API ~proxy();

    /// Returns an instance of Proxy.
    CORE_API static proxy::sptr get();

    /// Registers a signal in the channel. It will be connected to all slots in the channel.
    CORE_API void connect(channel_key_type _channel, core::com::signal_base::sptr _signal);

    /// Registers a slot in the channel. It will be connected to all signals in the channel.
    CORE_API void connect(channel_key_type _channel, core::com::slot_base::sptr _slot);

    /// Unregisters the signal. Disconnects it from the slots in channel
    CORE_API void disconnect(channel_key_type _channel, core::com::signal_base::sptr _signal);

    /// Unregisters the slot. Disconnects it from the signals in channel
    CORE_API void disconnect(channel_key_type _channel, core::com::slot_base::sptr _slot);

private:

    /**
     * @brief Structure to regsiter signal and slot informations
     * Contains a signal container, a slot container and a mutex to protect access
     */
    struct sig_slots
    {
        using signal_container_t = std::set<core::com::signal_base::sptr>;
        using slot_container_t   = std::set<core::com::slot_base::sptr>;
        signal_container_t m_signals;
        slot_container_t m_slots;
        slot_container_t::iterator m_last_connected_slot;

        core::mt::read_write_mutex m_mutex;
    };

    using channel_map_type = std::map<channel_key_type, SPTR(sig_slots)>;

    /// Association channels, SigSlot
    channel_map_type m_channels;

    /// Used to protect the m_channels access.
    mutable core::mt::read_write_mutex m_channel_mutex;
};

} // namespace sight::core::com
