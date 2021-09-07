/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "modules/io/igtl/INetworkListener.hpp"

#include <data/MatrixTL.hpp>

#include <io/igtl/Client.hpp>

#include <cstdint>

namespace sight::data
{

class Composite;

}

namespace sight::module::io::igtl
{

/**
 * @brief class for network client service use OpenIGTLink
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service type="sight::module::io::igtl::STDataListener">
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
 * - \b timeline [sight::data::MatrixTL]: Timeline used to store received matrix.
 * @subsection Configuration Configuration:
 * - \b deviceName(optional) : filter by device Name in Message, by default all messages will be processed
 * - \b TData : specified IGTL matrix name to push in specified timeline index
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname, port and deviceName of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>, <deviceName>%DEVICE_NAME%</deviceName>)
 */
class MODULE_IO_IGTL_CLASS_API STDataListener : public module::io::igtl::INetworkListener
{
public:

    SIGHT_DECLARE_SERVICE(STDataListener, module::io::igtl::INetworkListener);

    /// Constructor
    MODULE_IO_IGTL_API STDataListener();

    /// Destructor
    MODULE_IO_IGTL_API virtual ~STDataListener();

protected:

    /// Configure port, hostname and device name
    MODULE_IO_IGTL_API void configuring() override;

    /// Start the client and try to connect to the server specify in configuration
    MODULE_IO_IGTL_API void starting() override;

    /// Disconnect the client from the server
    MODULE_IO_IGTL_API void stopping() override;

private:

    typedef std::map<std::string, unsigned long> MatrixNameIndexType;

    ///Push received matrices in timeline
    void manageTimeline(const SPTR(data::Composite)& obj, double timestamp = 0);

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void runClient();

    /// Future used to wait for the client
    std::future<void> m_clientFuture;

    /// client socket
    sight::io::igtl::Client m_client;

    /// hostname config key
    std::string m_hostnameConfig;

    /// port config key
    std::string m_portConfig;

    /// device names key
    std::vector<core::runtime::ConfigurationElement::sptr> m_deviceNamesConfig;

    MatrixNameIndexType m_matrixNameIndex;

    data::ptr<data::MatrixTL, sight::data::Access::inout> m_timeline {this, "timeline"};
};

} // namespace sight::module::io::igtl
