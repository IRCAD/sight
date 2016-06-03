/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/helper/SigSlotConnection.hpp"

#include <fwCom/SignalBase.hpp>
#include <fwCore/spyLog.hpp>

#include <boost/foreach.hpp>

namespace fwServices
{
namespace helper
{

SigSlotConnection::SigSlotConnection()
{
}

SigSlotConnection::~SigSlotConnection()
{
    this->disconnect();
}

void SigSlotConnection::connect(  ::fwCom::HasSignals::csptr hasSignals, ::fwCom::Signals::SignalKeyType signalKey,
                                  ::fwCom::HasSlots::csptr hasSlots, ::fwCom::Slots::SlotKeyType slotKey )
{
    ::fwCom::Connection connection;
    connection = hasSignals->signal( signalKey )->connect( hasSlots->slot( slotKey ) );
    m_connections.push_back(connection);
}

void SigSlotConnection::connect(::fwCom::HasSignals::csptr hasSignals,
                                ::fwCom::HasSlots::csptr hasSlots,
                                const KeyConnectionsType & keyConnections )
{
    ::fwCom::Connection connection;
    for( KeyConnectionType keys : keyConnections )
    {
        auto signal = hasSignals->signal( keys.first );
        SLM_ASSERT("Signal '" + keys.first + "' not found.", signal);
        auto slot = hasSlots->slot( keys.second );
        SLM_ASSERT("Slot '" + keys.second + "' not found.", slot);
        connection = signal->connect( slot );
        m_connections.push_back(connection);
    }
}

void SigSlotConnection::addConnection( ::fwCom::Connection connection )
{
    m_connections.push_back(connection);
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
