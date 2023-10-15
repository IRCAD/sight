/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#include "modules/io/igtl/network_listener.hpp"

#include <data/object.hpp>

#include <io/igtl/Client.hpp>
#include <io/igtl/Server.hpp>

#include <cstdint>
#include <future>
#include <string>

namespace sight::module::io::igtl
{

/**
 * @brief OpenIGTLink server that will listen objects from the connected clients
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="sight::module::io::igtl::server_listener" auto_connect="true" >
 *      <port>...</port>
 *      <inout group="objects">
 *           <key uid="..." deviceName="device01" />
 *           <key uid="..." deviceName="device02" />
 *      </inout>
 * </service>
 * @endcode
 * @subsection Configuration Configuration:
 * - \b port : defines the port where the objects will be listen
 * @subsection Input Input:
 * - \b objects [sight::data::object]: specified objects to listen.
 * They must have an attribute 'deviceName' to know the device-name used for this specific data.
 **/
class MODULE_IO_IGTL_CLASS_API server_listener : public module::io::igtl::network_listener
{
public:

    SIGHT_DECLARE_SERVICE(server_listener, module::io::igtl::network_listener);

    /// Constructor
    MODULE_IO_IGTL_API server_listener();

    /// Destructor
    MODULE_IO_IGTL_API ~server_listener() override;

protected:

    /// Configure port and device name
    MODULE_IO_IGTL_API void configuring() override;

    /// Start the server.
    MODULE_IO_IGTL_API void starting() override;

    /// Stop the server
    MODULE_IO_IGTL_API void stopping() override;

private:

    /**
     * @brief method contain the receive loop
     * this method run in a thread
     */
    void receiveObject();

    /// Server instance
    sight::io::igtl::Server::sptr m_server;

    /// Future used to wait for the server
    std::future<void> m_serverFuture;

    /// Future used to wait for message
    std::future<void> m_receiveFuture;

    /// port preference key
    std::string m_portConfig;

    /// Vector of device name if the group configuration exists.
    std::vector<std::string> m_deviceNames;

    data::ptr_vector<data::object, sight::data::Access::inout> m_objects {this, "objects"};
};

} // namespace sight::module::io::igtl
