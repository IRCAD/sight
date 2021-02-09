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

#include <data/Object.hpp>

#include <io/igtl/Client.hpp>

#include <future>
#include <string>

namespace sight::modules::io::igtl
{

/**
 * @brief   OpenIGTLink client that will listen objects to the connected server
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="::sight::modules::io::igtl::SClientListener" >
 *      <server>127.0.0.1:4242</server>
 *      <inout group="objects">
 *          <key uid="..." deviceName="device01" />
 *          <key uid="..." deviceName="device02" />
 *      </inout>
 * </service>
 * @endcode
 * @subsection In-Out In-Out:
 * - \b object [data::Object]:
 *   - if associated object is a timeline (data::MatrixT or data::FrameTL):
 *   received IGTL data are pushed in timeline
 *   - else : object is updated with received IGTL data
 * @subsection Configuration Configuration:
 * - \b deviceName: filter by device Name in Message
 * - \b server: server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 *  (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class MODULE_IO_IGTL_CLASS_API SClientListener : public modules::io::igtl::INetworkListener
{

public:

    fwCoreServiceMacro(SClientListener, modules::io::igtl::INetworkListener)

    /// Constructor
    MODULE_IO_IGTL_API SClientListener();

    /// Destructor
    MODULE_IO_IGTL_API virtual ~SClientListener();

protected:

    /// Configure port, hostname and device name
    MODULE_IO_IGTL_API virtual void configuring() override;

    /// start the client and try to connect to the server specify in configuration
    MODULE_IO_IGTL_API virtual void starting() override;

    /// disconnect the client from the server
    MODULE_IO_IGTL_API virtual void stopping() override;

private:

    /**
     * @brief method contain the receive loop
     * this method run in a thread
     */
    void    runClient();

    /**
     * @brief method called when the current object is a timeline
     * @note Currently only data::MatrixTL and data::FrameTL are managed
     */
    void manageTimeline(data::Object::sptr obj, size_t index);

    /// client socket
    sight::io::igtl::Client m_client;

    /// Future used to wait for the client
    std::future<void> m_clientFuture;

    /// hostname config key
    std::string m_hostnameConfig;

    /// port config key
    std::string m_portConfig;

    bool m_tlInitialized;

    /// Vector of device name used
    std::vector<std::string> m_deviceNames;
};

} // namespace sight::modules::io::igtl
