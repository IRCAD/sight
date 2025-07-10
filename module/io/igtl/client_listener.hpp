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

#include "module/io/igtl/network_listener.hpp"

#include <data/object.hpp>

#include <io/igtl/client.hpp>

#include <future>
#include <string>

namespace sight::module::io::igtl
{

/**
 * @brief   OpenIGTLink client that will listen objects to the connected server
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="sight::module::io::igtl::client_listener" >
 *      <server>127.0.0.1:4242</server>
 *      <inout group="objects">
 *          <key uid="..." deviceName="device01" />
 *          <key uid="..." deviceName="device02" />
 *      </inout>
 * </service>
 * @endcode
 * @subsection In-Out In-Out:
 * - \b object [sight::data::object]:
 *   - if associated object is a timeline (data::matrix_tl or data::frame_tl):
 *   received IGTL data are pushed in timeline
 *   - else : object is updated with received IGTL data
 * @subsection Configuration Configuration:
 * - \b deviceName: filter by device Name in Message
 * - \b server: server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 *  (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class client_listener : public module::io::igtl::network_listener
{
public:

    SIGHT_DECLARE_SERVICE(client_listener, module::io::igtl::network_listener);

    /// Constructor
    client_listener();

    /// Destructor
    ~client_listener() override;

protected:

    /// Configure port, hostname and device name
    void configuring() override;

    /// start the client and try to connect to the server specify in configuration
    void starting() override;

    /// disconnect the client from the server
    void stopping() override;

private:

    /**
     * @brief method contain the receive loop
     * this method run in a thread
     */
    void run_client();

    /**
     * @brief method called when the current object is a timeline
     * @note Currently only data::matrix_tl and data::frame_tl are managed
     */
    void manage_timeline(data::object::sptr _obj, std::size_t _index);

    /// client socket
    sight::io::igtl::client m_client;

    /// Future used to wait for the client
    std::future<void> m_client_future;

    /// hostname config key
    std::string m_hostname_config;

    /// port config key
    std::string m_port_config;

    bool m_tl_initialized {false};

    /// Vector of device name used
    std::vector<std::string> m_device_names;

    data::ptr_vector<data::object, sight::data::access::inout> m_objects {this, "objects"};
};

} // namespace sight::module::io::igtl
