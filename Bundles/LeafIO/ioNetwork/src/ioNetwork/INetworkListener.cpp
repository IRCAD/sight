/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioNetwork/INetworkListener.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

namespace ioNetwork
{
const ::fwCom::Signals::SignalKeyType INetworkListener::s_CLIENT_CONNECTED_SIGNAL    = "clientConnected";
const ::fwCom::Signals::SignalKeyType INetworkListener::s_CLIENT_DISCONNECTED_SIGNAL = "clientDisconnected";
const ::fwCom::Signals::SignalKeyType INetworkListener::s_RECEIVED_OBJECT_SIGNAL     = "receivedObj";

//-----------------------------------------------------------------------------

INetworkListener::INetworkListener()
{
    m_sigClientConnected    = ClientConnectedSignalType::New();
    m_sigReceivedObject     = ReceivedObjectSignalType::New();
    m_sigClientDisconnected = ClientDisconnectSignalType::New();

    ::fwCom::HasSignals::m_signals(s_CLIENT_CONNECTED_SIGNAL, m_sigClientConnected);
    ::fwCom::HasSignals::m_signals(s_CLIENT_DISCONNECTED_SIGNAL, m_sigClientDisconnected);
    ::fwCom::HasSignals::m_signals(s_RECEIVED_OBJECT_SIGNAL, m_sigReceivedObject);
}

//-----------------------------------------------------------------------------

INetworkListener::~INetworkListener()
{
}

//-----------------------------------------------------------------------------

void INetworkListener::configuring()
{
}

//-----------------------------------------------------------------------------

void INetworkListener::starting()
{
}

//-----------------------------------------------------------------------------

void INetworkListener::stopping()
{
}

//-----------------------------------------------------------------------------

void INetworkListener::updating()
{
}

//-----------------------------------------------------------------------------

void INetworkListener::swapping()
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL

