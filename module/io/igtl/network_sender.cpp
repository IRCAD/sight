/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
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

#include <data/object.hpp>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

network_sender::network_sender()
{
    new_signal<signals::connected_t>(signals::CONNECTED);
    new_signal<signals::disconnected_t>(signals::DISCONNECTED);
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
                this->send_object(object.get_shared(), i);
            }
        }
    }
}

//-----------------------------------------------------------------------------

service::connections_t network_sender::auto_connections() const
{
    service::connections_t connections;
    connections.push(OBJECTS_INPUT, data::signals::MODIFIED, service::slots::UPDATE);
    return connections;
}

// ----------------------------------------------------------------------------

} // namespace sight::module::io::igtl.
