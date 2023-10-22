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

#pragma once

#include "modules/io/igtl/config.hpp"

#include <core/com/signal.hpp>

#include <service/controller.hpp>

namespace sight::module::io::igtl
{

/// Interface providing behavior of network listeners, if you want create a new network listener you must be inherit.
class MODULE_IO_IGTL_CLASS_API network_listener : public service::controller
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(network_listener, service::controller);

    /// Initializes signals.
    MODULE_IO_IGTL_API network_listener();

    /// Destroys the interface.
    MODULE_IO_IGTL_API ~network_listener() override;

protected:

    /// Defines the signal's name emitted when service is connected.
    MODULE_IO_IGTL_API static const core::com::signals::key_t CONNECTED_SIGNAL;

    /// Defines the signal's name emitted when service is disconnected.
    MODULE_IO_IGTL_API static const core::com::signals::key_t DISCONNECTED_SIGNAL;

    /// Does nothing.
    MODULE_IO_IGTL_API void updating() override;

    /// Defines the signal emitted when service is connected.
    typedef core::com::signal<void ()> connected_signal_t;
    connected_signal_t::sptr m_sigConnected;

    /// Defines the signal emitted when service is disconnected.
    typedef core::com::signal<void ()> disconnect_signal_t;
    disconnect_signal_t::sptr m_sigDisconnected;
};

} // namespace sight::module::io::igtl.
