/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

