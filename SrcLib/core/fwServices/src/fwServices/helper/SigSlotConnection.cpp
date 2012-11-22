/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwServices/helper/SigSlotConnection.hpp"

namespace fwServices
{
namespace helper
{

SigSlotConnection::SigSlotConnection()
{}

SigSlotConnection::~SigSlotConnection()
{
    this->disconnect();
}

void SigSlotConnection::connect(  ::fwCom::HasSignals::sptr hasSignals, ::fwCom::Signals::SignalKeyType signalKey,
                                  ::fwCom::HasSlots::sptr hasSlots, ::fwCom::Slots::SlotKeyType slotKey )
{
    ::fwCom::Connection connection;
    connection = hasSignals->signal( signalKey )->connect( hasSlots->slot( slotKey ) );
    m_connections.push_back(connection);
}

void SigSlotConnection::connect(::fwCom::HasSignals::sptr hasSignals,
                                ::fwCom::HasSlots::sptr hasSlots,
                                const KeyConnectionsType & keyConnections )
{
    ::fwCom::Connection connection;
    BOOST_FOREACH( KeyConnectionType keys, keyConnections )
    {
        connection = hasSignals->signal( keys.first )->connect( hasSlots->slot( keys.second ) );
        m_connections.push_back(connection);
    }
}

void SigSlotConnection::disconnect()
{
    BOOST_REVERSE_FOREACH( ::fwCom::Connection & connection, m_connections )
    {
        connection.disconnect();
    }
    m_connections.clear();
}

} // end namespace helper
} // end namespace fwServices
