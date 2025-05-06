/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#pragma once

#include <sight/core/config.hpp>

#include <core/com/connection.hpp>
#include <core/com/has_signals.hpp>
#include <core/com/has_slots.hpp>

#include <list>
#include <utility>
#include <vector>

namespace sight::core::com::helper
{

/**
 * @brief  This class provides few tools to ease connect/disconnect between a signal emitter and a slot receiver.
 */
class SIGHT_CORE_CLASS_API sig_slot_connection
{
public:

    using key_connection_t  = std::pair<core::com::signals::key_t, core::com::slots::key_t>;
    using key_connections_t = std::vector<key_connection_t>;

    /// Constructor, do nothing
    SIGHT_CORE_API sig_slot_connection();

    /// Destructor, call disconnect()
    SIGHT_CORE_API virtual ~sig_slot_connection();

    /// Connect signal to slot, and register this new connection in  m_connections
    SIGHT_CORE_API void connect(
        const core::com::has_signals::csptr& _has_signals,
        core::com::signals::key_t _signal_key,
        const core::com::has_slots::csptr& _has_slots,
        core::com::slots::key_t _slot_key
    );

    /// Connect signals to slots, and register these new connections in  m_connections
    SIGHT_CORE_API void connect(
        const core::com::has_signals::csptr& _has_signals,
        const core::com::has_slots::csptr& _has_slots,
        const key_connections_t& _key_connections
    );

    /// Add a connection
    SIGHT_CORE_API void add_connection(core::com::connection _connection);

    /// Disconnect all registered connections and clear m_connections
    SIGHT_CORE_API void disconnect();

protected:

    /// Connections storage
    using connection_container_t = std::list<core::com::connection>;
    connection_container_t m_connections;
};

} // namespace sight::core::com::helper
