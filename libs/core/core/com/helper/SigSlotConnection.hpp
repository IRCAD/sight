/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/config.hpp"

#include <core/com/Connection.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>

#include <list>
#include <utility>
#include <vector>

namespace sight::core::com
{
namespace helper
{

/**
 * @brief  This class provides few tools to ease connect/disconnect between a signal emitter and a slot receiver.
 */
class CORE_CLASS_API SigSlotConnection
{

public:

    typedef std::list< core::com::Connection > ConnectionContainerType;

    typedef std::pair< core::com::Signals::SignalKeyType, core::com::Slots::SlotKeyType > KeyConnectionType;

    typedef std::vector< KeyConnectionType > KeyConnectionsType;

    /// Constructor, do nothing
    CORE_API SigSlotConnection();

    /// Destructor, call disconnect()
    CORE_API virtual ~SigSlotConnection();

    /// Connect signal to slot, and register this new connection in  m_connections
    CORE_API void connect(const core::com::HasSignals::csptr& hasSignals, core::com::Signals::SignalKeyType signalKey,
                          const core::com::HasSlots::csptr& hasSlots, core::com::Slots::SlotKeyType slotKey );

    /// Connect signals to slots, and register these new connections in  m_connections
    CORE_API void connect(const core::com::HasSignals::csptr& hasSignals, const core::com::HasSlots::csptr& hasSlots,
                          const KeyConnectionsType& keyConnections );

    /// Add a connection
    CORE_API void addConnection( core::com::Connection connection );

    /// Disconnect all registered connections and clear m_connections
    CORE_API void disconnect();

protected:

    /// Connections storage
    ConnectionContainerType m_connections;
};

} // end namespace helper
} // end namespace sight::core::com
