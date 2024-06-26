/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "io/igtl/client.hpp"
#include <sight/io/igtl/config.hpp>
#include "io/igtl/network.hpp"
// Patched header.
#include "io/igtl/patch/igtlSocket.h"

#include <core/exception.hpp>
#include <core/mt/types.hpp>

#include <igtlServerSocket.h>

#include <list>
#include <string>
#include <vector>
#include <optional>

namespace sight::io::igtl
{

/**
 *
 * @brief a network server class use igtl::ServerSocket
 */
class SIGHT_IO_IGTL_CLASS_API server : public io::igtl::network
{
public:

    using sptr = std::shared_ptr<server>;

    /**
     * @brief constructor
     */
    SIGHT_IO_IGTL_API server();

    /**
     * @brief destructor
     */
    SIGHT_IO_IGTL_API ~server() override;

    /**
     * @brief method to start the server on a port
     *
     * @param[in] _port the port to listen
     */
    SIGHT_IO_IGTL_API void start(std::uint16_t _port);

    /**
     * @brief method to wait a connection
     *
     * @param[in] _msec time to wait in millisecond
     */
    SIGHT_IO_IGTL_API client::sptr wait_for_connection(int _msec = 0);

    /**
     * @brief method to stop the server
     */
    SIGHT_IO_IGTL_API void stop();

    /**
     * @brief method to test if the server started
     */
    SIGHT_IO_IGTL_API bool started() const;

    /**
     * @brief method to broadcast to all client the obj
     */
    SIGHT_IO_IGTL_API void broadcast(const data::object::csptr& _obj);

    /**
     * @brief method to broadcast to all client a msg
     */
    SIGHT_IO_IGTL_API void broadcast(::igtl::MessageBase::Pointer _msg);

    /**
     * @brief get the port
     *
     * @return the port listened by server instance
     */
    SIGHT_IO_IGTL_API std::uint16_t get_port() const;

    /**
     * @brief method to run server and start event loop of server
     */
    SIGHT_IO_IGTL_API void run_server();

    /**
     * @brief method to have the current number of clients
     */
    SIGHT_IO_IGTL_API std::size_t num_clients() const;

    /**
     * @brief method to receive all headers of all connected clients
     *
     * @return vector of igl::MessageHeader::Pointer
     */
    SIGHT_IO_IGTL_API std::vector< ::igtl::MessageHeader::Pointer> receive_headers();

    /** @brief receive body pack of a specific connected client
     *
     *  @param[in] _header msg header
     *  @param[in] _client num
     *
     *  @return Message
     */
    SIGHT_IO_IGTL_API ::igtl::MessageBase::Pointer receive_body(
        ::igtl::MessageHeader::Pointer _header,
        unsigned int _client
    );

    /**
     * @brief method to retrieve vector of received object from all connected clients
     *
     * @return a vector of smart pointer of fwData object
     */
    SIGHT_IO_IGTL_API std::vector<data::object::sptr> receive_objects(std::vector<std::string>& _device_names);

    /**
     * @brief set the device name when a message is sent
     */
    SIGHT_IO_IGTL_API void set_message_device_name(const std::string& _device_name);

    /// Sets the receive timeout in ms.
    inline void set_receive_timeout(std::optional<unsigned int> _timeout);

    /// Gets the current receive timeout.
    inline std::optional<int> get_receive_timeout() const;

private:

    /// Patched version of igtlServer::CreateServer.
    int create_server(std::uint16_t _port);

    static void remove_client(client::sptr _client);

    /// server socket
    ::igtl::ServerSocket::Pointer m_server_socket;

    ///mutex
    mutable core::mt::mutex m_mutex;

    /// state of server
    bool m_is_started {false};

    /// vector of clients
    std::vector<client::sptr> m_clients;

    /// Server port
    std::uint16_t m_port {0};

    /// integer constant for success
    static const int SUCCESS = 0;

    /// device name in the sended message
    std::string m_device_name_out;

    /// Optional timeout for receiving message from clients
    std::optional<unsigned int> m_receive_timeout;
};

//------------------------------------------------------------------------------

inline void server::set_receive_timeout(std::optional<unsigned int> _timeout)
{
    m_receive_timeout = _timeout;
}

//------------------------------------------------------------------------------

inline std::optional<int> server::get_receive_timeout() const
{
    return m_receive_timeout;
}

} // namespace sight::io::igtl
