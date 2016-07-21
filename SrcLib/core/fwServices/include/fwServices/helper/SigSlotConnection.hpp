/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_HELPER_SIGSLOTCONNECTION_HPP__
#define __FWSERVICES_HELPER_SIGSLOTCONNECTION_HPP__

#include <list>
#include <utility>
#include <vector>

#include "fwServices/config.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/HasSignals.hpp>


namespace fwServices
{
namespace helper
{

/**
 * @brief  This class provides few tools to ease connect/disconnect between a signal emitter and a slot receiver.
 */
class FWSERVICES_CLASS_API SigSlotConnection
{

public:

    typedef std::list< ::fwCom::Connection > ConnectionContainerType;

    typedef std::pair< ::fwCom::Signals::SignalKeyType, ::fwCom::Slots::SlotKeyType > KeyConnectionType;

    typedef std::vector< KeyConnectionType > KeyConnectionsType;

    /// Constructor, do nothing
    FWSERVICES_API SigSlotConnection();

    /// Destructor, call disconnect()
    FWSERVICES_API virtual ~SigSlotConnection();

    /// Connect signal to slot, and register this new connection in  m_connections
    FWSERVICES_API void connect(const ::fwCom::HasSignals::csptr& hasSignals, ::fwCom::Signals::SignalKeyType signalKey,
                                const ::fwCom::HasSlots::csptr& hasSlots, ::fwCom::Slots::SlotKeyType slotKey );

    /// Connect signals to slots, and register these new connections in  m_connections
    FWSERVICES_API void connect(const ::fwCom::HasSignals::csptr& hasSignals, const ::fwCom::HasSlots::csptr& hasSlots,
                                const KeyConnectionsType& keyConnections );

    /// Add a connection
    FWSERVICES_API void addConnection( ::fwCom::Connection connection );

    /// Disconnect all registered connections and clear m_connections
    FWSERVICES_API void disconnect();

protected:

    /// Connections storage
    ConnectionContainerType m_connections;
};

} // end namespace helper
} // end namespace fwServices

#endif // __FWSERVICES_HELPER_SIGSLOTCONNECTION_HPP__
