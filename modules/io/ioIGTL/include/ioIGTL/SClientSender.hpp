/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioIGTL/config.hpp"

#include <igtlNetwork/Client.hpp>

#include <ioNetwork/INetworkSender.hpp>

namespace ioIGTL
{

/**
 * @brief OpenIGTLink client that will send objects to the connected server
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="::ioIGTL::SClientSender" autoConnect="yes">
 *      <server>127.0.0.1:4242</server>
 *      <in group="objects">
 *           <key uid="..." deviceName="device01" />
 *           <key uid="..." deviceName="device02" />
 *      </in>
 * </service>
 * @endcode
 * @subsection Input Input:
 * - \b objects [::fwData::Object]: specified objects to send to the connected server.
 * They must have an attribute 'deviceName' to known the device-name used for this specific data.
 * @subsection Configuration Configuration:
 * - \b deviceName : filter by device Name in Message
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class IOIGTL_CLASS_API SClientSender : public ::ioNetwork::INetworkSender
{

public:

    fwCoreServiceMacro(SClientSender, ::ioNetwork::INetworkSender);

    /// Constructor
    IOIGTL_API SClientSender();

    /// Destructor
    IOIGTL_API virtual ~SClientSender();

protected:

    /// Configures the service.
    IOIGTL_API virtual void configuring() override;

    /**
     * @brief start the client and try to connect to the server specify in configuration
     */
    IOIGTL_API virtual void starting() override;

    /**
     * @brief disconnect the client from the server
     */
    IOIGTL_API virtual void stopping() override;

private:

    /**
     * @brief method to send data.
     *
     * @param[in] obj obj to send
     * @param[in] index index of the object in the group
     */
    void sendObject(const ::fwData::Object::csptr& obj, const size_t index) override;

    /// client socket
    ::igtlNetwork::Client m_client;

    /// hostname config key
    std::string m_hostnameConfig;

    /// port config key
    std::string m_portConfig;

    /// Vector of device name used
    std::vector< std::string > m_deviceNames;
};

} // namespace ioIGTL
