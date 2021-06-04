/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <data/Object.hpp>

#include <service/IController.hpp>

namespace sight::module::io::igtl
{

/// Interface providing behavior of network senders, if you want create a new network sender you must be inherit.
class MODULE_IO_IGTL_CLASS_API INetworkSender : public service::IController
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(INetworkSender, service::IController);

    /// Initializes signals.
    MODULE_IO_IGTL_API INetworkSender();

    /// Destroys the interface.
    MODULE_IO_IGTL_API ~INetworkSender() override;

protected:

    /// Defines the signal's name emitted when service is connected.
    MODULE_IO_IGTL_API static const core::com::Signals::SignalKeyType s_CONNECTED_SIGNAL;

    /// Defines the signal's name emitted when service is disconnected.
    MODULE_IO_IGTL_API static const core::com::Signals::SignalKeyType s_DISCONNECTED_SIGNAL;

    /**
     * @brief Sends input objects
     * @pre The service must be started.
     */
    MODULE_IO_IGTL_API void updating() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Object::s_MODIFIED_SIG to s_UPDATE_SLOT.
     */
    MODULE_IO_IGTL_API KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Sends the obj at the specified index.
     * @pre the configuration group must exists.
     */
    MODULE_IO_IGTL_API virtual void sendObject(const data::Object::csptr& obj, const size_t index) = 0;

    /// Defines the signal emitted when service is connected.
    typedef core::com::Signal<void ()> ConnectedSignalType;
    ConnectedSignalType::sptr m_sigConnected;

    /// Defines the signal emitted when service is disconnected.
    typedef core::com::Signal<void ()> DisconnectSignalType;
    DisconnectSignalType::sptr m_sigDisconnected;
};

} // namespace sight::module::io::igtl.
