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

#include <data/object.hpp>

#include <service/controller.hpp>

namespace sight::module::io::igtl
{

/// Interface providing behavior of network senders, if you want create a new network sender you must be inherit.
class MODULE_IO_IGTL_CLASS_API network_sender : public service::controller
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(network_sender, service::controller);

    /// Initializes signals.
    MODULE_IO_IGTL_API network_sender();

    /// Destroys the interface.
    MODULE_IO_IGTL_API ~network_sender() override;

protected:

    /// Defines the signal's name emitted when service is connected.
    MODULE_IO_IGTL_API static const core::com::signals::key_t CONNECTED_SIGNAL;

    /// Defines the signal's name emitted when service is disconnected.
    MODULE_IO_IGTL_API static const core::com::signals::key_t DISCONNECTED_SIGNAL;

    /**
     * @brief Sends input objects
     * @pre The service must be started.
     */
    MODULE_IO_IGTL_API void updating() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::object::MODIFIED_SIG to service::slots::UPDATE.
     */
    MODULE_IO_IGTL_API connections_t auto_connections() const override;

    /**
     * @brief Sends the obj at the specified index.
     * @pre the configuration group must exists.
     */
    MODULE_IO_IGTL_API virtual void sendObject(const data::object::csptr& obj, std::size_t index) = 0;

    /// Defines the signal emitted when service is connected.
    typedef core::com::signal<void ()> ConnectedSignalType;
    ConnectedSignalType::sptr m_sigConnected;

    /// Defines the signal emitted when service is disconnected.
    typedef core::com::signal<void ()> DisconnectSignalType;
    DisconnectSignalType::sptr m_sigDisconnected;

    static constexpr std::string_view s_OBJECTS_INPUT = "objects";
    data::ptr_vector<data::object, sight::data::Access::in> m_objects {this, s_OBJECTS_INPUT, true};
};

} // namespace sight::module::io::igtl.
