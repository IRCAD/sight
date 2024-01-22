/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
#include "modules/io/igtl/network_sender.hpp"

#include <data/object.hpp>

#include <io/igtl/client.hpp>
#include <io/igtl/server.hpp>

#include <cstdint>
#include <future>
#include <string>

namespace sight::module::io::igtl
{

/**
 * @brief OpenIGTLink server that will send objects to the connected clients
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="sight::module::io::igtl::server_sender" auto_connect="true" >
 *      <port>...</port>
 *      <in group="objects">
 *           <key uid="..." deviceName="device01" />
 *           <key uid="..." deviceName="device02" />
 *      </in>
 * </service>
 * @endcode
 * @subsection Configuration Configuration:
 * - \b port : defines the port where the objects will be sent
 * @subsection Input Input:
 * - \b objects [sight::data::object]: specified objects to send.
 * They must have an attribute 'deviceName' to know the device-name used for this specific data.
 **/

class MODULE_IO_IGTL_CLASS_API server_sender : public module::io::igtl::network_sender
{
public:

    SIGHT_DECLARE_SERVICE(server_sender, module::io::igtl::network_sender);

    /// Constructor
    MODULE_IO_IGTL_API server_sender();

    /// Destructor
    MODULE_IO_IGTL_API ~server_sender() override;

protected:

    /// Configure port and device name
    MODULE_IO_IGTL_API void configuring() override;

    /// Start the server.
    MODULE_IO_IGTL_API void starting() override;

    /// Stop the server
    MODULE_IO_IGTL_API void stopping() override;

private:

    /**
     * @brief method to send data.
     *
     * @param[in] _obj obj to send
     * @param[in] _index index of the object in the group
     */
    void send_object(const data::object::csptr& _obj, std::size_t _index) override;

    /// Server instance
    sight::io::igtl::server::sptr m_server;

    /// Future used to wait for the server
    std::future<void> m_server_future;

    /// port preference key
    std::string m_port_config;

    /// Vector of device name if the group configuration exists.
    std::vector<std::string> m_device_names;
};

} // namespace sight::module::io::igtl
