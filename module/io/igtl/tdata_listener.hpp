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

#include <data/matrix_tl.hpp>

#include <io/igtl/client.hpp>

#include <cstdint>

namespace sight::data
{

class map;

} // namespace sight::data

namespace sight::module::io::igtl
{

/**
 * @brief class for network client service use OpenIGTLink
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service type="sight::module::io::igtl::tdata_listener">
 *      <inout key="timeline" uid="..." />
 *      <server>127.0.0.1:4242</server>
 *      <deviceName>Name1</deviceName>
 *      <deviceName>...</deviceName>
 *      <TData>
 *          <matrix name="matrix_name" index="0" />
 *          <matrix name="..." index="..." />
 *      </TData>
 *      ...
 * </service>
 * @endcode
 * @subsection In-Out In-Out:
 * - \b timeline [sight::data::matrix_tl]: Timeline used to store received matrix.
 * @subsection Configuration Configuration:
 * - \b deviceName(optional) : filter by device Name in Message, by default all messages will be processed
 * - \b TData : specified IGTL matrix name to push in specified timeline index
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname, port and deviceName of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>, <deviceName>%DEVICE_NAME%</deviceName>)
 */
class tdata_listener : public module::io::igtl::network_listener
{
public:

    SIGHT_DECLARE_SERVICE(tdata_listener, module::io::igtl::network_listener);

    tdata_listener()           = default;
    ~tdata_listener() override = default;

protected:

    /// Configure port, hostname and device name
    void configuring() override;

    /// Start the client and try to connect to the server specify in configuration
    void starting() override;

    /// Disconnect the client from the server
    void stopping() override;

private:

    using matrix_name_index_t = std::map<std::string, std::uint64_t>;

    ///Push received matrices in timeline
    void manage_timeline(const SPTR(data::map)& _obj, double _timestamp = 0);

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void run_client();

    /// Future used to wait for the client
    std::future<void> m_client_future;

    /// client socket
    sight::io::igtl::client m_client;

    /// hostname config key
    std::string m_hostname_config;

    /// port config key
    std::string m_port_config;

    /// device names key
    std::vector<std::string> m_device_names_config;

    matrix_name_index_t m_matrix_name_index;

    data::ptr<data::matrix_tl, sight::data::access::inout> m_timeline {this, "timeline"};
};

} // namespace sight::module::io::igtl
