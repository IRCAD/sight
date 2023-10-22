/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "network_sender.hpp"

#include <core/com/signal.hxx>

#include <data/object.hpp>

namespace sight::module::io::igtl
{

const core::com::signals::key_t network_sender::CONNECTED_SIGNAL    = "connected";
const core::com::signals::key_t network_sender::DISCONNECTED_SIGNAL = "disconnected";

//-----------------------------------------------------------------------------

network_sender::network_sender() :
    m_sigConnected(new_signal<connected_signal_t>(CONNECTED_SIGNAL)),
    m_sigDisconnected(new_signal<disconnect_signal_t>(DISCONNECTED_SIGNAL))
{
}

//-----------------------------------------------------------------------------

network_sender::~network_sender()
= default;

//-----------------------------------------------------------------------------

void network_sender::updating()
{
    if(this->started())
    {
        const std::size_t num_objects = m_objects.size();
        // Grab the objects to send.
        for(std::size_t i = 0 ; i < num_objects ; ++i)
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

service::connections_t network_sender::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_OBJECTS_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

// ----------------------------------------------------------------------------

} // namespace sight::module::io::igtl.
