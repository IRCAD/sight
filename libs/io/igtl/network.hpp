/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "io/igtl/config.hpp"
// Patched header.
#include "io/igtl/patch/igtlSocket.h"

#include <core/exception.hpp>

#include <data/object.hpp>

#include <igtlMessageHeader.h>
#include <igtlSocket.h>

#include <set>
#include <string>

namespace sight::io::igtl
{

/**
 *
 * @brief a interface for client and server classes
 */
class IO_IGTL_CLASS_API network
{
public:

    /**
     * @brief default constructor
     */
    IO_IGTL_API network();

    /**
     * @brief Destructor if a connection is opened the destructor close it
     */
    IO_IGTL_API virtual ~network();

    /**
     * @brief generic method to receive object the type of the object is determined by response header
     * this method call the correct receiver for the type found in response header
     *
     * @return a smart pointer of fwData object
     */
    IO_IGTL_API data::object::sptr receive_object(std::string& _device_name);

    /**
     * @brief generic method to receive object
     * the type of the object is determined by response header
     * this method calls the correct receiver for the type found in response header and sets the timestamp
     * parameter according to the timestamp set in the message
     *
     * @return a smart pointer of fwData object
     */
    IO_IGTL_API data::object::sptr receive_object(std::string& _device_name, double& _timestamp);

    /**
     * @brief generic method to send a object the type of object is determined by classname
     *        this method call the correct sender method. If the client is not connected you receive
     *        a sigpipe signal
     * @param[in] dest object to send
     */
    IO_IGTL_API bool send_object(const data::object::csptr& _dest);

    /**
     * @brief generic method to send a igtl Msg, this method is useful for redirect message
     * @param[in] msg message to send
     */
    IO_IGTL_API bool send_msg(::igtl::MessageBase::Pointer _msg);

    /**
     * @brief receive header
     * @throw igtl::exception on error (network error or timeout).
     * @return header
     */

    IO_IGTL_API ::igtl::MessageHeader::Pointer receive_header();

    /** @brief receive body pack
     *
     *  @param[in] header msg header
     *  @throw igtl::exception on error (network error or timeout).
     *  @return Message
     */
    IO_IGTL_API ::igtl::MessageBase::Pointer receive_body(::igtl::MessageHeader::Pointer _header);

    /**
     * @brief get socket
     *
     * @return socket
     */
    [[nodiscard]] IO_IGTL_API ::igtl::Socket::Pointer get_socket() const;

    /**
     * @brief add a new authorized device name
     * @param[in] std::string device name
     */
    IO_IGTL_API void add_authorized_device(const std::string& _device_name);

    /**
     * @brief get filteringByDeviceName (true if activated, false otherwise)
     *
     * @return boolean
     */
    [[nodiscard]] IO_IGTL_API bool get_filtering_by_device_name() const;

    /**
     * @brief activate/desactivate the filtering by device name
     * note that if add_authorized_device() is never called this option is automatically disabled
     *
     * @param[in] boolean
     */
    IO_IGTL_API void set_filtering_by_device_name(bool _filtering);

    /**
     * @brief set the device name when a message is sent
     */
    IO_IGTL_API void set_device_name_out(const std::string& _device_name);

    /**
     * @brief get the device name when a message is sent
     *
     * return std::string
     */
    [[nodiscard]] IO_IGTL_API std::string get_device_name_out() const;

protected:

    /// Patched methods from igtl.

    /// Closes the socket (from igtlSocket::CloseSocket()).
    /// Patched version: Added a close() after shutdown() on Linux.
    static void close_socket(int _socket_descriptor);
    /// Creates an endpoint for communication and returns the descriptor.
    /// -1 indicates error.
    static int create_socket();
    /// Listen for connections on a socket. Returns 0 on success. -1 on error.
    static int listen_socket(int _socket_descriptor);
    /// Binds socket to a particular port.
    /// Returns 0 on success other -1 is returned.
    /// Patched version: Doesn't rely on VTK_HAVE_SO_REUSEADDR to add option SO_REUSEADDR.
    static int bind_socket(int _socket_descriptor, std::uint16_t _port);

    /// client socket
    ::igtl::Socket::Pointer m_socket;

    /// filter the message by device name
    bool m_filtering_by_device_name {false};

    /// Set of authorized device names
    std::set<std::string> m_device_names_in;

    /// device name in the sent message
    std::string m_device_name_out;
};

} // namespace sight::io::igtl
