/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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
#include "io/igtl/INetwork.hpp"

#include <core/Exception.hpp>

#include <data/Object.hpp>

#include <igtlClientSocket.h>
#include <igtlMessageHeader.h>

#include <string>

namespace sight::io::igtl
{

/**
 *
 * @brief A Network client using igtl::ClientSocket can send fwData primitive
 */
class IO_IGTL_CLASS_API Client : public io::igtl::INetwork
{
public:

    typedef SPTR(Client) sptr;

    /**
     * @brief default constructor
     */
    IO_IGTL_API Client();

    /**
     * @brief Constructor take a igtl::ClientSocket used only by server class.
     *        You should not use this constructor, use default constructor instead
     */
    IO_IGTL_API Client(::igtl::ClientSocket::Pointer socket);

    /**
     * @brief Destructor if a connection is opened the destructor close it
     */
    IO_IGTL_API ~Client();

    /**
     * @brief method to connect client to a server by using hostname and port
     *
     * @param[in] addr ip or hostname
     * @param[in] port port of server
     */
    IO_IGTL_API void connect(const std::string& addr, std::uint16_t port);

    /**
     * @brief method to disconnect client
     */
    IO_IGTL_API void disconnect();

    /**
     * @brief method to check if the client is connected
     *
     * @return a boolean to say if the client is connected or not
     */
    IO_IGTL_API bool isConnected() const;

private:

    /**
     * @brief utility method to avoid code duplication
     * @param[in] msg exception message
     * @param[in] result throw the exception if result=true
     */
    void throwExceptionIfFailed(const std::string& msg, bool result);
};

} // namespace sight::io::igtl
