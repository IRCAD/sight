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
#include "modules/io/igtl/INetworkSender.hpp"

#include <data/Object.hpp>

#include <io/igtl/Client.hpp>
#include <io/igtl/Server.hpp>

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
 * <service uid="..." type="::sight::module::io::igtl::SServerSender" autoConnect="yes" >
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
 * - \b objects [sight::data::Object]: specified objects to send.
 * They must have an attribute 'deviceName' to know the device-name used for this specific data.
 **/

class MODULE_IO_IGTL_CLASS_API SServerSender : public module::io::igtl::INetworkSender
{

public:

    fwCoreServiceMacro(SServerSender,  module::io::igtl::INetworkSender )

    /// Constructor
    MODULE_IO_IGTL_API SServerSender();

    /// Destructor
    MODULE_IO_IGTL_API virtual ~SServerSender();

protected:

    /// Configure port and device name
    MODULE_IO_IGTL_API virtual void configuring() override;

    /// Start the server.
    MODULE_IO_IGTL_API virtual void starting() override;

    /// Stop the server
    MODULE_IO_IGTL_API virtual void stopping() override;

private:

    /**
     * @brief method to send data.
     *
     * @param[in] obj obj to send
     * @param[in] index index of the object in the group
     */
    void sendObject(const data::Object::csptr& obj, const size_t index) override;

    /// Server instance
    sight::io::igtl::Server::sptr m_server;

    /// Future used to wait for the server
    std::future<void> m_serverFuture;

    /// port preference key
    std::string m_portConfig;

    /// Vector of device name if the group configuration exists.
    std::vector< std::string > m_deviceNames;

};

} // namespace sight::module::io::igtl
