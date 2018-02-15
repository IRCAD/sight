/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioNetwork/INetworkListener.hpp"

#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>

namespace ioNetwork
{
const ::fwCom::Signals::SignalKeyType INetworkListener::s_CONNECTED_SIGNAL    = "connected";
const ::fwCom::Signals::SignalKeyType INetworkListener::s_DISCONNECTED_SIGNAL = "disconnected";

//-----------------------------------------------------------------------------

INetworkListener::INetworkListener()
{
    m_sigConnected    = newSignal<ConnectedSignalType>(s_CONNECTED_SIGNAL);
    m_sigDisconnected = newSignal<DisconnectSignalType>(s_DISCONNECTED_SIGNAL);
}

//-----------------------------------------------------------------------------

INetworkListener::~INetworkListener()
{
}

//-----------------------------------------------------------------------------

void INetworkListener::updating()
{
}

//-----------------------------------------------------------------------------

void INetworkListener::swapping()
{
    // Classic default approach to update service when object change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL

