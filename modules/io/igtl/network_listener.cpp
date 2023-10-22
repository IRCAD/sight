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

#include "network_listener.hpp"

#include <core/com/signal.hxx>

#include <service/macros.hpp>

namespace sight::module::io::igtl
{

const core::com::signals::key_t network_listener::CONNECTED_SIGNAL    = "connected";
const core::com::signals::key_t network_listener::DISCONNECTED_SIGNAL = "disconnected";

//-----------------------------------------------------------------------------

network_listener::network_listener() :
    m_sigConnected(new_signal<connected_signal_t>(CONNECTED_SIGNAL)),
    m_sigDisconnected(new_signal<disconnect_signal_t>(DISCONNECTED_SIGNAL))
{
}

//-----------------------------------------------------------------------------

network_listener::~network_listener()
= default;

//-----------------------------------------------------------------------------

void network_listener::updating()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl.
