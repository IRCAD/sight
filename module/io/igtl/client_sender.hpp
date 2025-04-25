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

#include "module/io/igtl/network_sender.hpp"

#include <io/igtl/client.hpp>

namespace sight::module::io::igtl
{

/**
 * @brief OpenIGTLink client that will send objects to the connected server
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="sight::module::io::igtl::client_sender" auto_connect="true">
 *      <server>127.0.0.1:4242</server>
 *      <in group="objects">
 *           <key uid="..." deviceName="device01" />
 *           <key uid="..." deviceName="device02" />
 *      </in>
 * </service>
 * @endcode
 * @subsection Input Input:
 * - \b objects [sight::data::object]: specified objects to send to the connected server.
 * They must have an attribute 'deviceName' to known the device-name used for this specific data.
 * @subsection Configuration Configuration:
 * - \b deviceName : filter by device Name in Message
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class client_sender : public module::io::igtl::network_sender
{
public:

    SIGHT_DECLARE_SERVICE(client_sender, module::io::igtl::network_sender);

    /// Constructor
    client_sender();

    /// Destructor
    ~client_sender() override;

protected:

    /// Configures the service.
    void configuring() override;

    /**
     * @brief start the client and try to connect to the server specify in configuration
     */
    void starting() override;

    /**
     * @brief disconnect the client from the server
     */
    void stopping() override;

private:

    /**
     * @brief method to send data.
     *
     * @param[in] _obj obj to send
     * @param[in] _index index of the object in the group
     */
    void send_object(const data::object::csptr& _obj, std::size_t _index) override;

    /// client socket
    sight::io::igtl::client m_client;

    /// hostname config key
    std::string m_hostname_config;

    /// port config key
    std::string m_port_config;

    /// Vector of device name used
    std::vector<std::string> m_device_names;
};

} // namespace sight::module::io::igtl
