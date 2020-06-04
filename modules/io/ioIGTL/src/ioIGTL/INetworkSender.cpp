/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "ioIGTL/INetworkSender.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Object.hpp>

namespace ioIGTL
{

static const ::fwCom::Signals::SignalKeyType s_CONNECTED_SIGNAL    = "connected";
static const ::fwCom::Signals::SignalKeyType s_DISCONNECTED_SIGNAL = "disconnected";

static const ::fwServices::IService::KeyType s_OBJECTS_INOUT = "objects";

//-----------------------------------------------------------------------------

INetworkSender::INetworkSender()
{
    m_sigConnected    = newSignal<ConnectedSignalType>(s_CONNECTED_SIGNAL);
    m_sigDisconnected = newSignal<DisconnectSignalType>(s_DISCONNECTED_SIGNAL);
}

//-----------------------------------------------------------------------------

INetworkSender::~INetworkSender()
{
}

//-----------------------------------------------------------------------------

void INetworkSender::updating()
{
    if(this->isStarted())
    {
        const size_t numObjects = this->getKeyGroupSize(s_OBJECTS_INOUT);
        // Grab the objects to send.
        for(size_t i = 0; i < numObjects; ++i)
        {
            ::fwData::Object::csptr object = this->getInput< ::fwData::Object >(s_OBJECTS_INOUT, i);

            if (object)
            {
                this->sendObject(object, i);
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap INetworkSender::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_OBJECTS_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

} // namespace ioIGTL.
