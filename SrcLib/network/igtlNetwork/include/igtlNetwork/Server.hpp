/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "igtlNetwork/Client.hpp"
#include "igtlNetwork/config.hpp"
#include "igtlNetwork/INetwork.hpp"

#include <fwCore/Exception.hpp>
#include <fwCore/mt/types.hpp>

#include <igtlServerSocket.h>

#include <list>
#include <string>
#include <vector>

namespace igtlNetwork
{

/**
 *
 * @brief a network server class use igtl::ServerSocket
 */
class IGTLNETWORK_CLASS_API Server : public ::igtlNetwork::INetwork
{
public:

    typedef SPTR (Server) sptr;

    /**
     * @brief constructor
     */
    IGTLNETWORK_API Server();

    /**
     * @brief destructor
     */
    IGTLNETWORK_API ~Server();

    /**
     * @brief method to start the server on a port
     *
     * @param[in] port the port to listen
     */
    IGTLNETWORK_API void start(std::uint16_t port);

    /**
     * @brief method to wait a connection
     *
     * @param[in] msec time to wait in millisecond
     */
    IGTLNETWORK_API Client::sptr waitForConnection (int msec = 0);

    /**
     * @brief method to stop the server
     */
    IGTLNETWORK_API void stop();

    /**
     * @brief method to test if the server started
     */
    IGTLNETWORK_API bool isStarted() const;

    /**
     * @brief method to broadcast to all client the obj
     */
    IGTLNETWORK_API void broadcast(const ::fwData::Object::csptr& obj);

    /**
     * @brief method to broadcast to all client a msg
     */
    IGTLNETWORK_API void broadcast(::igtl::MessageBase::Pointer msg);

    /**
     * @brief get the port
     *
     * @return the port listened by server instance
     */
    IGTLNETWORK_API std::uint16_t getPort() const;

    /**
     * @brief method to run server and start event loop of server
     */
    IGTLNETWORK_API void runServer();

    /**
     * @brief method to have the current number of clients
     */
    IGTLNETWORK_API size_t getNumberOfClients() const;

    /**
     * @brief method to receive all headers of all connected clients
     *
     * @return vector of igl::MessageHeader::Pointer
     */
    IGTLNETWORK_API std::vector< ::igtl::MessageHeader::Pointer > receiveHeaders();

    /** @brief receive body pack of a specific connected client
     *
     *  @param[in] header msg header
     *  @param[in] client num
     *
     *  @return Message
     */
    IGTLNETWORK_API ::igtl::MessageBase::Pointer receiveBody(::igtl::MessageHeader::Pointer header,
                                                             unsigned int client);

    /**
     * @brief method to retrieve vector of received object from all connected clients
     *
     * @return a vector of smart pointer of fwData object
     */
    IGTLNETWORK_API std::vector< ::fwData::Object::sptr > receiveObjects(std::vector<std::string>& deviceNames);

    /**
     * @brief set the device name when a message is sent
     */
    IGTLNETWORK_API void setMessageDeviceName(const std::string& deviceName);

private:

    /// server socket
    ::igtl::ServerSocket::Pointer m_serverSocket;

    ///mutex
    mutable ::fwCore::mt::Mutex m_mutex;

    /// state of server
    bool m_isStarted;

    /// vector of clients
    std::vector< Client::sptr > m_clients;

    /// Server port
    std::uint16_t m_port;

    /// integer constant for success
    static const int s_SUCCESS = 0;

    /// device name in the sended message
    std::string m_deviceNameOut;

};

}
