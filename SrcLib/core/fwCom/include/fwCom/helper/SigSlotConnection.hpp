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

#ifndef __FWCOM_HELPER_SIGSLOTCONNECTION_HPP__
#define __FWCOM_HELPER_SIGSLOTCONNECTION_HPP__

#include "fwCom/config.hpp"
#include <fwCom/Connection.hpp>
#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>

#include <list>
#include <utility>
#include <vector>


namespace fwCom
{
namespace helper
{

/**
 * @brief  This class provides few tools to ease connect/disconnect between a signal emitter and a slot receiver.
 */
class FWCOM_CLASS_API SigSlotConnection
{

public:

    typedef std::list< ::fwCom::Connection > ConnectionContainerType;

    typedef std::pair< ::fwCom::Signals::SignalKeyType, ::fwCom::Slots::SlotKeyType > KeyConnectionType;

    typedef std::vector< KeyConnectionType > KeyConnectionsType;

    /// Constructor, do nothing
    FWCOM_API SigSlotConnection();

    /// Destructor, call disconnect()
    FWCOM_API virtual ~SigSlotConnection();

    /// Connect signal to slot, and register this new connection in  m_connections
    FWCOM_API void connect(const ::fwCom::HasSignals::csptr& hasSignals, ::fwCom::Signals::SignalKeyType signalKey,
                           const ::fwCom::HasSlots::csptr& hasSlots, ::fwCom::Slots::SlotKeyType slotKey );

    /// Connect signals to slots, and register these new connections in  m_connections
    FWCOM_API void connect(const ::fwCom::HasSignals::csptr& hasSignals, const ::fwCom::HasSlots::csptr& hasSlots,
                           const KeyConnectionsType& keyConnections );

    /// Add a connection
    FWCOM_API void addConnection( ::fwCom::Connection connection );

    /// Disconnect all registered connections and clear m_connections
    FWCOM_API void disconnect();

protected:

    /// Connections storage
    ConnectionContainerType m_connections;
};

} // end namespace helper
} // end namespace fwCom

#endif // __FWCOM_HELPER_SIGSLOTCONNECTION_HPP__
