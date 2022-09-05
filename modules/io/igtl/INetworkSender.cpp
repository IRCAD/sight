/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "INetworkSender.hpp"

#include <core/com/Signal.hxx>

#include <data/Object.hpp>

namespace sight::module::io::igtl
{

const core::com::Signals::SignalKeyType INetworkSender::s_CONNECTED_SIGNAL    = "connected";
const core::com::Signals::SignalKeyType INetworkSender::s_DISCONNECTED_SIGNAL = "disconnected";

//-----------------------------------------------------------------------------

INetworkSender::INetworkSender()
{
    m_sigConnected    = newSignal<ConnectedSignalType>(s_CONNECTED_SIGNAL);
    m_sigDisconnected = newSignal<DisconnectSignalType>(s_DISCONNECTED_SIGNAL);
}

//-----------------------------------------------------------------------------

INetworkSender::~INetworkSender()
= default;

//-----------------------------------------------------------------------------

void INetworkSender::updating()
{
    if(this->isStarted())
    {
        const std::size_t numObjects = m_objects.size();
        // Grab the objects to send.
        for(std::size_t i = 0 ; i < numObjects ; ++i)
        {
            const auto object = m_objects[i].lock();

            if(object)
            {
                this->sendObject(object.get_shared(), i);
            }
        }
    }
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap INetworkSender::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_OBJECTS_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

} // namespace sight::module::io::igtl.
