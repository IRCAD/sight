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

#pragma once

#include <core/com/signal.hpp>

#include <service/controller.hpp>

namespace sight::module::io::igtl
{

/// Interface providing behavior of network listeners, if you want create a new network listener you must be inherit.
class network_listener : public service::controller
{
public:

    struct signals
    {
        using connected_t    = core::com::signal<void ()>;
        using disconnected_t = core::com::signal<void ()>;
        static inline const signal_key_t CONNECTED    = "connected";
        static inline const signal_key_t DISCONNECTED = "disconnected";
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(network_listener, service::controller);

    /// Initializes signals.
    network_listener();

    /// Destroys the interface.
    ~network_listener() override;

protected:

    /// Does nothing.
    void updating() override;
};

} // namespace sight::module::io::igtl.
