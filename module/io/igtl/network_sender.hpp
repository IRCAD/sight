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

#include <data/object.hpp>

#include <service/controller.hpp>

namespace sight::module::io::igtl
{

/// Interface providing behavior of network senders, if you want create a new network sender you must be inherit.
class network_sender : public service::controller
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
    SIGHT_DECLARE_SERVICE(network_sender, service::controller);

    /// Initializes signals.
    network_sender();

    /// Destroys the interface.
    ~network_sender() override;

protected:

    /**
     * @brief Sends input objects
     * @pre The service must be started.
     */
    void updating() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::signals::MODIFIED to service::slots::UPDATE.
     */
    connections_t auto_connections() const override;

    /**
     * @brief Sends the obj at the specified index.
     * @pre the configuration group must exists.
     */
    virtual void send_object(const data::object::csptr& _obj, std::size_t _index) = 0;

    static constexpr std::string_view OBJECTS_INPUT = "objects";

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    data::ptr_vector<data::object, sight::data::access::in> m_objects {this, OBJECTS_INPUT};
};

} // namespace sight::module::io::igtl.
